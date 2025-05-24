#include <not_implemented.h>
#include <cstddef>
#include "../include/allocator_buddies_system.h"
#include <sstream>

allocator_buddies_system::~allocator_buddies_system()
{
    if (_trusted_memory == nullptr) return;
    debug_with_guard("allocator destructor started");

    auto byte_ptr = reinterpret_cast<std::byte*>(_trusted_memory);
    auto parent_allocator = *reinterpret_cast<std::pmr::memory_resource**>(byte_ptr + sizeof(logger*));

    if (parent_allocator == nullptr) parent_allocator = std::pmr::get_default_resource();

    size_t total_size = get_general_size(_trusted_memory) + allocator_metadata_size;
    parent_allocator->deallocate(_trusted_memory, total_size);
}

allocator_buddies_system::allocator_buddies_system(
        allocator_buddies_system &&other) noexcept: _trusted_memory(std::exchange(other._trusted_memory, nullptr)) {}

allocator_buddies_system &allocator_buddies_system::operator=(
        allocator_buddies_system &&other) noexcept
{
    if (this != &other)
    {
        std::swap(_trusted_memory, other._trusted_memory);
    }
    return *this;
}

allocator_buddies_system::allocator_buddies_system(
        size_t space_size,
        std::pmr::memory_resource *parent_allocator,
        logger *logger,
        allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    if (power_of_two(space_size) <= allocator_metadata_size)  throw std::logic_error("space_size too small");

    std::pmr::memory_resource* alloc = parent_allocator ? parent_allocator: std::pmr::get_default_resource();

    size_t needed = power_of_two(space_size) + allocator_metadata_size;

    _trusted_memory = alloc->allocate(needed);

    if (_trusted_memory == nullptr) throw std::bad_alloc();


    auto logger_ptr = reinterpret_cast<class logger**>(_trusted_memory);
    *logger_ptr = logger;

    auto parent_alloc_ptr = reinterpret_cast<std::pmr::memory_resource**>(logger_ptr + 1);
    *parent_alloc_ptr = alloc;

    auto fit_mode_ptr = reinterpret_cast<allocator_with_fit_mode::fit_mode*>(parent_alloc_ptr + 1);
    *fit_mode_ptr = allocate_fit_mode;

    auto size_ptr = reinterpret_cast<unsigned char*>(fit_mode_ptr + 1);
    *size_ptr = space_size;

    auto mutex_ptr = reinterpret_cast<std::mutex*>(size_ptr + 1);
    new (mutex_ptr) std::mutex();

    auto start = reinterpret_cast<block_metadata*>(reinterpret_cast<std::byte*>(_trusted_memory) + allocator_metadata_size);

    start->occupied = false;
    start->size = __detail::nearest_greater_k_of_2(power_of_two(space_size));

    std::stringstream addr_stream;
    addr_stream << _trusted_memory;
    debug_with_guard("allocator created with size " + std::to_string(get_general_size(_trusted_memory)) + " at " + addr_stream.str());
}

[[nodiscard]] void *allocator_buddies_system::do_allocate_sm(
        size_t size)
{
    std::lock_guard<std::mutex> lock(get_mutex(_trusted_memory));

    size_t needed = power_of_two(__detail::nearest_greater_k_of_2(size + occupied_block_metadata_size));

    void* free_block_ptr = nullptr;
    switch (get_fit_mode(_trusted_memory))
    {
        case allocator_with_fit_mode::fit_mode::first_fit:
            free_block_ptr = get_first(needed);
            break;
        case allocator_with_fit_mode::fit_mode::the_best_fit:
            free_block_ptr = get_best(needed);
            break;
        case allocator_with_fit_mode::fit_mode::the_worst_fit:
            free_block_ptr = get_worst(needed);
            break;
    }

    if (free_block_ptr == nullptr)
    {
        debug_with_guard("failed to allocate " + std::to_string(needed) + " bytes");
        throw std::bad_alloc();
    }

    while (get_block_size(free_block_ptr) >= needed * 2)
    {
        --(reinterpret_cast<block_metadata*>(free_block_ptr)->size);
        auto cur_buddy = get_buddy(free_block_ptr);
        reinterpret_cast<block_metadata*>(cur_buddy)->size = reinterpret_cast<block_metadata*>(free_block_ptr)->size;
        reinterpret_cast<block_metadata*>(cur_buddy)->occupied = false;
    }

    auto free_metadata = reinterpret_cast<block_metadata*>(free_block_ptr);
    free_metadata->occupied = true;

    auto parent_ptr = reinterpret_cast<void**>(free_metadata + 1);
    *parent_ptr = _trusted_memory;

    std::stringstream addr_stream1;
    addr_stream1 << free_block_ptr;

    std::stringstream addr_stream2;
    addr_stream2 << (reinterpret_cast<std::byte*>(free_block_ptr) - (reinterpret_cast<std::byte*>(_trusted_memory) + allocator_metadata_size));

    debug_with_guard("allocator allocated " + std::to_string(needed) + " bytes at " + addr_stream1.str() + " (" + addr_stream2.str() + ")");
    return reinterpret_cast<std::byte*>(free_block_ptr) + occupied_block_metadata_size;
}

void allocator_buddies_system::do_deallocate_sm(void *at)
{
    std::lock_guard lock(get_mutex(_trusted_memory));
    std::cout << reinterpret_cast<block_metadata*>(at)->occupied << std::endl;
    void* block_start = reinterpret_cast<std::byte*>(at) - occupied_block_metadata_size;

    if (*reinterpret_cast<void**>(reinterpret_cast<std::byte*>(block_start) + sizeof(block_metadata)) != _trusted_memory)
    {
        error_with_guard("get wrong deallocation object");
        throw std::logic_error("get wrong deallocation object");
    }

    size_t block_size = get_block_size(block_start) - occupied_block_metadata_size;

    debug_with_guard(get_dump((char*)at, block_size));

    reinterpret_cast<block_metadata*>(block_start)->occupied = false;

    void* buddy = get_buddy(block_start);

    while(get_block_size(block_start) < get_general_size(_trusted_memory) && get_block_size(block_start) == get_block_size(buddy) && !reinterpret_cast<block_metadata*>(buddy)->occupied)
    {
        void* interested_ptr = block_start < buddy ? block_start : buddy;

        auto metadata = reinterpret_cast<block_metadata*>(interested_ptr);
        ++metadata->size;

        block_start = interested_ptr;
        buddy = get_buddy(block_start);
    }
    debug_with_guard("deallocated " + std::to_string(block_size) + " bytes and merged to" + std::to_string(get_block_size(block_start)));

}

bool allocator_buddies_system::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    if (this == &other) return true;
    auto casted_other = dynamic_cast<const allocator_buddies_system*>(&other);
    return &casted_other->get_mutex(_trusted_memory) == &get_mutex(_trusted_memory)
           && casted_other->get_logger() == get_logger()
           && casted_other->get_fit_mode(_trusted_memory) == get_fit_mode(_trusted_memory);
}

inline void allocator_buddies_system::set_fit_mode(
        allocator_with_fit_mode::fit_mode mode)
{
    auto ptr = reinterpret_cast<allocator_with_fit_mode::fit_mode*>(reinterpret_cast<std::byte*>(_trusted_memory) + sizeof(logger*) + sizeof(allocator_dbg_helper*));
    *ptr = mode;
}



inline std::string allocator_buddies_system::get_typename() const
{
    return std::string("allocator_buddies_system");
}

std::vector<allocator_test_utils::block_info> allocator_buddies_system::get_blocks_info() const noexcept
{
    return get_blocks_info_inner();
}

std::vector<allocator_test_utils::block_info> allocator_buddies_system::get_blocks_info_inner() const
{
    std::vector<allocator_test_utils::block_info> result;
    for (auto it = begin(); it != end(); ++it)
    {
        result.push_back({it.size(), it.occupied()});
    }

    return result;
}

allocator_buddies_system::buddy_iterator allocator_buddies_system::begin() const noexcept
{
    return {reinterpret_cast<std::byte*>(_trusted_memory) + allocator_metadata_size};
}

allocator_buddies_system::buddy_iterator allocator_buddies_system::end() const noexcept
{
    return {reinterpret_cast<std::byte*>(_trusted_memory) + allocator_metadata_size + get_general_size(_trusted_memory)};
}

bool allocator_buddies_system::buddy_iterator::operator==(const allocator_buddies_system::buddy_iterator &other) const noexcept
{
    return _block == other._block;
}

bool allocator_buddies_system::buddy_iterator::operator!=(const allocator_buddies_system::buddy_iterator &other) const noexcept
{
    return _block != other._block;
}

allocator_buddies_system::buddy_iterator &allocator_buddies_system::buddy_iterator::operator++() & noexcept
{
    _block = reinterpret_cast<std::byte*>(_block) + power_of_two(reinterpret_cast<block_metadata*>(_block)->size);
    return *this;
}

allocator_buddies_system::buddy_iterator allocator_buddies_system::buddy_iterator::operator++(int n)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

size_t allocator_buddies_system::buddy_iterator::size() const noexcept
{
    return power_of_two(reinterpret_cast<block_metadata*>(_block)->size);
}

bool allocator_buddies_system::buddy_iterator::occupied() const noexcept
{
    return reinterpret_cast<block_metadata*>(_block)->occupied;
}

void *allocator_buddies_system::buddy_iterator::operator*() const noexcept
{
    return _block;
}

allocator_buddies_system::buddy_iterator::buddy_iterator(void *start): _block(start) {}

allocator_buddies_system::buddy_iterator::buddy_iterator(): _block(nullptr) {}

void *allocator_buddies_system::get_first(size_t size) noexcept
{
    for(auto it = begin(), sent = end(); it != sent; ++it)
    {
        if (!it.occupied() && it.size() >= size)
        {
            return *it;
        }
    }
    return nullptr;
}

void *allocator_buddies_system::get_best(size_t size) noexcept
{
    buddy_iterator res;

    for(auto it = begin(), sent = end(); it != sent; ++it)
    {
        if (!it.occupied() && it.size() >= size && (it.size() < res.size() || *res == nullptr))
        {
            res = it;
        }
    }

    return *res;
}

void *allocator_buddies_system::get_worst(size_t size) noexcept
{
    buddy_iterator res;
    size_t res_size = 0;
    for(auto it = begin(), sent = end(); it != sent; ++it)
    {
        if (!it.occupied() && it.size() >= size && (it.size() > res_size || *res == nullptr))
        {
            res_size = it.size();
            res = it;
        }
    }

    return *res;
}


size_t allocator_buddies_system::get_block_size(void* block) const noexcept {
    return power_of_two(reinterpret_cast<block_metadata*>(block)->size);
}

void *allocator_buddies_system::get_buddy(void *cur_block) {
    size_t block_size = get_block_size(cur_block);
    size_t block_offset = reinterpret_cast<std::byte*>(cur_block) - (reinterpret_cast<std::byte*>(_trusted_memory) + allocator_metadata_size);
    size_t buddy_offset = block_offset ^ block_size;
    return reinterpret_cast<std::byte*>(_trusted_memory) + allocator_metadata_size + buddy_offset;
}

size_t allocator_buddies_system::power_of_two(size_t size)
{
    return 1 << size;
}

inline logger* allocator_buddies_system::get_logger() const
{
    return *reinterpret_cast<logger**>(_trusted_memory);
}

size_t allocator_buddies_system::get_general_size(void *trusted_memory)
{
    auto ptr = reinterpret_cast<std::byte*>(trusted_memory) + sizeof(logger*) + sizeof(allocator_dbg_helper*) + sizeof(fit_mode);
    return power_of_two(*reinterpret_cast<size_t*>(ptr));

}

allocator_dbg_helper* allocator_buddies_system::get_parent(void *trusted_memory)
{
    auto ptr = reinterpret_cast<std::byte*>(trusted_memory) + sizeof(logger*);
    return *reinterpret_cast<allocator_dbg_helper**>(ptr);
}

std::mutex& allocator_buddies_system::get_mutex(void *trusted_memory)
{
    auto ptr = reinterpret_cast<std::byte*>(trusted_memory) + sizeof(logger*) + sizeof(allocator_dbg_helper*) + sizeof(fit_mode) + sizeof(unsigned char);
    return *reinterpret_cast<std::mutex*>(ptr);
}

allocator_with_fit_mode::fit_mode allocator_buddies_system::get_fit_mode(void *trusted_memory)
{
    auto ptr = reinterpret_cast<std::byte*>(trusted_memory) + sizeof(logger*) + sizeof(allocator_dbg_helper*);
    return *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(ptr);
}
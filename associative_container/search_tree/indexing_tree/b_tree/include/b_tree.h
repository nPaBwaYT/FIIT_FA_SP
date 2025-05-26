#ifndef MP_OS_B_TREE_H
#define MP_OS_B_TREE_H

#include <logger_guardant.h>
#include <pp_allocator.h>
#include <search_tree.h>

#include <boost/container/static_vector.hpp>
#include <initializer_list>
#include <iterator>
#include <stack>
#include <utility>
#include <functional>

template<typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5>
class B_tree final : private logger_guardant, private compare {
public:
    using tree_data_type = std::pair<tkey, tvalue>;
    using tree_data_type_const = std::pair<tkey, tvalue>;
    using value_type = tree_data_type_const;

private:
    static constexpr const size_t minimum_keys_in_node = t - 1;
    static constexpr const size_t maximum_keys_in_node = 2 * t - 1;


    inline bool compare_keys(const tkey& lhs, const tkey& rhs) const;
    inline bool compare_pairs(const tree_data_type& lhs, const tree_data_type& rhs) const;



    struct btree_node {
        boost::container::static_vector<tree_data_type, maximum_keys_in_node + 1> _keys;
        boost::container::static_vector<btree_node*, maximum_keys_in_node + 2> _pointers;
        btree_node() noexcept;

    };

    pp_allocator<value_type> _allocator;
    logger* _logger;
    btree_node* _root;
    size_t _size;

    logger* get_logger() const noexcept override;
    pp_allocator<value_type> get_allocator() const noexcept;

public:

    explicit B_tree(const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    explicit B_tree(pp_allocator<value_type> alloc, const compare& comp = compare(), logger* logger = nullptr);

    template<input_iterator_for_pair<tkey, tvalue> iterator>
    explicit B_tree(iterator begin, iterator end, const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    B_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);



    B_tree(const B_tree& other);

    B_tree(B_tree&& other) noexcept;

    B_tree& operator=(const B_tree& other);

    B_tree& operator=(B_tree&& other) noexcept;

    ~B_tree() noexcept override;



    class btree_iterator;
    class btree_reverse_iterator;
    class btree_const_iterator;
    class btree_const_reverse_iterator;

    class btree_iterator final {
        std::stack<std::pair<btree_node**, size_t>> _path;
        size_t _index;

    public:
        using value_type = tree_data_type_const;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_const_iterator;
        friend class btree_const_reverse_iterator;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_iterator(const std::stack<std::pair<btree_node**, size_t>>& path = std::stack<std::pair<btree_node**, size_t>>(), size_t index = 0);
    };

    class btree_const_iterator final {
        std::stack<std::pair<btree_node* const*, size_t>> _path;
        size_t _index;

    public:
        using value_type = tree_data_type_const;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_const_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_iterator;
        friend class btree_const_reverse_iterator;

        btree_const_iterator(const btree_iterator& it) noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_const_iterator(const std::stack<std::pair<btree_node* const*, size_t>>& path = std::stack<std::pair<btree_node* const*, size_t>>(), size_t index = 0);
    };

    class btree_reverse_iterator final {
        std::stack<std::pair<btree_node**, size_t>> _path;
        size_t _index;

    public:
        using value_type = tree_data_type_const;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_reverse_iterator;

        friend class B_tree;
        friend class btree_iterator;
        friend class btree_const_iterator;
        friend class btree_const_reverse_iterator;

        btree_reverse_iterator(const btree_iterator& it) noexcept;
        operator btree_iterator() const noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_reverse_iterator(const std::stack<std::pair<btree_node**, size_t>>& path = std::stack<std::pair<btree_node**, size_t>>(), size_t index = 0);
    };

    class btree_const_reverse_iterator final {
        std::stack<std::pair<btree_node* const*, size_t>> _path;
        size_t _index;

    public:
        using value_type = tree_data_type_const;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_const_reverse_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_const_iterator;
        friend class btree_iterator;

        btree_const_reverse_iterator(const btree_reverse_iterator& it) noexcept;
        operator btree_const_iterator() const noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_const_reverse_iterator(const std::stack<std::pair<btree_node* const*, size_t>>& path = std::stack<std::pair<btree_node* const*, size_t>>(), size_t index = 0);
    };

    friend class btree_iterator;
    friend class btree_const_iterator;
    friend class btree_reverse_iterator;
    friend class btree_const_reverse_iterator;



    /*
     * Returns a reference to the mapped value of the element with specified key. If no such element exists, an exception of type std::out_of_range is thrown.
     */
    tvalue& at(const tkey&);
    const tvalue& at(const tkey&) const;

    /*
     * If key not exists, makes default initialization of value
     */
    tvalue& operator[](const tkey& key);
    tvalue& operator[](tkey&& key);


    btree_iterator begin();
    btree_iterator end();

    btree_const_iterator begin() const;
    btree_const_iterator end() const;

    btree_const_iterator cbegin() const;
    btree_const_iterator cend() const;

    btree_reverse_iterator rbegin();
    btree_reverse_iterator rend();

    btree_const_reverse_iterator rbegin() const;
    btree_const_reverse_iterator rend() const;

    btree_const_reverse_iterator crbegin() const;
    btree_const_reverse_iterator crend() const;



    size_t size() const noexcept;
    bool empty() const noexcept;

    /*
     * Returns end() if not exist
     */

    btree_iterator find(const tkey& key);
    btree_const_iterator find(const tkey& key) const;

    btree_iterator lower_bound(const tkey& key);
    btree_const_iterator lower_bound(const tkey& key) const;

    btree_iterator upper_bound(const tkey& key);
    btree_const_iterator upper_bound(const tkey& key) const;

    bool contains(const tkey& key) const;



    void clear() noexcept;

    /*
     * Does nothing if key exists, delegates to emplace.
     * Second return value is true, when inserted
     */
    std::pair<btree_iterator, bool> insert(const tree_data_type& data);
    std::pair<btree_iterator, bool> insert(tree_data_type&& data);

    template<typename... Args>
    std::pair<btree_iterator, bool> emplace(Args&&... args);

    /*
     * Updates value if key exists, delegates to emplace.
     */
    btree_iterator insert_or_assign(const tree_data_type& data);
    btree_iterator insert_or_assign(tree_data_type&& data);

    template<typename... Args>
    btree_iterator emplace_or_assign(Args&&... args);

    /*
     * Return iterator to node next ro removed or end() if key not exists
     */
    btree_iterator erase(btree_iterator pos);
    btree_iterator erase(btree_const_iterator pos);

    btree_iterator erase(btree_iterator beg, btree_iterator en);
    btree_iterator erase(btree_const_iterator beg, btree_const_iterator en);


    btree_iterator erase(const tkey& key);


};

template<std::input_iterator iterator, compator<typename std::iterator_traits<iterator>::value_type::first_type> compare = std::less<typename std::iterator_traits<iterator>::value_type::first_type>,
        std::size_t t = 5, typename U>
B_tree(iterator begin, iterator end, const compare& cmp = compare(), pp_allocator<U> = pp_allocator<U>(),
       logger* logger = nullptr) -> B_tree<typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare, t>;

template<typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5, typename U>
B_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(), pp_allocator<U> = pp_allocator<U>(),
       logger* logger = nullptr) -> B_tree<tkey, tvalue, compare, t>;

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::compare_pairs(const B_tree::tree_data_type& lhs,
                                                     const B_tree::tree_data_type& rhs) const {
    return compare_keys(lhs.first, rhs.first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::compare_keys(const tkey& lhs, const tkey& rhs) const {
    return compare::operator()(lhs, rhs);
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_node::btree_node() noexcept {
    _keys.reserve(maximum_keys_in_node + 1);
    _pointers.reserve(maximum_keys_in_node + 2);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
logger* B_tree<tkey, tvalue, compare, t>::get_logger() const noexcept {
    return _logger;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
pp_allocator<typename B_tree<tkey, tvalue, compare, t>::value_type> B_tree<tkey, tvalue, compare, t>::get_allocator() const noexcept {
    return _allocator;
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(const compare& cmp, pp_allocator<value_type> alloc, logger* logger)
        : _allocator(alloc), _logger(logger), _root(nullptr), _size(0) {
    compare::operator=(cmp);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(pp_allocator<value_type> alloc, const compare& comp, logger* logger)
        : _allocator(alloc), _logger(logger), _root(nullptr), _size(0) {
    compare::operator=(comp);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<input_iterator_for_pair<tkey, tvalue> iterator>
B_tree<tkey, tvalue, compare, t>::B_tree(iterator begin, iterator end,
                                         const compare& cmp,
                                         pp_allocator<value_type> alloc,
                                         logger* logger) : _allocator(alloc), _logger(logger), _root(nullptr), _size(0) {
    compare::operator=(cmp);
    for (auto it = begin; it != end; ++it) {
        insert(*it);
    }
}
template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp,
                                         pp_allocator<value_type> alloc,
                                         logger* logger) : _allocator(alloc), _logger(logger), _root(nullptr), _size(0) {
    compare::operator=(cmp);
    for (const auto& pair: data) {
        insert(pair);
    }
}



template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::~B_tree() noexcept {
    clear();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(const B_tree& other) : _allocator(other._allocator),
                                                                _logger(other._logger),
                                                                _root(nullptr),
                                                                _size(0) {
    compare::operator=(other);
    for (const auto& pair: other) {
        insert(pair);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>& B_tree<tkey, tvalue, compare, t>::operator=(const B_tree& other) {
    if (this != &other) {
        clear();
        _allocator = other._allocator;
        _logger = other._logger;
        compare::operator=(other);
        for (const auto& pair: other) {
            insert(pair);
        }
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(B_tree&& other) noexcept : _allocator(std::move(other._allocator)),
                                                                    _logger(other._logger),
                                                                    _root(other._root),
                                                                    _size(other._size) {
    compare::operator=(std::move(other));
    other._root = nullptr;
    other._size = 0;
    other._logger = nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>& B_tree<tkey, tvalue, compare, t>::operator=(B_tree&& other) noexcept {
    if (this != &other) {
        clear();
        _allocator = std::move(other._allocator);
        _logger = other._logger;
        _root = other._root;
        _size = other._size;
        compare::operator=(std::move(other));
        other._root = nullptr;
        other._size = 0;
        other._logger = nullptr;
    }
    return *this;
}



template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_iterator::btree_iterator(
        const std::stack<std::pair<btree_node**, size_t>>& path, size_t index) : _path(path), _index(index) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator*() const noexcept {
    return (*(_path.top().first))->_keys[_path.top().second];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator->() const noexcept {
    return &((*(_path.top().first))->_keys[_path.top().second]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator&
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++() {
    if (_path.empty()) return *this;

    auto& [node_ptr, pointer_idx] = _path.top();
    btree_node* node = *node_ptr;

    if (!node->_pointers.empty()) {
        _path.top().second = _path.top().second + 1;
        _path.push({ &node->_pointers[_index + 1], 0 });
        node = *(_path.top().first);
        while (!node->_pointers.empty()) {
            _path.push({ &node->_pointers[0], 0 });
            node = *(_path.top().first);
        }
        _index = 0;
        return *this;
    }

    if (_index + 1 < node->_keys.size()) {
        ++_index;
        _path.top().second = _index;
        return *this;
    }

    _path.pop();
    while (!_path.empty()) {
        auto& [parent_ptr, child_idx] = _path.top();
        btree_node* parent = *parent_ptr;
        if (child_idx < parent->_keys.size()) {
            _index = child_idx;

            return *this;
        }
        _path.pop();
    }

    _index = 0;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++(int) {
    btree_iterator temp = *this;
    ++(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator&
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--() {
    if (_path.empty()) {
        return *this;
    }

    auto* node = *_path.top().first;
    if (_index > 0) {
        --_index;
        _path.top().second = _index;
        return *this;
    }

    if (node->_pointers.empty()) {
        while (!_path.empty()) {
            auto [ptr, idx_in_parent] = _path.top();
            _path.pop();
            if (_path.empty()) {
                _path = {};
                _index = 0;
                return *this;
            }
            auto* parent = *_path.top().first;
            size_t pos = ptr - parent->_pointers.data();
            if (pos > 0) {
                _index = pos - 1;
                _path.top().second = _index;
                return *this;
            }
        }
    } else {
        _path.push({&node->_pointers[_index], 0});
        node = *_path.top().first;
        while (!node->_pointers.empty()) {
            _path.push({&node->_pointers[node->_pointers.size() - 1], 0});
            node = *_path.top().first;
        }
        _index = node->_keys.size() - 1;
        _path.top().second = _index;
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--(int) {
    btree_iterator temp = *this;
    --(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::operator==(const self& other) const noexcept {
    if (_path.empty() && other._path.empty()) {
        return true;
    }
    if (_path.empty() || other._path.empty()) {
        return false;
    }
    return _path.top().first == other._path.top().first && _index == other._index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::operator!=(const self& other) const noexcept {
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::depth() const noexcept {
    return _path.size() - 1;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::current_node_keys_count() const noexcept {
    if (_path.empty()) {
        return 0;
    }
    return (*_path.top().first)->_keys.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::is_terminate_node() const noexcept {
    if (_path.empty()) {
        return true;
    }
    return (*_path.top().first)->_pointers.empty();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::index() const noexcept {
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(
        const std::stack<std::pair<btree_node* const*, size_t>>& path, size_t index) : _path(path), _index(index) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(
        const btree_iterator& it) noexcept : _path(it._path), _index(it._index) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator*() const noexcept {
    return (*(_path.top().first))->_keys[_path.top().second];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator->() const noexcept {
    return &((*(_path.top().first))->_keys[_path.top().second]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++() {
    if (_path.empty()) return *this;

    auto& [node_ptr, pointer_idx] = _path.top();
    btree_node* node = *node_ptr;

    if (!node->_pointers.empty()) {
        _path.top().second = _path.top().second + 1;
        _path.push({ &node->_pointers[_index + 1], 0 });
        node = *(_path.top().first);
        while (!node->_pointers.empty()) {
            _path.push({ &node->_pointers[0], 0 });
            node = *(_path.top().first);
        }
        _index = 0;
        return *this;
    }

    if (_index + 1 < node->_keys.size()) {
        ++_index;
        _path.top().second = _index;
        return *this;
    }

    _path.pop();
    while (!_path.empty()) {
        auto& [parent_ptr, child_idx] = _path.top();
        btree_node* parent = *parent_ptr;
        if (child_idx < parent->_keys.size()) {
            _index = child_idx;

            return *this;
        }
        _path.pop();
    }

    _index = 0;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--() {
    if (_path.empty()) {
        return *this;
    }

    auto* node = *_path.top().first;
    if (_index > 0) {
        --_index;
        _path.top().second = _index;
        return *this;
    }

    if (node->_pointers.empty()) {
        while (!_path.empty()) {
            auto [ptr, idx_in_parent] = _path.top();
            _path.pop();
            if (_path.empty()) {
                _path = {};
                _index = 0;
                return *this;
            }
            auto* parent = *_path.top().first;
            size_t pos = ptr - parent->_pointers.data();
            if (pos > 0) {
                _index = pos - 1;
                _path.top().second = _index;
                return *this;
            }
        }
    } else {
        _path.push({&node->_pointers[_index], 0});
        node = *_path.top().first;
        while (!node->_pointers.empty()) {
            _path.push({&node->_pointers[node->_pointers.size() - 1], 0});
            node = *_path.top().first;
        }
        _index = node->_keys.size() - 1;
        _path.top().second = _index;
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator==(const self& other) const noexcept {
    if (_path.empty() && other._path.empty()) return true;
    if (_path.empty() || other._path.empty()) return false;

    return _path.top().first == other._path.top().first && _index == other._index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator!=(const self& other) const noexcept {
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::depth() const noexcept {
    return _path.size() - 1;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::current_node_keys_count() const noexcept {
    if (_path.empty()) return 0;
    return (*_path.top().first)->_keys.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::is_terminate_node() const noexcept {
    if (_path.empty()) return true;
    return (*_path.top().first)->_pointers.empty();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::index() const noexcept {
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(
        const std::stack<std::pair<btree_node**, size_t>>& path, size_t index) : _path(path), _index(index) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(
        const btree_iterator& it) noexcept : _path(it._path), _index(it._index) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator B_tree<tkey, tvalue, compare, t>::btree_iterator() const noexcept {
    return btree_iterator(_path, _index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator*() const noexcept {
    return (*_path.top().first)->_keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator->() const noexcept {
    return &((*_path.top().first)->_keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++() {
    auto fwd = static_cast<btree_iterator>(*this);
    --fwd;
    *this = btree_reverse_iterator(fwd);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++(int) {
    btree_const_iterator temp = *this;
    --(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--() {
    auto fwd = static_cast<btree_iterator>(*this);
    ++fwd;
    *this = btree_reverse_iterator(fwd);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--(int) {
    btree_const_iterator temp = *this;
    ++(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator==(const self& other) const noexcept {
    if (_path.empty() && other._path.empty()) return true;
    if (_path.empty() || other._path.empty()) return false;

    return _path.top().first == other._path.top().first && _index == other._index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator!=(const self& other) const noexcept {
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::depth() const noexcept {
    return _path.size() - 1;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::current_node_keys_count() const noexcept {
    if (_path.empty()) return 0;
    return (*_path.top().first)->_keys.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::is_terminate_node() const noexcept {
    if (_path.empty()) return true;
    return (*_path.top().first)->_pointers.empty();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::index() const noexcept {
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(
        const std::stack<std::pair<btree_node* const*, size_t>>& path, size_t index) : _path(path), _index(index) {}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(
        const btree_reverse_iterator& it) noexcept : _path(it._path), _index(it._index) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator B_tree<tkey, tvalue, compare, t>::btree_const_iterator() const noexcept {
    return btree_const_iterator(_path, _index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator*() const noexcept {
    return (*_path.top().first)->_keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator->() const noexcept {
    return &((*_path.top().first)->_keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++() {
    auto fwd = static_cast<btree_iterator>(*this);
    --fwd;
    *this = btree_reverse_iterator(fwd);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++(int) {
    btree_const_iterator temp = *this;
    --(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--() {
    auto fwd = static_cast<btree_iterator>(*this);
    ++fwd;
    *this = btree_reverse_iterator(fwd);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--(int) {
    btree_const_iterator temp = *this;
    ++(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator==(const self& other) const noexcept {
    if (_path.empty() && other._path.empty()) return true;
    if (_path.empty() || other._path.empty()) return false;

    return _path.top().first == other._path.top().first && _index == other._index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator!=(const self& other) const noexcept {
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::depth() const noexcept {
    return _path.size() - 1;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::current_node_keys_count() const noexcept {
    if (_path.empty()) return 0;
    return (*_path.top().first)->_keys.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::is_terminate_node() const noexcept {
    if (_path.empty()) return true;
    return (*_path.top().first)->_pointers.empty();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::index() const noexcept {
    return _index;
}



template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::at(const tkey& key) {
    auto it = find(key);
    if (it == end()) {
        throw std::out_of_range("Key not found in B-tree");
    }
    return it->second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
const tvalue& B_tree<tkey, tvalue, compare, t>::at(const tkey& key) const {
    auto it = find(key);
    if (it == end()) {
        throw std::out_of_range("Key not found in B-tree");
    }
    return it->second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::operator[](const tkey& key) {
    auto it = find(key);
    if (it == end()) {
        it = insert(std::make_pair(key, tvalue())).first;
    }
    return it->second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::operator[](tkey&& key) {
    auto it = find(key);
    if (it == end()) {
        it = insert(std::make_pair(key, tvalue())).first;
    }
    return it->second;
}




template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::begin() {
    if (!_root) {
        return end();
    }
    std::stack<std::pair<btree_node**, size_t>> path;
    btree_node* current = _root;
    while (!current->_pointers.empty()) {
        path.push(std::make_pair(&current, 0));
        current = current->_pointers[0];
    }
    path.push(std::make_pair(&current, 0));
    return btree_iterator(path, 0);
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::end() {
    return btree_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator
B_tree<tkey, tvalue, compare, t>::begin() const {
    if (!_root) {
        return end();
    }
    std::stack<std::pair<btree_node* const*, size_t>> path;
    btree_node* const* node_ptr = &_root;
    while (!(*node_ptr)->_pointers.empty()) {
        path.push(std::make_pair(node_ptr, 0));
        node_ptr = &((*node_ptr)->_pointers[0]);
    }
    path.push(std::make_pair(node_ptr, 0));
    return btree_const_iterator(path, 0);
}



template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::end() const {
    return btree_const_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cbegin() const {
    return begin();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cend() const {
    return end();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin() {
    if (!_root) {
        return rend();
    }
    std::stack<std::pair<btree_node**, size_t>> path;
    btree_node* current = _root;
    while (!current->_pointers.empty()) {
        path.push({&current, current->_keys.size()});
        current = current->_pointers[current->_pointers.size() - 1];
    }
    path.push({&current, current->_keys.size() - 1});
    return btree_reverse_iterator(path, current->_keys.size() - 1);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend() {
    return btree_reverse_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin() const {
    if (!_root) {
        return rend();
    }
    std::stack<std::pair<btree_node* const*, size_t>> path;
    const btree_node* current = _root;
    while (!current->_pointers.empty()) {
        path.push({&current, current->_keys.size()});
        current = current->_pointers[current->_pointers.size() - 1];
    }
    path.push({&current, current->_keys.size() - 1});
    return btree_const_reverse_iterator(path, current->_keys.size() - 1);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend() const {
    return btree_const_reverse_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crbegin() const {
    return rbegin();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crend() const {
    return rend();
}



template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::size() const noexcept {
    return _size;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::empty() const noexcept {
    return _size == 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::find(const tkey& key) {
    if (!_root)
        return end();

    std::stack<std::pair<btree_node**, size_t>> path;
    btree_node** ptr = &_root;
    btree_node* node = *ptr;

    while (true) {
        size_t lo = 0, hi = node->_keys.size();
        while (lo < hi) {
            size_t mid = (lo + hi) / 2;
            if (compare_keys(node->_keys[mid].first, key))
                lo = mid + 1;
            else
                hi = mid;
        }
        if (lo < node->_keys.size()
            && !compare_keys(key, node->_keys[lo].first)
            && !compare_keys(node->_keys[lo].first, key)) {
            path.emplace(ptr, lo);
            return btree_iterator(path, lo);
        }
        if (node->_pointers.empty())
            break;

        path.emplace(ptr, lo);
        ptr = &node->_pointers[lo];
        node = *ptr;
    }
    return end();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::find(const tkey& key) const {
    if (!_root) return cend();
    std::stack<std::pair<btree_node* const*, size_t>> path;
    btree_node* const* node_ptr = &_root;
    while (true) {
        btree_node* node = *node_ptr;
        size_t lo = 0, hi = node->_keys.size();
        while (lo < hi) {
            size_t mid = (lo + hi) / 2;
            if (compare_keys(node->_keys[mid].first, key))
                lo = mid + 1;
            else
                hi = mid;
        }
        if (lo < node->_keys.size() &&
            !compare_keys(key, node->_keys[lo].first) &&
            !compare_keys(node->_keys[lo].first, key)) {
            path.emplace(node_ptr, lo);
            return btree_const_iterator(path, lo);
        }
        if (node->_pointers.empty()) break;
        path.emplace(node_ptr, lo);
        node_ptr = &node->_pointers[lo];
    }
    return cend();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key) {
    if (!_root) return end();
    std::stack<std::pair<btree_node**, size_t>> path;
    btree_node* node = _root;
    while (true) {
        size_t lo = 0, hi = node->_keys.size();
        while (lo < hi) {
            size_t mid = (lo + hi) / 2;
            if (compare_keys(node->_keys[mid].first, key))
                lo = mid + 1;
            else
                hi = mid;
        }
        path.emplace(&node, lo);
        if (node->_pointers.empty()) {
            if (lo < node->_keys.size())
                return btree_iterator(path, lo);
            else
                return end();
        }
        node = node->_pointers[lo];
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key) const {
    if (!_root) return cend();
    std::stack<std::pair<btree_node* const*, size_t>> path;
    btree_node* const* node_ptr = &_root;
    while (true) {
        btree_node* node = *node_ptr;
        size_t lo = 0, hi = node->_keys.size();
        while (lo < hi) {
            size_t mid = (lo + hi) / 2;
            if (compare_keys(node->_keys[mid].first, key))
                lo = mid + 1;
            else
                hi = mid;
        }
        path.emplace(node_ptr, lo);
        if (node->_pointers.empty()) {
            if (lo < node->_keys.size())
                return btree_const_iterator(path, lo);
            else
                return cend();
        }
        node_ptr = &node->_pointers[lo];
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key) {
    if (!_root) return end();
    std::stack<std::pair<btree_node**, size_t>> path;
    btree_node* node = _root;
    while (true) {
        size_t lo = 0, hi = node->_keys.size();
        while (lo < hi) {
            size_t mid = (lo + hi) / 2;
            if (!compare_keys(key, node->_keys[mid].first))
                lo = mid + 1;
            else
                hi = mid;
        }
        path.emplace(&node, lo);
        if (node->_pointers.empty()) {
            if (lo < node->_keys.size())
                return btree_iterator(path, lo);
            else
                return end();
        }
        node = node->_pointers[lo];
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key) const {
    if (!_root) return cend();
    std::stack<std::pair<btree_node* const*, size_t>> path;
    btree_node* const* node_ptr = &_root;
    while (true) {
        btree_node* node = *node_ptr;
        size_t lo = 0, hi = node->_keys.size();
        while (lo < hi) {
            if (!compare_keys(key, node->_keys[(lo + hi) / 2].first))
                lo = (lo + hi) / 2 + 1;
            else
                hi = (lo + hi) / 2;
        }
        path.emplace(node_ptr, lo);
        if (node->_pointers.empty()) {
            if (lo < node->_keys.size())
                return btree_const_iterator(path, lo);
            else
                return cend();
        }
        node_ptr = &node->_pointers[lo];
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::contains(const tkey& key) const {
    return find(key) != end();
}



template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::clear() noexcept {
    if (_root) {
        std::stack<btree_node*> nodes;
        nodes.push(_root);
        while (!nodes.empty()) {
            auto* current = nodes.top();
            nodes.pop();
            for (auto* child: current->_pointers) {
                if (child) {
                    nodes.push(child);
                }
            }
            delete current;
        }
        _root = nullptr;
        _size = 0;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::insert(const tree_data_type& data) {
    if (!_root) {
        _root = new btree_node();
        _root->_keys.push_back(data);
        _size = 1;
        return {btree_iterator(), true};
    }

    std::stack<std::pair<btree_node*, size_t>> path_stack;
    btree_node* cur = _root;

    while (true) {
        size_t idx = 0;
        while (idx < cur->_keys.size() && compare_keys(cur->_keys[idx].first, data.first)) {
            ++idx;
        }
        path_stack.push({cur, idx});
        if (cur->_pointers.empty()) break;
        cur = cur->_pointers[idx];
    }

    auto& keys = cur->_keys;
    auto it = std::lower_bound(
            keys.begin(), keys.end(), data,
            [this](auto const& a, auto const& b) { return compare_pairs(a, b); }
    );
    if (it != keys.end() &&
        !compare_keys(data.first, it->first) &&
        !compare_keys(it->first, data.first)) {
        return {btree_iterator(), false};
    }
    keys.insert(it, data);
    ++_size;

    while (!path_stack.empty()) {
        auto [node, child_idx] = path_stack.top();
        path_stack.pop();

        if (node->_keys.size() <= maximum_keys_in_node) break;

        size_t mid = t;
        auto mid_key = node->_keys[mid];
        btree_node* right = new btree_node();
        right->_keys.assign(
                node->_keys.begin() + mid + 1,
                node->_keys.end()
        );
        node->_keys.resize(mid);

        if (!node->_pointers.empty()) {
            right->_pointers.assign(
                    node->_pointers.begin() + mid + 1,
                    node->_pointers.end()
            );
            node->_pointers.resize(mid + 1);
        }

        if (path_stack.empty()) {
            btree_node* new_root = new btree_node();
            new_root->_keys.push_back(mid_key);
            new_root->_pointers.push_back(node);
            new_root->_pointers.push_back(right);
            _root = new_root;
        } else {
            auto [parent, ignored] = path_stack.top();

            auto insert_it = std::lower_bound(
                    parent->_keys.begin(),
                    parent->_keys.end(),
                    mid_key,
                    [this](auto const& a, auto const& b){ return compare_pairs(a, b); }
            );
            size_t pos = std::distance(parent->_keys.begin(), insert_it);

            parent->_keys.insert(parent->_keys.begin() + pos, mid_key);
            parent->_pointers.insert(parent->_pointers.begin() + pos + 1, right);
        }
    }

    return {btree_iterator(), true};
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::insert(tree_data_type&& data) {
    return insert(data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<typename... Args>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::emplace(Args&&... args) {
    return insert(tree_data_type(std::forward<Args>(args)...));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::insert_or_assign(const tree_data_type& data) {
    auto it = find(data.first);
    if (it != end()) {
        it->second = data.second;
        return it;
    }
    return insert(data).first;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::insert_or_assign(tree_data_type&& data) {
    auto it = find(data.first);
    if (it != end()) {
        it->second = std::move(data.second);
        return it;
    }
    return insert(std::move(data)).first;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<typename... Args>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::emplace_or_assign(Args&&... args) {
    return insert_or_assign(tree_data_type(std::forward<Args>(args)...));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_iterator pos) {
    if (pos == end()) return pos;
    tkey key = pos->first;
    ++pos;
    erase(key);
    return pos;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_const_iterator pos) {
    return erase(btree_iterator(pos));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_iterator first, btree_iterator last) {
    while (first != last) {
        first = erase(first);
    }
    return last;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_const_iterator beg, btree_const_iterator en) {
    return erase(btree_iterator(beg), btree_iterator(en));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(const tkey& key) {
    if (!_root) return end();


    auto merge_nodes = [](btree_node* parent, size_t child_idx) -> btree_node* {
        btree_node* left_child = parent->_pointers[child_idx];
        btree_node* right_child = parent->_pointers[child_idx + 1];


        left_child->_keys.push_back(parent->_keys[child_idx]);


        left_child->_keys.insert(
                left_child->_keys.end(),
                right_child->_keys.begin(),
                right_child->_keys.end()
        );


        if (!right_child->_pointers.empty()) {
            left_child->_pointers.insert(
                    left_child->_pointers.end(),
                    right_child->_pointers.begin(),
                    right_child->_pointers.end()
            );
        }


        parent->_keys.erase(parent->_keys.begin() + child_idx);

        parent->_pointers.erase(parent->_pointers.begin() + child_idx + 1);


        btree_node* node_to_delete = right_child;

        return node_to_delete;
    };


    auto borrow_from_left = [](btree_node* parent, size_t child_idx) {
        btree_node* child = parent->_pointers[child_idx];
        btree_node* left_sibling = parent->_pointers[child_idx - 1];


        child->_keys.insert(child->_keys.begin(), parent->_keys[child_idx - 1]);


        parent->_keys[child_idx - 1] = left_sibling->_keys.back();
        left_sibling->_keys.pop_back();


        if (!left_sibling->_pointers.empty()) {
            child->_pointers.insert(child->_pointers.begin(), left_sibling->_pointers.back());
            left_sibling->_pointers.pop_back();
        }
    };


    auto borrow_from_right = [](btree_node* parent, size_t child_idx) {
        btree_node* child = parent->_pointers[child_idx];
        btree_node* right_sibling = parent->_pointers[child_idx + 1];


        child->_keys.push_back(parent->_keys[child_idx]);


        parent->_keys[child_idx] = right_sibling->_keys.front();
        right_sibling->_keys.erase(right_sibling->_keys.begin());


        if (!right_sibling->_pointers.empty()) {
            child->_pointers.push_back(right_sibling->_pointers.front());
            right_sibling->_pointers.erase(right_sibling->_pointers.begin());
        }
    };


    auto fix_underflow = [&](btree_node* parent, size_t child_idx) -> bool {
        btree_node* child = parent->_pointers[child_idx];


        if (child_idx > 0) {
            btree_node* left_sibling = parent->_pointers[child_idx - 1];
            if (left_sibling->_keys.size() > minimum_keys_in_node) {
                borrow_from_left(parent, child_idx);
                return false;
            }
        }


        if (child_idx < parent->_pointers.size() - 1) {
            btree_node* right_sibling = parent->_pointers[child_idx + 1];
            if (right_sibling->_keys.size() > minimum_keys_in_node) {
                borrow_from_right(parent, child_idx);
                return false;
            }
        }


        btree_node* node_to_delete = nullptr;

        if (child_idx > 0) {

            node_to_delete = merge_nodes(parent, child_idx - 1);
        } else {

            node_to_delete = merge_nodes(parent, child_idx);
        }

        delete node_to_delete;


        return parent->_keys.size() < minimum_keys_in_node;
    };


    std::function<bool(btree_node*, const tkey&)> remove_key;
    remove_key = [this, &remove_key, &fix_underflow](btree_node* node, const tkey& k) -> bool {

        size_t idx = 0;
        while (idx < node->_keys.size() && compare_keys(node->_keys[idx].first, k)) ++idx;


        bool key_found = idx < node->_keys.size() &&
                         !compare_keys(k, node->_keys[idx].first) &&
                         !compare_keys(node->_keys[idx].first, k);

        if (key_found) {

            if (node->_pointers.empty()) {
                node->_keys.erase(node->_keys.begin() + idx);
                --_size;
                return node->_keys.size() < minimum_keys_in_node;
            }
            else {

                btree_node* predecessor = node->_pointers[idx];
                while (!predecessor->_pointers.empty())
                    predecessor = predecessor->_pointers.back();


                auto pred_pair = predecessor->_keys.back();


                node->_keys[idx] = pred_pair;


                bool underflow = remove_key(node->_pointers[idx], pred_pair.first);


                if (underflow)
                    return fix_underflow(node, idx);

                return false;
            }
        }

        if (node->_pointers.empty())
            return false;
        bool underflow = remove_key(node->_pointers[idx], k);

        if (underflow)
            return fix_underflow(node, idx);

        return false;
    };
    bool root_underflow = remove_key(_root, key);

    if (root_underflow && _root->_keys.empty() && !_root->_pointers.empty()) {
        btree_node* old_root = _root;
        _root = _root->_pointers[0];
        delete old_root;
    }

    if (_root->_keys.empty() && _root->_pointers.empty()) {
        delete _root;
        _root = nullptr;
    }

    return find(key);
}



template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool compare_pairs(const typename B_tree<tkey, tvalue, compare, t>::tree_data_type& lhs,
                   const typename B_tree<tkey, tvalue, compare, t>::tree_data_type& rhs) {
    return compare()(lhs.first, rhs.first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool compare_keys(const tkey& lhs, const tkey& rhs) {
    return compare()(lhs, rhs);
}

#endif
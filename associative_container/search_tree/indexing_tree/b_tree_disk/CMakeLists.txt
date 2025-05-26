add_subdirectory(tests)

add_library(
        mp_os_assctv_cntnr_srch_tr_indxng_tr_b_tr_dsk
        include/b_tree_disk.hpp
        src/hhh.cpp)

target_include_directories(
        mp_os_assctv_cntnr_srch_tr_indxng_tr_b_tr_dsk
        PUBLIC
        ./include)

target_link_libraries(
        mp_os_assctv_cntnr_srch_tr_indxng_tr_b_tr_dsk
        PUBLIC
        mp_os_lggr_clnt_lggr)
target_link_libraries(
        mp_os_assctv_cntnr_srch_tr_indxng_tr_b_tr_dsk
        PUBLIC
        mp_os_assctv_cntnr_srch_tr)
target_link_libraries(
        mp_os_assctv_cntnr_srch_tr_indxng_tr_b_tr_dsk
        PUBLIC
        ${BOOST_LIBRARIES})

target_include_directories(
        mp_os_assctv_cntnr_srch_tr_indxng_tr_b_tr
        PUBLIC
        ${Boost_INCLUDE_DIRS})
target_link_directories(
        mp_os_assctv_cntnr_srch_tr_indxng_tr_b_tr
        PUBLIC
        ${Boost_LIBRARY_DIR}
)
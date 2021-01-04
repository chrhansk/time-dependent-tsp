add_library(test_common EXCLUDE_FROM_ALL
  tour/program_test.cc
  tour/acyclic_relaxation_test.cc
  tour/relaxation_test.cc
  tour/timed/separators/separator_test.cc
  tour/path/pricers/path_based_pricer_test.cc
  tour/sparse/pricers/sparse_pricer_test.cc
  tour/sparse/separators/sparse_separator_test.cc
  basic_test.cc)

target_link_libraries(test_common
  common
  ${GTEST_LIBRARIES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(build_tests)

add_custom_target(run_tests
  COMMAND ${CMAKE_CTEST_COMMAND}
  DEPENDS build_tests)

function(add_unit_test TEST_NAME)
  get_filename_component(BASE_NAME ${TEST_NAME} NAME)
  add_executable(${BASE_NAME}
    EXCLUDE_FROM_ALL
    ${TEST_NAME}.cc)

  target_link_libraries(${BASE_NAME}
    ${GTEST_MAIN_LIBRARIES}
    test_common)

  add_dependencies(build_tests ${BASE_NAME})

  add_test(NAME ${BASE_NAME} COMMAND ${BASE_NAME})
endfunction()

add_unit_test(arborescence/min_arborescence_test)

add_unit_test(timed/augmented_edge_func_test)
add_unit_test(timed/time_expanded_graph_test)
add_unit_test(router/distance_tree_test)
add_unit_test(router/router_test)

add_unit_test(tour/sparse/sparse_program_test)
add_unit_test(tour/sparse/sparse_propagator_test)

add_unit_test(tour/sparse/pricers/sparse_acyclic_pricer_test)
add_unit_test(tour/sparse/pricers/sparse_acyclic_hole_free_pricer_test)
add_unit_test(tour/sparse/pricers/sparse_edge_pricer_test)
add_unit_test(tour/sparse/pricers/sparse_path_pricer_test)
add_unit_test(tour/sparse/pricers/sparse_two_cycle_free_pricer_test)
add_unit_test(tour/sparse/pricers/sparse_stabilizing_pricer_test)

add_unit_test(tour/sparse/separators/sparse_cycle_separator_test)
add_unit_test(tour/sparse/separators/sparse_dk_separator_test)
add_unit_test(tour/sparse/separators/sparse_odd_cat_separator_test)
add_unit_test(tour/sparse/separators/sparse_lifted_subtour_separator_test)
add_unit_test(tour/sparse/separators/sparse_subtour_separator_test)
add_unit_test(tour/sparse/separators/sparse_odd_path_free_separator_test)
add_unit_test(tour/sparse/separators/sparse_unitary_afc_separator_test)

add_unit_test(tour/path/path_based_program_test)
add_unit_test(tour/path/pricers/path_based_acyclic_pricer_test)
add_unit_test(tour/path/pricers/path_based_acyclic_hole_free_pricer_test)
add_unit_test(tour/path/pricers/path_based_two_cycle_free_pricer_test)

add_unit_test(tour/path/relaxations/path_based_acyclic_relaxation_test)
add_unit_test(tour/path/relaxations/path_based_acyclic_hole_free_relaxation_test)
add_unit_test(tour/path/relaxations/path_based_two_cycle_free_relaxation_test)

add_unit_test(tour/path/relaxations/path_based_simple_relaxation_test)
add_unit_test(tour/path/relaxations/path_based_acyclic_stabilizing_relaxation_test)
add_unit_test(tour/path/relaxations/path_based_acyclic_hole_free_stabilizing_relaxation_test)
add_unit_test(tour/path/relaxations/path_based_two_cycle_free_stabilizing_relaxation_test)

add_unit_test(tour/path/pricers/path_based_acyclic_stabilizing_pricer_test)
add_unit_test(tour/path/pricers/path_based_acyclic_hole_free_stabilizing_pricer_test)
add_unit_test(tour/path/pricers/path_based_simple_stabilizing_pricer_test)
add_unit_test(tour/path/pricers/path_based_two_cycle_free_stabilizing_pricer_test)


add_unit_test(tour/timed/simple_program_test)
add_unit_test(tour/timed/separators/cycle_separator_test)
add_unit_test(tour/timed/separators/lifted_subtour_separator_test)
add_unit_test(tour/timed/separators/simple_dk_separator_test)
add_unit_test(tour/timed/separators/simple_odd_cat_separator_test)
add_unit_test(tour/timed/separators/simple_odd_path_free_separator_test)
add_unit_test(tour/timed/separators/simple_unitary_afc_separator_test)
add_unit_test(tour/timed/separators/subtour_separator_test)
add_unit_test(tour/timed/separators/two_cycle_separator_test)

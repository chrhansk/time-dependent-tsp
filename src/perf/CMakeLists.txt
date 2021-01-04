 add_library(perf_common EXCLUDE_FROM_ALL
   tour/program_benchmark.cc
   tour/path/pricers/path_based_pricer_benchmark.cc
   tour/sparse/heuristics/sparse_heuristic_benchmark.cc
   tour/sparse/pricers/sparse_pricer_benchmark.cc
   tour/sparse/full/lifted_subtour/full_lifted_subtour_benchmark.cc
   tour/sparse/full/subtour/full_subtour_benchmark.cc
   tour/sparse/separators/sparse_separator_benchmark.cc
   tour/sparse/separators/combined/combined_separator_benchmark.cc)

include_directories(${CMAKE_SOURCE_DIR}/src/test)
include_directories(${CMAKE_SOURCE_DIR}/src/perf)

target_link_libraries(perf_common
  common
  test_common)

add_custom_target(build_benchmarks)

add_custom_target(run_benchmarks DEPENDS build_benchmarks)

add_custom_target(collect)

add_dependencies(collect build_benchmarks)

# include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(DRIVERS "" CACHE INTERNAL "")

function(add_benchmark BENCHMARK_NAME)
  get_filename_component(BASE_NAME ${BENCHMARK_NAME} NAME)
  add_executable(${BASE_NAME}
    EXCLUDE_FROM_ALL
    ${BENCHMARK_NAME}.cc)

  add_dependencies(build_benchmarks ${BASE_NAME})

  add_custom_target(run_${BASE_NAME}
    COMMAND ${BASE_NAME})

  add_dependencies(run_benchmarks run_${BASE_NAME})

  target_link_libraries(${BASE_NAME}
    perf_common)

  list(APPEND DRIVERS ${BASE_NAME})

  set(DRIVERS "${DRIVERS}" CACHE INTERNAL "")

endfunction()

function(add_collect_benchmark BENCHMARK_NAME)
  cmake_parse_arguments(PARSE_ARGV 0 ARGS "" "DRIVER;SIZE" "")

  #set(INSTANCE_DIR "${DATASET_DIR}/${ARGS_INSTANCE}")

  #add_benchmark(${BENCHMARK_NAME})
  set(SIZE "${ARGS_SIZE}")
  set(DRIVER "${ARGS_DRIVER}")

  set(TARGET_NAME "collect_${DRIVER}_${SIZE}")

  #get_filename_component(BASE_NAME ${BENCHMARK_NAME} NAME)

  add_custom_target(${TARGET_NAME}
    COMMAND ${DRIVER} --size=${SIZE} > "${CMAKE_CURRENT_BINARY_DIR}/${DRIVER}_${SIZE}.csv")

  add_dependencies(${TARGET_NAME} ${DRIVER})

  add_dependencies(collect ${TARGET_NAME})

endfunction()

add_benchmark(tour/path/path_based_program_benchmark)
add_benchmark(tour/sparse/sparse_program_benchmark)
add_benchmark(tour/timed/simple_program_benchmark)

add_benchmark(tour/path/pricers/path_based_simple_pricer_benchmark)
add_benchmark(tour/path/pricers/path_based_two_cycle_free_pricer_benchmark)

add_benchmark(tour/path/pricers/path_based_acyclic_hole_free_pricer_3_benchmark)
add_benchmark(tour/path/pricers/path_based_acyclic_hole_free_pricer_4_benchmark)
add_benchmark(tour/path/pricers/path_based_acyclic_hole_free_pricer_5_benchmark)

add_benchmark(tour/path/pricers/path_based_simple_stabilizing_pricer_benchmark)
add_benchmark(tour/path/pricers/path_based_two_cycle_free_stabilizing_pricer_benchmark)

add_benchmark(tour/path/pricers/path_based_acyclic_hole_free_stabilizing_pricer_3_benchmark)
add_benchmark(tour/path/pricers/path_based_acyclic_hole_free_stabilizing_pricer_4_benchmark)
add_benchmark(tour/path/pricers/path_based_acyclic_hole_free_stabilizing_pricer_5_benchmark)

add_benchmark(tour/path/relaxations/path_based_simple_relaxation_benchmark)
add_benchmark(tour/path/relaxations/path_based_two_cycle_free_relaxation_benchmark)

add_benchmark(tour/path/relaxations/path_based_acyclic_hole_free_relaxation_3_benchmark)
add_benchmark(tour/path/relaxations/path_based_acyclic_hole_free_relaxation_4_benchmark)
add_benchmark(tour/path/relaxations/path_based_acyclic_hole_free_relaxation_5_benchmark)

add_benchmark(tour/path/relaxations/path_based_acyclic_hole_free_stabilizing_relaxation_3_benchmark)
add_benchmark(tour/path/relaxations/path_based_acyclic_hole_free_stabilizing_relaxation_4_benchmark)
add_benchmark(tour/path/relaxations/path_based_acyclic_hole_free_stabilizing_relaxation_5_benchmark)

add_benchmark(tour/path/relaxations/path_based_simple_stabilizing_relaxation_benchmark)
add_benchmark(tour/path/relaxations/path_based_two_cycle_free_stabilizing_relaxation_benchmark)

add_benchmark(tour/sparse/full_sparse_program_benchmark)

add_benchmark(tour/sparse/pricers/sparse_acyclic_pricer_3_benchmark)
add_benchmark(tour/sparse/pricers/sparse_acyclic_pricer_4_benchmark)
add_benchmark(tour/sparse/pricers/sparse_acyclic_pricer_5_benchmark)

add_benchmark(tour/sparse/pricers/sparse_acyclic_hole_free_pricer_3_benchmark)
add_benchmark(tour/sparse/pricers/sparse_acyclic_hole_free_pricer_4_benchmark)
add_benchmark(tour/sparse/pricers/sparse_acyclic_hole_free_pricer_5_benchmark)

add_benchmark(tour/sparse/heuristics/sparse_compound_greedy_construction_benchmark)
add_benchmark(tour/sparse/heuristics/sparse_value_greedy_construction_benchmark)
add_benchmark(tour/sparse/heuristics/sparse_time_greedy_construction_benchmark)

add_benchmark(tour/sparse/pricers/sparse_edge_pricer_benchmark)
add_benchmark(tour/sparse/pricers/sparse_path_pricer_benchmark)
add_benchmark(tour/sparse/pricers/sparse_two_cycle_free_pricer_benchmark)
add_benchmark(tour/sparse/pricers/sparse_stabilizing_pricer_benchmark)

add_benchmark(tour/sparse/separators/sparse_cycle_separator_benchmark)
add_benchmark(tour/sparse/separators/sparse_dk_separator_benchmark)
add_benchmark(tour/sparse/separators/sparse_lifted_subtour_separator_benchmark)
add_benchmark(tour/sparse/separators/sparse_odd_cat_separator_benchmark)
add_benchmark(tour/sparse/separators/sparse_odd_path_free_separator_benchmark)
add_benchmark(tour/sparse/separators/sparse_subtour_separator_benchmark)
add_benchmark(tour/sparse/separators/sparse_unitary_afc_separator_benchmark)

add_benchmark(tour/sparse/separators/combined/combined_cycle_separator_benchmark)
add_benchmark(tour/sparse/separators/combined/combined_dk_separator_benchmark)
add_benchmark(tour/sparse/separators/combined/combined_odd_cat_separator_benchmark)
add_benchmark(tour/sparse/separators/combined/combined_unitary_afc_separator_benchmark)

add_benchmark(tour/sparse/relaxations/sparse_simple_relaxation_benchmark)
add_benchmark(tour/sparse/relaxations/sparse_two_cycle_free_relaxation_benchmark)

add_benchmark(tour/sparse/relaxations/sparse_acyclic_hole_free_relaxation_3_benchmark)
add_benchmark(tour/sparse/relaxations/sparse_acyclic_hole_free_relaxation_4_benchmark)
add_benchmark(tour/sparse/relaxations/sparse_acyclic_hole_free_relaxation_5_benchmark)

add_benchmark(tour/sparse/relaxations/sparse_acyclic_hole_free_stabilizing_relaxation_3_benchmark)
add_benchmark(tour/sparse/relaxations/sparse_acyclic_hole_free_stabilizing_relaxation_4_benchmark)
add_benchmark(tour/sparse/relaxations/sparse_acyclic_hole_free_stabilizing_relaxation_5_benchmark)

add_benchmark(tour/sparse/full/lifted_subtour/full_lifted_subtour_dk_benchmark)
add_benchmark(tour/sparse/full/lifted_subtour/full_lifted_subtour_odd_cat_benchmark)
add_benchmark(tour/sparse/full/lifted_subtour/full_lifted_subtour_odd_path_free_benchmark)
add_benchmark(tour/sparse/full/lifted_subtour/full_lifted_subtour_unitary_afc_benchmark)

add_benchmark(tour/sparse/full/subtour/full_subtour_dk_benchmark)
add_benchmark(tour/sparse/full/subtour/full_subtour_odd_cat_benchmark)
add_benchmark(tour/sparse/full/subtour/full_subtour_odd_path_free_benchmark)
add_benchmark(tour/sparse/full/subtour/full_subtour_unitary_afc_benchmark)

set(SIZES "medium;large")

foreach(DRIVER ${DRIVERS})
  foreach(SIZE ${SIZES})
    add_collect_benchmark(DRIVER ${DRIVER} SIZE ${SIZE})
  endforeach()
endforeach()

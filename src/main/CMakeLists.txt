set(COMMON_SRC
  ansi_color.cc
  cmp.cc
  log.cc
  util.cc
  program.cc
  lp_observer.cc
  solution_stats.cc
  arborescence/min_arborescence.cc
  flow/max_flow.cc
  instance.cc
  graph/edge.cc
  graph/edge_set.cc
  graph/graph.cc
  graph/subgraph.cc
  graph/vertex.cc
  graph/vertex_set.cc
  path/path.cc
  router/odd_cycle.cc
  router/router.cc
  timed/augmented_edge_func.cc
  timed/cached_distances.cc
  timed/cached_tree_distances.cc
  timed/path_decomposition.cc
  timed/timed_reader.cc
  timed/timed_router.cc
  timed/timed_edge.cc
  timed/timed_path.cc
  timed/timed_vertex.cc
  timed/time_expanded_graph.cc
  timed/router/time_expanded_router.cc
  timed/router/timed_path_set.cc
  timed/router/simple_time_expanded_router.cc
  timed/router/two_cycle_free_time_expanded_router.cc
  tour/heuristics/kopt_solver.cc
  tour/heuristics/two_opt_solver.cc
  tour/heuristics/three_opt_solver.cc
  tour/path/path_based_program.cc
  tour/path/pricers/path_based_pricer.cc
  tour/separators/dk_separator.cc
  tour/separators/odd_cat_separator.cc
  tour/separators/odd_path_free_separator.cc
  tour/separators/unitary_afc_separator.cc
  tour/sparse/separators/sparse_cut.cc
  tour/sparse/separators/sparse_cycle_separator.cc
  tour/sparse/separators/sparse_dk_separator.cc
  tour/sparse/separators/sparse_lifted_subtour_separator.cc
  tour/sparse/separators/sparse_odd_cat_separator.cc
  tour/sparse/separators/sparse_separator.cc
  tour/sparse/separators/sparse_separation_manager.cc
  tour/sparse/separators/sparse_subtour_separator.cc
  tour/sparse/separators/sparse_odd_path_free_separator.cc
  tour/sparse/separators/sparse_unitary_afc_separator.cc
  tour/sparse/heuristics/greedy_construction.cc
  tour/sparse/heuristics/compound_greedy_construction.cc
  tour/sparse/heuristics/value_greedy_construction.cc
  tour/sparse/heuristics/time_greedy_construction.cc
  tour/sparse/pricers/sparse_edge_pricer.cc
  tour/sparse/pricers/sparse_path_pricer.cc
  tour/sparse/pricers/sparse_pricer.cc
  tour/sparse/pricers/sparse_pricing_manager.cc
  tour/sparse/pricers/sparse_stabilizing_pricer.cc
  tour/sparse/sparse_objective_propagator.cc
  tour/sparse/sparse_program.cc
  tour/solution_result.cc
  tour/tour.cc
  tour/static/tour_solver.cc
  tour/static/subtour_handler.cc
  tour/static/separators/static_dk_separator.cc
  tour/static/separators/static_odd_cat_separator.cc
  tour/timed/covering_relaxation.cc
  tour/timed/expand_tour.cc
  tour/timed/exact_timed_solver.cc
  tour/timed/simple_program.cc
  tour/timed/heuristics/timed_lkh_solver.cc
  tour/timed/heuristics/restricted_dynamic_solver.cc
  tour/timed/heuristics/shifting_solver.cc
  tour/timed/separators/cycle_separator.cc
  tour/timed/separators/simple_odd_path_free_separator.cc
  tour/timed/separators/subtour_separator.cc
  tour/timed/separators/two_cycle_separator.cc
  tour/timed/separators/lifted_subtour_separator.cc
  tour/timed/separators/simple_dk_separator.cc
  tour/timed/separators/simple_odd_cat_separator.cc
  tour/timed/separators/simple_unitary_afc_separator.cc)

configure_file(defs.hh.in ${CMAKE_BINARY_DIR}/defs.hh)

add_library(common ${COMMON_SRC})

if(THREADS_HAVE_PTHREAD_ARG)
  set_property(TARGET common PROPERTY COMPILE_OPTIONS "-pthread")
  set_property(TARGET common PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
endif()

target_link_libraries(common
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
  ${SCIP_LIBRARIES})

add_executable(static_solver static_solver.cc)
target_link_libraries(static_solver common)

add_executable(timed_solver timed_solver.cc)
target_link_libraries(timed_solver common)

add_executable(lp_exporter lp_exporter.cc)
target_link_libraries(lp_exporter common)

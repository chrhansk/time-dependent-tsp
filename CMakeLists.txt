cmake_minimum_required(VERSION 3.10)
project(tdtsp)

find_package(Boost 1.70
  CONFIG
  COMPONENTS log log_setup thread system program_options
  REQUIRED)

add_definitions(-DBOOST_LOG_DYN_LINK)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

enable_testing()

find_package(GTest REQUIRED)

find_package(SCIP REQUIRED)

if(${SCIP_VERSION} VERSION_LESS "7.0.0")
  message(WARNING "You are using an old version of SCIP (${SCIP_VERSION})")
endif()

if(CMAKE_BUILD_TYPE MATCHES "Release")
  message("Release mode, disabling assertions")
  add_definitions("-DNDEBUG")
endif()

include_directories(${CMAKE_BINARY_DIR}/)

add_subdirectory(src)

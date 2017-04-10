cmake_minimum_required (VERSION 2.8)
project(ActiveRestart)

include_directories("${PROJECT_SOURCE_DIR}/include")
SET(COVERAGE_COMPILE_FLAGS "-Wall -std=c++11")
SET(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -DCONSOLE_OUT")

if(CMAKE_BUILD_TYPE EQUAL DEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
elseif(CMAKE_BUILD_TYPE EQUAL RELEASE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
endif(CMAKE_BUILD_TYPE EQUAL DEBUG)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_COMPILE_FLAGS}" )

add_executable(
  ActiveRestart
  src/main.cpp
  src/processQuery.cpp
)

target_link_libraries(ActiveRestart Psapi)

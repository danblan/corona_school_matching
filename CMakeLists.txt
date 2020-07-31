cmake_minimum_required(VERSION 3.15)
project(corona_school_matching)
set(CMAKE_CXX_STANDARD 17)

set(BOOST_ROOT "C:/Users/danie/boost_1_73_0")
set(JSON_BuildTests OFF CACHE INTERNAL "")
include_directories("json-develop")

find_package(Boost 1.73.0 REQUIRED)
add_subdirectory(json-develop)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find lemon!")
endif()
#include_directories(${Boost_INCLUDE_DIR})
#link_directories(${Boost_LIBRARY_DIR})

add_executable(corona_school_matching main.cpp graph_creator.h costs.h matching.h types.h types.cpp graph_creator.cpp)
target_link_libraries(corona_school_matching PRIVATE nlohmann_json::nlohmann_json)
# link Boost libraries to the new target
#target_link_libraries(corona_school_matching ${Boost_LIBRARIES})

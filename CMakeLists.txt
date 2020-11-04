cmake_minimum_required(VERSION 3.15)
project(corona_school_matching)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_FIND_DEBUG_MODE 1)
set(BOOST_ROOT "C:/Users/danie/boost_1_73_0")
set(JSON_BuildTests OFF CACHE INTERNAL "")
include_directories("json-develop")

find_package(Boost 1.73.0 REQUIRED)
add_subdirectory(json-develop)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find boost!")
endif()

add_executable(corona_school_matching src/main.cpp src/graph_creator.h src/costs.h src/matching.h src/types.h src/graph_creator.cpp)
target_link_libraries(corona_school_matching PRIVATE nlohmann_json::nlohmann_json)

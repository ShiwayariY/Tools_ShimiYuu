cmake_minimum_required(VERSION 3.10)
project(Tools_ShimiYuu)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -std=c++17")

find_package(CURL REQUIRED)

include_directories(SYSTEM "${CURL_INCLUDE_DIRS}")
include_directories(include)

add_library(curl SHARED src/network/curl.cc)
add_library(helper SHARED src/helper.cc)

target_link_libraries(curl "${CURL_LIBRARIES}")

install(TARGETS
	curl
DESTINATION "${CMAKE_SOURCE_DIR}/lib/network")

install(TARGETS
	helper
DESTINATION "${CMAKE_SOURCE_DIR}/lib")

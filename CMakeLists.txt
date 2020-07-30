cmake_minimum_required(VERSION 3.10)
project(Tools_ShimiYuu)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -std=c++17")

find_package(CURL REQUIRED)
find_package(SQLite3 REQUIRED)

include_directories(SYSTEM
	"${CURL_INCLUDE_DIRS}"
	"${SQLite3_INCLUDE_DIRS}"
 )
include_directories(include)

add_library(curl SHARED src/network/curl.cc)
target_link_libraries(curl "${CURL_LIBRARIES}")

add_library(sqlite3db SHARED src/SQLite3DB.cc)
target_link_libraries(sqlite3db "${SQLite3_LIBRARIES}")

add_library(helper SHARED src/helper.cc)

install(TARGETS
	curl
DESTINATION "${CMAKE_SOURCE_DIR}/lib/network")

install(TARGETS
	helper sqlite3db
DESTINATION "${CMAKE_SOURCE_DIR}/lib")

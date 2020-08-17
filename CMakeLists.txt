cmake_minimum_required(VERSION 3.14)
project(Tools_Shimiyuu)

add_compile_options(-Wall -Wextra -pedantic)
set(CMAKE_CXX_STANDARD 17)

set(BUILD_SFX "${PLATFORM}/${CMAKE_BUILD_TYPE}")

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
DESTINATION "${CMAKE_SOURCE_DIR}/lib/${BUILD_SFX}/network")

install(TARGETS
	helper sqlite3db
DESTINATION "${CMAKE_SOURCE_DIR}/lib/${BUILD_SFX}")

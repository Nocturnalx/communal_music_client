cmake_minimum_required(VERSION 3.5.0)
project(MusicClient VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set( project_sources
   src/main.cpp
   src/Player.cpp
   src/Sample.cpp
   src/SampleTypes.cpp
   src/Synth.cpp
   src/Transport.cpp
   src/Network.cpp
)

add_executable(${PROJECT_NAME} ${project_sources})


find_package(Threads REQUIRED)

find_package(CURL REQUIRED)
if(CURL_FOUND)
 message(STATUS "Found CURL version: ${CURL_VERSION_STRING}")
 message(STATUS "Using CURL include dir(s): ${CURL_INCLUDE_DIRS}")
 message(STATUS "Using CURL lib(s): ${CURL_LIBRARIES}")
else()
 message(FATAL_ERROR "Could not find CURL")
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(CURLPP REQUIRED curlpp)
message(STATUS "curlpp include dirs: ${CURLPP_INCLUDE_DIRS}")
message(STATUS "curlpp libraries: ${CURLPP_LIBRARIES}")

find_package(nlohmann_json REQUIRED)


include_directories(${CURL_INCLUDE_DIRS} ${CURLPP_INCLUDE_DIRS} ${NLOHMANN_JSON_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} pulse Threads::Threads ${CURL_LIBRARIES} ${CURLPP_LIBRARIES} nlohmann_json::nlohmann_json)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

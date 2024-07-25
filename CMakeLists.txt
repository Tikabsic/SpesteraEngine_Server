cmake_minimum_required(VERSION 3.10)

project(SpesteraEngine_Server VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost 1.71 REQUIRED system)
find_package(Protobuf REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${Protobuf_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # Możesz usunąć, jeśli nie ma folderu include
)

file(GLOB SOURCES "*.cpp")

add_definitions(-DZLIB_CONST)  # Definicja wymagana przez zlib
add_library(zlib STATIC IMPORTED)
set_property(TARGET zlib PROPERTY IMPORTED_LOCATION ${ZLIB_LIBRARIES})

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES} ${Protobuf_LIBRARIES} zlib)

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
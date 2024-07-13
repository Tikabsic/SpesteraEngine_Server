cmake_minimum_required(VERSION 3.10)

# Ustaw nazwę projektu i wersję
project(SpesteraEngine_Server VERSION 1.0)

# Ustaw standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Znajdź pakiety Boost, Protobuf i Zlib
find_package(Boost 1.71 REQUIRED system)
find_package(Protobuf REQUIRED)
find_package(ZLIB REQUIRED)

# Dołącz katalogi z plikami nagłówkowymi
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${Protobuf_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # Możesz usunąć, jeśli nie ma folderu include
)

# Zbierz wszystkie pliki źródłowe .cpp z jednego katalogu
file(GLOB SOURCES "*.cpp")

# Dołącz bibliotekę zlib do kompilacji
add_definitions(-DZLIB_CONST)  # Definicja wymagana przez zlib
add_library(zlib STATIC IMPORTED)
set_property(TARGET zlib PROPERTY IMPORTED_LOCATION ${ZLIB_LIBRARIES})

# Dodaj target wykonywalny
add_executable(${PROJECT_NAME} ${SOURCES})

# Dołącz bibliotekę Boost, Protobuf, zlib
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES} ${Protobuf_LIBRARIES} zlib)

# Włącz ostrzeżenia dla kompilatora
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
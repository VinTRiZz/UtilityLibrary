# Build cmake setting
cmake_minimum_required(VERSION 2.8)
project(UtilityLibrary)

# include_directories(/home/lazarev_as/Projects/Qt/UtilityLibrary/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/BIN)

# Build variables
file(GLOB HEADERS src/*/*.h)
file(GLOB SOURCES src/*/*.cpp)

# Build output
add_library(UtilityLibrary SHARED
        ${SOURCES}
        ${HEADERS}
)

set_target_properties(UtilityLibrary PROPERTIES 
    VERSION 1.0
    SOVERSION 1
    PUBLIC_HEADER include/UtilityLibrary
)

# Encryption
target_link_libraries(UtilityLibrary -lcrypto)

# SQLite
target_link_libraries(UtilityLibrary -lsqlite3)

# Qt core
find_package(Qt5Core)
target_link_libraries(UtilityLibrary Qt5::Core)

# Qt network
find_package(Qt5Network)
target_link_libraries(UtilityLibrary Qt5::Network)

find_package(Qt5Widgets)
target_link_libraries(UtilityLibrary Qt5::Widgets)

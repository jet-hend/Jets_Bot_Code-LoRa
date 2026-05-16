cmake_minimum_required(VERSION 3.16)

project(Jets_Bot LANGUAGES CXX C)

# ==================== Basic Settings ====================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==================== Find Packages ====================
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

# ==================== Include Directories ====================
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}
)

# ==================== Source Files ====================
file(GLOB_RECURSE SOURCES 
    "${PROJECT_SOURCE_DIR}/main.cpp"
    "${PROJECT_SOURCE_DIR}/subsystems/*.cpp"
    "${PROJECT_SOURCE_DIR}/commands/**/*.cpp"
)

add_executable(Jets_Bot ${SOURCES})

# ==================== Linking ====================
target_link_libraries(Jets_Bot 
    PRIVATE
        Threads::Threads
        Boost::system
        wiringPi
        i2c
)

# ==================== Phoenix CTRE Libraries ====================
if(CMAKE_LIBRARY_ARCHITECTURE MATCHES "arm")
    set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/arm32)
elseif(CMAKE_LIBRARY_ARCHITECTURE MATCHES "aarch64")
    set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/arm64)
else()
    set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/x86-64)
endif()

target_link_libraries(Jets_Bot 
    PRIVATE
        ${PHOENIX_LIBS_DIR}/libCTRE_Phoenix.so
        ${PHOENIX_LIBS_DIR}/libCTRE_PhoenixCCI.so
        ${PHOENIX_LIBS_DIR}/libCTRE_PhoenixTools.so
)

# Copy Phoenix shared libraries to output folder after build
add_custom_command(TARGET Jets_Bot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PHOENIX_LIBS_DIR}
        $<TARGET_FILE_DIR:Jets_Bot>
    COMMENT "Copying Phoenix CTRE libraries to bin directory"
)

# ==================== Status Message ====================
message(STATUS "=======================================")
message(STATUS "Project: Jets_Bot")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Phoenix Lib Path: ${PHOENIX_LIBS_DIR}")
message(STATUS "=======================================")
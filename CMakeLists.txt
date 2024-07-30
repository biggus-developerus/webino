cmake_minimum_required(VERSION 3.10)

project(webino LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CMK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(LIBS)
set(DEFS)
set(PCHS)

list(APPEND PCHS ${INC_DIR}/webino/net/net_headers.hpp)

add_subdirectory(libs)

list(APPEND LIBS "fmt")

if(WIN32)
    list(APPEND LIBS "winmm" "ws2_32")
endif()

add_library(
    ${PROJECT_NAME}

    ${SRC_DIR}/webino.cpp

    ${SRC_DIR}/net/net.cpp
)

target_precompile_headers(${PROJECT_NAME} PRIVATE ${PCHS})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${DEFS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC ${INC_DIR})

add_executable(${PROJECT_NAME}_NET_TEST tests/net_test.cpp)
target_link_libraries(${PROJECT_NAME}_NET_TEST PRIVATE ${PROJECT_NAME})
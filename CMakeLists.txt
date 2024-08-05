cmake_minimum_required(VERSION 3.10)

project(webino LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(LIBS)
set(DEFS)
set(PCHS)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
list(APPEND PCHS ${INC_DIR}/webino/net/net_necessities.hpp)
list(APPEND LIBS "fmt" "spdlog")

if(WIN32)
    list(APPEND LIBS "winmm" "ws2_32")
endif()

add_subdirectory(libs)

add_library(
    ${PROJECT_NAME}

    ${SRC_DIR}/logging.cpp
    ${SRC_DIR}/webino.cpp
)

target_precompile_headers(${PROJECT_NAME} PRIVATE ${PCHS})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${DEFS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC ${INC_DIR})

include(add_test)
add_test(logging_test tests/logging_test.cpp)
add_test(webino_test tests/webino_test.cpp)

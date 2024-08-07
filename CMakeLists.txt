cmake_minimum_required(VERSION 3.10)

project(webino LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(LIBS)
set(INCS)
set(DEFS)
set(PCHS)

find_package(OpenSSL REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

list(APPEND PCHS ${INC_DIR}/webino/net/net_necessities.hpp)
list(APPEND LIBS "fmt" "spdlog" ${OPENSSL_LIBRARIES})

if(WIN32)
    list(APPEND LIBS "winmm" "ws2_32")
endif()

list(APPEND INCS ${INC_DIR} ${OPENSSL_INCLUDE_DIR})

add_subdirectory(libs)
add_library(
    ${PROJECT_NAME}

    ${SRC_DIR}/logging.cpp
    ${SRC_DIR}/webino.cpp
)

target_precompile_headers(${PROJECT_NAME} PRIVATE ${PCHS})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${DEFS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCS})

include(add_test)

add_test(logging_test tests/logging_test.cpp)
add_test(net_test tests/net_test.cpp)

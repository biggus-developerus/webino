function(add_test test_name test_srcs)
    add_executable(${test_name} ${test_srcs})
    target_link_libraries(${test_name} PRIVATE ${PROJECT_NAME})
endfunction()
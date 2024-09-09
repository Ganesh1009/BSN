# test executables 

include(code-coverage.cmake)
 
add_executable(test_bsn_msg test_bsn_msg.c )
add_dependencies(test_bsn_msg bsn_messaging)
target_link_libraries(test_bsn_msg PRIVATE bsn_messaging)  

add_executable(test_sample_container test_sample_container.c)
add_dependencies(test_sample_container sample_container)
target_link_libraries(test_sample_container PRIVATE sample_container)

add_code_coverage_all_targets()
#the test  
target_code_coverage(test_bsn_msg ALL)
target_code_coverage(test_sample_container ALL)  

add_test(NAME test_bsn_msg COMMAND test_bsn_msg)
add_test(NAME test_sample_container COMMAND test_sample_container)

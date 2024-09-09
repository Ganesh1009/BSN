# library
add_library(sample_container sample_container.c )
target_include_directories(sample_container PUBLIC .)

#the libs 
if(CODE_COVERAGE)
  target_code_coverage(sample_container ALL)
endif()
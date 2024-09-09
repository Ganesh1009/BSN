# library
add_library(bsn_messaging bsn_crc16.c bsn_crc32.c bsn_msg.c) 
target_include_directories(bsn_messaging PUBLIC .)
   
#the libs 
if(CODE_COVERAGE)
  target_code_coverage(bsn_messaging ALL)
endif()

target_include_directories(bsn_messaging INTERFACE "." ) 

install(TARGETS bsn_messaging DESTINATION lib)
install(FILES bsn_crc16.h bsn_crc32.h bsn_msg.h DESTINATION include)

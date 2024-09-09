cmake_minimum_required(VERSION 3.14)  


add_executable(sn-client-tests 
	catch_main.cpp
	SNClient_test.cpp 
	SNClientTransport_test.cpp
	SNProtocol_test.cpp
	SNSerialMessageFraming_test.cpp
	SNSensorTypes_test.cpp)
#
#add_executable(sn-client-types-tests 
#	catch_main.cpp 
#	SNSensorTypes_test.cpp)
	
add_executable(sn-client-hardware-tests 
	catch_main.cpp 
	SNClient_endtoend_test.cpp 
	SNClientTransportSerialPort_test.cpp 
	)

add_dependencies(sn-client-tests sn-client-library )
target_link_libraries(sn-client-tests PRIVATE
						sn-client-library 
						#asio::asio_static
						Catch2::Catch2
						Threads::Threads 
						) 
#
#add_dependencies(sn-client-types-tests sn-client-library )
#target_link_libraries(sn-client-types-tests PRIVATE
#						sn-client-library 
#						Catch2::Catch2 
#						) 
						
add_dependencies(sn-client-hardware-tests sn-client-library )
target_link_libraries(sn-client-hardware-tests PRIVATE
						sn-client-library 
						Catch2::Catch2
						Threads::Threads 
						) 
						
add_test(NAME sn-tests COMMAND sn-client-tests)
#add_test(NAME sn-types-tests COMMAND sn-client-types-tests)
add_test(NAME sn-serial-tests COMMAND sn-client-hardware-tests)

setup_target_for_coverage_lcov(
		NAME coverage_snclient
		EXECUTABLE sn-client-tests
		DEPENDENCIES sn-client-library  
		BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
		#EXCLUDE "extern/** tests/** /usr/** include/*"      
		)
#
#setup_target_for_coverage_lcov(
#		NAME coverage_types
#		EXECUTABLE sn-client-types-tests
#		DEPENDENCIES sn-client-library  
#		BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
#		#EXCLUDE "extern/** tests/** /usr/** include/* usr** lib** /**"      
#		)
		  
setup_target_for_coverage_lcov(
		NAME coverage_hardware
		EXECUTABLE sn-client-hardware-tests
		DEPENDENCIES sn-client-library        
        BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
		#EXCLUDE "extern/** tests/** /usr/** include/*"      
		)
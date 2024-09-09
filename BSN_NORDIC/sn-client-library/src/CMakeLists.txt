add_library(sn-client-library 
	Debug.cpp
	SNClient.cpp
	SNClientImpl.cpp
	SNClientTransport.cpp
	SNClientTransportSerialPort.cpp
	SNProtocol.cpp
	SNSensorTypes.cpp
	SNSerialMessageFraming.cpp
	SNUtils.cpp
)
add_dependencies(sn-client-library 
	bsn_messaging 
	#spdlog::spdlog
	asio
	#mars-logging
) 
target_link_libraries(sn-client-library 
	PRIVATE 
	asio	
	bsn_messaging 
	#spdlog::spdlog
	#mars-logging
) 

set_target_properties(sn-client-library PROPERTIES POSITION_INDEPENDENT_CODE ON)


target_include_directories(sn-client-library PUBLIC "." )  
target_include_directories(sn-client-library PUBLIC "${PROJECT_SOURCE_DIR}/include" ) 
# target_include_directories(sn-client-library INTERFACE  
# $<BUILD_INTERFACE:"${PROJECT_SOURCE_DIR}/include">
# $<INSTALL_INTERFACE:include> ) 
# target_include_directories(sn-client-library INTERFACE  
# $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
# $<INSTALL_INTERFACE:include> ) 

SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Mathias Musahl (mathias.musahl@dfki.de)") #required

install(TARGETS sn-client-library
	#asio::asio_shared	
	#bsn_messaging
	DESTINATION lib EXPORT sn-client-library) 
install(FILES "${PROJECT_SOURCE_DIR}/include/SNClient.h" DESTINATION include)
install(FILES "${PROJECT_SOURCE_DIR}/include/SNClientLib.h" DESTINATION include)
install(FILES "${PROJECT_SOURCE_DIR}/include/SNClientTransport.h" DESTINATION include)
install(FILES "${PROJECT_SOURCE_DIR}/include/SNProtocol.h" DESTINATION include)
install(FILES "${PROJECT_SOURCE_DIR}/include/SNSensorTypes.h" DESTINATION include)
install(FILES "${PROJECT_SOURCE_DIR}/src/SNClientTransportSerialPort.h" DESTINATION include) 

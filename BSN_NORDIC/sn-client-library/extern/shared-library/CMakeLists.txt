cmake_minimum_required (VERSION 3.14)

# git information
find_package(Git QUIET)
if(GIT_FOUND)
	execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    	            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        	        OUTPUT_VARIABLE PACKAGE_GIT_VERSION
            	    ERROR_QUIET
                	OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

# get build number
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.buildnumber")
	file (STRINGS ".buildnumber" BUILD_NUMBER)
else()
	set(BUILD_NUMBER 0)
endif()

project(sn-shared-library VERSION 0.0.0.${BUILD_NUMBER})  
message("CMAKE_PROJECT_NAME = ${CMAKE_PROJECT_NAME}")
message("PROJECT_NAME = ${PROJECT_NAME}")
message("CMAKE_PROJECT_VERSION = ${CMAKE_PROJECT_VERSION}")
message("PACKAGE_GIT_VERSION = ${PACKAGE_GIT_VERSION}")

# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
# add tests and cppcheck only if we are no submodule
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

# add option to enable testing
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

	# cppcheck integration
	option(ENABLE_CPPCHECK "Enables running cppcheck" OFF)
	if(ENABLE_CPPCHECK)
		find_program(CMAKE_CXX_CPPCHECK cppcheck NAMES cppcheck)
		if(CMAKE_CXX_CPPCHECK)
			message(STATUS "cppcheck found ${CMAKE_CXX_CPPCHECK}")
			set(CMAKE_CXX_CPPCHECK ${CMAKE_CXX_CPPCHECK})
			set(CMAKE_C_CPPCHECK ${CMAKE_CXX_CPPCHECK})
		else()
			message(WARNING "cppcheck NOT found - please add it for static code analyis")
		endif(CMAKE_CXX_CPPCHECK)
	endif()

	option(ENABLE_TESTING "Enables tests for the library" OFF)
	if(ENABLE_TESTING)
		include(CTest)
		enable_testing()
		add_subdirectory(tests)
	endif() 
	
	message(WARNING "ENABLE_TESTING = ${ENABLE_TESTING}")
	message(WARNING "CODE_COVERAGE = ${CODE_COVERAGE}")
endif()

# subdirectories
add_subdirectory(messaging)
add_subdirectory(sample_container) 

 

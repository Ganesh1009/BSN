cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
# set up hunter for dependency management
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
include("extern/huntergate/cmake/HunterGate.cmake")
# HunterGate(
#     URL "https://github.com/ruslo/hunter/archive/v0.23.214.tar.gz"
#     SHA1 "e14bc153a7f16d6a5eeec845fb0283c8fad8c358"
# )
#HunterGate(
#    URL "https://github.com/cpp-pm/hunter/archive/v0.23.284.tar.gz"
#    SHA1 "b15e14b31d96ee2e2be32187d21e2390d0b974c8"
#)
HunterGate(
    URL "https://github.com/cpp-pm/hunter/archive/v0.23.300.tar.gz"
    SHA1 "1151d539465d9cdbc880ee30f794864aec11c448"
)
# >>>>>>>>>>>>>>>>>>>>>>>
# configure project / git versioning
# >>>>>>>>>>>>>>>>>>>>>>>

# git information
find_package(Git QUIET)
if(GIT_FOUND)
	# Get the current working branch
	execute_process(
	  COMMAND git rev-parse --abbrev-ref HEAD
	  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	  OUTPUT_VARIABLE GIT_BRANCH
	  OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	
	# Get the latest short hash
	execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    	            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        	        OUTPUT_VARIABLE GIT_VERSION
            	    ERROR_QUIET
                	OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

# get build number
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.buildnumber")
	file (STRINGS ".buildnumber" BUILD_NUMBER)
else()
	set(BUILD_NUMBER 0)
endif()

string(TIMESTAMP _output "%d/%m/%Y")
string(TIMESTAMP PROJECT_DATE "%Y-%m-%d %H:%M:%S")

#############################
# PROJECT VERSION
#############################
project(sn-client-library VERSION 0.1.1.${BUILD_NUMBER})
 
message("CMAKE_PROJECT_NAME = ${CMAKE_PROJECT_NAME}")
message("CMAKE_PROJECT_VERSION = ${CMAKE_PROJECT_VERSION}")
message("PROJECT_DATE = ${PROJECT_DATE}")  
message("PROJECT_NAME = ${PROJECT_NAME}")
message("PROJECT_VERSION = ${PROJECT_VERSION}")
message("PROJECT_VERSION_MAJOR = ${PROJECT_VERSION_MAJOR}")
message("PROJECT_VERSION_MINOR = ${PROJECT_VERSION_MINOR}")
message("PROJECT_VERSION_PATCH = ${PROJECT_VERSION_PATCH}")
message("PROJECT_VERSION_TWEAK = ${PROJECT_VERSION_TWEAK}")
message("GIT_VERSION = ${GIT_VERSION}") 
message("GIT_BRANCH = ${GIT_BRANCH}")  

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

configure_file("${PROJECT_SOURCE_DIR}/cmake/config.h.in" "${PROJECT_SOURCE_DIR}/src/config.h")

#add_compile_definitions(LOGGING_LEVEL_CUTOFF=2)
#add_compile_definitions(MARS_LOGGING_ENABLE)

# >>>>>>>>>>>>>>>>>>>>>>>
# compiler settings
# >>>>>>>>>>>>>>>>>>>>>>> 

set(CMAKE_CXX_STANDARD 17)  
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS OFF)

# >>>>>>>>>>>>>>>>>>>>>>>
# dependencies
# >>>>>>>>>>>>>>>>>>>>>>> 

# >>>>>>>>>>>>>>>>>>>>>>>
# dependencies
# >>>>>>>>>>>>>>>>>>>>>>> 
include(FetchContent)

FetchContent_Declare(asio
  #GIT_REPOSITORY git@github.com:chriskohlhoff/asio.git
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG master
  GIT_SHALLOW     TRUE
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)

FetchContent_GetProperties(asio)

if(NOT asio_POPULATED)
FetchContent_Populate(asio)
endif()

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
message(STATUS "asio_SOURCE_DIR found ${asio_SOURCE_DIR}") 

find_package(Threads)
target_link_libraries(asio INTERFACE Threads::Threads)

# asio needs Threads
FIND_PACKAGE(Threads REQUIRED) 

# use asio to avoid boost usage
#hunter_add_package(asio) 
#FIND_PACKAGE(asio CONFIG REQUIRED)

# spdlog library
hunter_add_package(spdlog)
find_package(spdlog CONFIG REQUIRED)

 
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
# add tests and cppcheck only if we are no submodule
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

# add option to enable testing
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	option(ENABLE_TESTING "Enables tests for the library" OFF)
	option(ENABLE_CPPCHECK "Enables running cppcheck" OFF)
	if(ENABLE_TESTING)
		message(STATUS "TESTING ENABLED")
		########################
		# add code coverage
		########################

		include(CodeCoverage)
		append_coverage_compiler_flags()
		file(REMOVE_RECURSE ${PROJECT_BINARY_DIR}/*.gcda)
		set(COVERAGE_EXCLUDES "tests/*" "build/*" "*extern/*" )

				
		#########################################
		# enable cppcheck integration if found 
		#########################################
		if(ENABLE_CPPCHECK)
			find_program(CMAKE_CXX_CPPCHECK cppcheck NAMES cppcheck)
			if(CMAKE_CXX_CPPCHECK)
				message(STATUS "cppcheck found ${CMAKE_CXX_CPPCHECK}")
				message(STATUS "cppcheck enabled")
				set(CMAKE_CXX_CPPCHECK ${CMAKE_CXX_CPPCHECK})
				set(CMAKE_C_CPPCHECK ${CMAKE_CXX_CPPCHECK})
			else()
				message(WARNING "cppcheck NOT found - please add it for static code analyis")
			endif(CMAKE_CXX_CPPCHECK)
		endif()
		
		######################## 
		# enable testing
		########################
		include(CTest)
		enable_testing()
		
		#########################################
		# include catch2 framework for testing
		#########################################
		
		#use hunter to download the depenency if it's not installed
		hunter_add_package(Catch)
		FIND_PACKAGE(Catch2)  

		#########################################
		# add test folder
		#########################################
		add_subdirectory(tests)
	else()
		message(STATUS "TESTING DISABLED")
	endif() 
endif()

# >>>>>>>>>>>>>>>>>>>>>>>
# folders, files, includes
# >>>>>>>>>>>>>>>>>>>>>>>

# Include subfolders
add_subdirectory(apps)
add_subdirectory(extern)
add_subdirectory(src)
add_subdirectory(docs)

# install(EXPORT sn-client-library
#   FILE sn-client-library.cmake
#   DESTINATION lib/cmake/sn-client-library
# )

# >>>>>>>>>>>>>>>>>>>>>>>
# packaging
# >>>>>>>>>>>>>>>>>>>>>>>
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION "sn-client-library - used for connecting to BIONIC and DFKI sensor networks")
include(CPack)


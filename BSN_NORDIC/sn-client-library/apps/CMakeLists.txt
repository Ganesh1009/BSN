# cxxopts instead of program options
hunter_add_package(cxxopts)
find_package(cxxopts CONFIG REQUIRED)

add_executable(sn-client-cli sn-client-cli.cpp)

target_link_libraries(sn-client-cli 
	sn-client-library 
	cxxopts::cxxopts
	spdlog::spdlog
)

add_dependencies(sn-client-cli
	sn-client-library 
	spdlog::spdlog )

install(TARGETS sn-client-cli DESTINATION bin)

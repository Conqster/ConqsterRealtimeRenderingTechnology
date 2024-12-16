message(STATUS "Checking for YAML package for ${PROJECT_NAME}")

#Try find prebuilt YAML installation
set(YAML_FLODER_NAME "yaml-cpp-installed-experiment")
find_path(YAML_ROOT_DIR 
		  NAMES debug/include 
		  HINTS ${CMAKE_CURRENT_SOURCE_DIR}/libs/${YAML_FLODER_NAME})


if(YAML_ROOT_DIR)
	

	set(YAML_DEBUG_DIR ${YAML_ROOT_DIR}/debug)
	set(YAML_RELEASE_DIR ${YAML_ROOT_DIR}/release)
	find_path(YAML_ALL_INCLUDES_DIR
			  NAMES yaml-cpp/yaml.h
			  HINTS ${YAML_RELEASE_DIR}/include)
	find_library(YAML_LIBRARY_DEBUG
				 NAMES yaml-cppd.lib
				 HINTS ${YAML_DEBUG_DIR}/lib)
	find_library(YAML_LIBRARY_RELEASE
				 NAMES yaml-cpp.lib
				 HINTS ${YAML_RELEASE_DIR}/lib)	 
	set(YAML_LIBRARIES
					  optimized ${YAML_LIBRARY_RELEASE}
					  debug ${YAML_LIBRARY_DEBUG})
	
	#extract 
	get_filename_component(YAML_LIB_DEBUG_NAME ${YAML_LIBRARY_DEBUG} NAME)
	get_filename_component(YAML_LIB_REL_NAME ${YAML_LIBRARY_RELEASE} NAME)
	
	set(YAML_LIBRARIES_BY_NAME 
					  optimized ${YAML_LIB_REL_NAME}
					  debug ${YAML_LIB_DEBUG_NAME})

	
	if(NOT YAML_LIBRARY_RELEASE)
		message(FATAL_ERROR "YAML build failed: ${YAML_LIBRARY_RELEASE}")
	elseif(NOT YAML_LIBRARY_DEBUG)
		message(FATAL_ERROR "YAML build failed: ${YAML_LIBRARY_DEBUG}")
	elseif(NOT YAML_ALL_INCLUDES_DIR)
		message(FATAL_ERROR "YAML include failed: ${YAML_ALL_INCLUDES_DIR}")
	endif()
	
	message(STATUS "My yaml library dir path.. : ${YAML_LIBRARIES}")
	message(STATUS "YAML successful.\n Include: ${YAML_ALL_INCLUDES_DIR},\n Library: ${YAML_LIBRARY_RELEASE},\n Library: ${YAML_LIBRARY_DEBUG}")
	
	
else()
	#prebuilt not found; fall-back to building from source
	message(STATUS "Prebuilt YAML not found. Attempting to build from source.")
	
	find_path(YAML_SOURCE_DIR
			  NAMES CMakeLists.txt
			  #HINTS ${CMAKE_SOURCE_DIR}/libs/yaml-cpp)
			  HINTS ${PROJECT_SOURCE_DIR}/libs/yaml-cpp)
			  
	if(NOT YAML_SOURCE_DIR)
		message(FATAL_ERROR "YAML source directory not found")
	endif()
	
	message(STATUS "Found Source Project with CMake: ${YAML_SOURCE_DIR}")
	
	
	#add YAML as a subdir
	
	#set(YAML_INSTALL_DIR ${CMAKE_BINARY_DIR}/yaml-installed)
	#set(YAML_INSTALL_DIR ${CMAKE_SOURCE_DIR}/libs/${YAML_FLODER_NAME})
	set(YAML_INSTALL_DIR ${PROJECT_SOURCE_DIR}/libs/${YAML_FLODER_NAME})
	
	message(STATUS "Configuring YAML build in: ${YAML_BUILD_DIR}")
	message(STATUS "Installing YAML to: ${YAML_INSTALL_DIR}")
	
	#Debug
	set(YAML_BUILD_DEBUG_DIR ${CMAKE_BINARY_DIR}/yaml-build-debug)
	set(YAML_CMAKE_BUILD_TYPE Debug)
	execute_process(COMMAND ${CMAKE_COMMAND} -S ${YAML_SOURCE_DIR} 
											 -B ${YAML_BUILD_DEBUG_DIR} 
											 -DCMAKE_INSTALL_PREFIX=${YAML_INSTALL_DIR}/debug
											 -DCMAKE_BUILD_TYPE=${YAML_CMAKE_BUILD_TYPE}
											 #hardcoded for now
											 -A Win32)
							
	execute_process(COMMAND ${CMAKE_COMMAND} --build ${YAML_BUILD_DEBUG_DIR}
											 --config Debug
											 --target install)
											 
	
	#Release 
	set(YAML_BUILD_RELEASE_DIR ${CMAKE_BINARY_DIR}/yaml-build-release)
	set(YAML_CMAKE_BUILD_TYPE Release)
	execute_process(COMMAND ${CMAKE_COMMAND} -S ${YAML_SOURCE_DIR} 
											 -B ${YAML_BUILD_RELEASE_DIR} 
											 -DCMAKE_INSTALL_PREFIX=${YAML_INSTALL_DIR}/release
											 -DCMAKE_BUILD_TYPE=${YAML_CMAKE_BUILD_TYPE}
											 #hardcoded for now
											 -A Win32)
							
	execute_process(COMMAND ${CMAKE_COMMAND} --build ${YAML_BUILD_RELEASE_DIR}
											 --config Release
											 --target install)
											 
	#clean up after build
	option(CLEAN_YAML_BUILD_DIRS "Delete YAML build directories after generation" ON)
	if(CLEAN_YAML_BUILD_DIRS)
		if(EXISTS ${YAML_BUILD_DEBUG_DIR})
			file(REMOVE_RECURSE ${YAML_BUILD_DEBUG_DIR})
		endif()
		if(EXISTS ${YAML_BUILD_RELEASE_DIR})
			file(REMOVE_RECURSE ${YAML_BUILD_RELEASE_DIR})
		endif()
	endif()

	
	set(YAML_DEBUG_DIR ${YAML_INSTALL_DIR}/debug)
	set(YAML_RELEASE_DIR ${YAML_INSTALL_DIR}/release)
	find_path(YAML_ALL_INCLUDES_DIR
				 NAMES yaml-cpp/yaml.h
				 HINTS ${YAML_RELEASE_DIR}/include)
	
	#get/find recommended libs 	 
	find_library(YAML_LIBRARY_DEBUG
				 NAMES yaml-cppd.lib
				 HINTS ${YAML_DEBUG_DIR}/lib)
	find_library(YAML_LIBRARY_RELEASE
				 NAMES yaml-cpp.lib
				 HINTS ${YAML_RELEASE_DIR}/lib)	 
	
	set(YAML_LIBRARIES 
					  optimized ${YAML_LIBRARY_RELEASE}
					  debug ${YAML_LIBRARY_DEBUG})
					  
	#extract 
	get_filename_component(YAML_LIB_DEBUG_NAME ${YAML_LIBRARY_DEBUG} NAME)
	get_filename_component(YAML_LIB_REL_NAME ${YAML_LIBRARY_RELEASE} NAME)
	
	set(YAML_LIBRARIES_BY_NAME 
					  optimized ${YAML_LIB_REL_NAME}
					  debug ${YAML_LIB_DEBUG_NAME})

	
	if(NOT YAML_LIBRARY_RELEASE)
		message(FATAL_ERROR "YAML build failed: ${YAML_LIBRARY_RELEASE}")
	elseif(NOT YAML_LIBRARY_DEBUG)
		message(FATAL_ERROR "YAML build failed: ${YAML_LIBRARY_DEBUG}")
	elseif(NOT YAML_ALL_INCLUDES_DIR)
		message(FATAL_ERROR "YAML include failed: ${YAML_ALL_INCLUDES_DIR}")
	endif()
	
	message(STATUS "My yaml library dir path.. : ${YAML_LIBRARIES}")
	message(STATUS "Built YAML successfully.\n Include: ${YAML_ALL_INCLUDES_DIR},\n Library: ${YAML_LIBRARY_RELEASE},\n Library: ${YAML_LIBRARY_DEBUG}")
	
endif()
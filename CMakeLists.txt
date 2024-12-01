cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME CRRT)

#generate project
project(${PROJECT_NAME})


#the C++ use standard 
#add_library(crrt_compiler_flags INTERFACE)
#ISO c++ 17 standard required for features like (std::hash, std::unorderedmap, std::filesystem, etc)
#target_compile_features(crrt_compiler_flags INTERFACE cxx_std_17) 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR})

#create list of cmake helpers 
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/libs_cmake)
#add glew 
find_package(GLEW REQUIRED)

#add assimp
find_package(ASSIMP REQUIRED)
#add yaml if required
option(USE_YAML "Use YAML in project for serialisation" ON)
if(USE_YAML)
	find_package(YAML REQUIRED)
endif()

#find all source files
file(GLOB_RECURSE source_files "src/*.h" "src/*.cpp")
file(GLOB_RECURSE library_files "libs*.h" "libs*.c" "libs*.cpp")
file(GLOB imgui_source "libs/imgui/*.h" "libs/imgui/*.cpp")
file(GLOB_RECURSE yaml_src "libs/yaml-cpp-installed/include/yaml-cpp/*.h" "libs/yaml-cpp-installed/include/yaml-cpp/*.cpp")
set(ALL_SRC_FILES ${source_files} ${library_files} ${imgui_source} ${yaml_src})
add_executable(${PROJECT_NAME} ${ALL_SRC_FILES})


#add binary I think ??
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})

#glew
target_link_directories(${PROJECT_NAME} PUBLIC ${GLEW_LIBRARY_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${GLEW_INCLUDE_DIR})

#assimp
target_include_directories(${PROJECT_NAME} PUBLIC ${ASSIMP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${ASSIMP_LIBRARY_DIR})

#YAML
if(USE_YAML)
	target_include_directories(${PROJECT_NAME} PUBLIC ${YAML_ALL_INCLUDES_DIR})
	#YAML_LIBRARIES => includes both debug & Release libraries
	#target_link_directories(${PROJECT_NAME} PUBLIC ${YAML_LIBRARIES})
	#wrong directory use below
	target_link_directories(${PROJECT_NAME} PUBLIC 
												  optimized ${YAML_RELEASE_DIR}/lib
												  debug ${YAML_DEBUG_DIR}/lib)
	message(STATUS "Successfully found libraries: ${YAML_LIBRARIES_BY_NAME}")
	
	#Preprocessor definations
	target_compile_definitions(${PROJECT_NAME} PRIVATE YAML_CPP_STATIC_DEFINE)
endif()

#Additional Include Directories (Properties > C/C++ > General > this)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})#/libs)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src/Core)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/libs/glfw/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/libs/GLM)

#yaml 


target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})

#Additional Library Directories (Properties > Linker > General > this)
#Later use command to find cmake for glew add build if x86 or x64 
target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/libs/glfw/lib-vc2022)
#yaml 



#Link Dependencies
target_link_libraries(${PROJECT_NAME} opengl32.lib glew32.lib glfw3.lib assimp-vc143-mt.lib ${YAML_LIBRARIES_BY_NAME})
#target_link_libraries(${PROJECT_NAME} opengl32.lib glew32.lib glfw3.lib assimp-vc143-mt.lib yaml-cppd.lib)
#target_link_libraries(${PROJECT_NAME} opengl32.lib glew32.lib glfw3.lib ${ASSIMP_LIBRARY} yaml-cppd.lib)



#set project working directory
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

#set CRRT as the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})


#Copy require dll files to output directory (glew32.dll, assimp-vc143-mt.dll)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
				   COMMAND ${CMAKE_COMMAND} -E copy_if_different
						   #copy glew dll file
						   ${PROJECT_SOURCE_DIR}/libs/glew/bin/Release/Win32/glew32.dll  
						   #copy assimp dll file
						   ${PROJECT_SOURCE_DIR}/libs/Assimp/bin/assimp-vc143-mt.dll
						   $<TARGET_FILE_DIR:${PROJECT_NAME}>)


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


option(STANDALONE "Build CRRT as a standalone project with its own main" OFF)

#create list of cmake helpers 
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs_cmake)


#Always required: GLEW and ASSIMP
find_package(GLEW REQUIRED)
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
list(APPEND ALL_SRC_FILES ${source_files} ${library_files} ${imgui_source})



if(USE_YAML)
	file(GLOB_RECURSE yaml_src "libs/yaml-cpp-installed/include/yaml-cpp/*.h" 
							   "libs/yaml-cpp-installed/include/yaml-cpp/*.cpp")
	list(APPEND ALL_SRC_FILES ${yaml_src})
endif()

#
set(ADDITIONAL_INC_DIR  
					  ${PROJECT_SOURCE_DIR}/src
					  ${PROJECT_SOURCE_DIR}
					  ${PROJECT_SOURCE_DIR}/src/Core
					  ${PROJECT_SOURCE_DIR}/libs/glfw/include
					  ${PROJECT_SOURCE_DIR}/libs/GLM
					  ${GLEW_INCLUDE_DIR}
					  ${ASSIMP_INCLUDE_DIR}
					  )
					  
set(REQ_LINK_DIRECTORIES 
						${GLEW_LIBRARY_DIR}							#GLEW
						${ASSIMP_LIBRARY_DIR}						#ASSIMP
						${PROJECT_SOURCE_DIR}/libs/glfw/lib-vc2022 	#GLFW
						)	
			
#Link Libraries
set(REQ_LINK_LIB
				opengl32.lib        				# OpenGL
				glew32.lib 			# GLEW
				glfw3.lib 							# GLFW
				assimp-vc143-mt.lib # ASSIMP
				)



if(NOT STANDALONE)
	add_library(${PROJECT_NAME} STATIC ${ALL_SRC_FILES})
else()
	add_executable(${PROJECT_NAME} ${ALL_SRC_FILES})
endif()

#Additional Include Directories (Properties > C/C++ > General > this)
target_include_directories(${PROJECT_NAME} PUBLIC ${ADDITIONAL_INC_DIR})

target_link_directories(${PROJECT_NAME} PUBLIC ${REQ_LINK_DIRECTORIES})

#Link Libraries
target_link_libraries(${PROJECT_NAME} PUBLIC ${REQ_LINK_LIB})

# required for asset using
if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})										
	target_compile_definitions(${PROJECT_NAME} PUBLIC CRRT_ASSETS_PATH="Assets")
else()									
	get_filename_component(CRRT_FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)	
	target_compile_definitions(${PROJECT_NAME} PUBLIC CRRT_ASSETS_PATH="${CRRT_FOLDER_NAME}/Assets")
endif()
											
if(USE_YAML)
	target_include_directories(${PROJECT_NAME} PUBLIC ${YAML_ALL_INCLUDES_DIR})
	#YAML_LIBRARIES => includes both debug & Release libraries
	#target_link_directories(${PROJECT_NAME} PUBLIC ${YAML_LIBRARIES})
	#wrong directory use below
	target_link_directories(${PROJECT_NAME} PUBLIC 
												  optimized ${YAML_RELEASE_DIR}/lib
												  debug ${YAML_DEBUG_DIR}/lib)
	#Preprocessor definations											  
	target_compile_definitions(${PROJECT_NAME} PRIVATE YAML_CPP_STATIC_DEFINE)
	target_link_libraries(${PROJECT_NAME} PUBLIC ${YAML_LIBRARIES_BY_NAME})
	message(STATUS "Successfully found libraries: ${YAML_LIBRARIES_BY_NAME}")
endif()



if(STANDALONE)
	message(STATUS "CRRT as a standalone")
	
	#target_link_libraries(${PROJECT_NAME}_Standalone ${PROJECT_NAME})
#set project working directory
	set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
	#Copy require dll files to output directory (glew32.dll, assimp-vc143-mt.dll)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
				   COMMAND ${CMAKE_COMMAND} -E copy_if_different
						   #copy glew dll file
						   ${PROJECT_SOURCE_DIR}/libs/glew/bin/Release/Win32/glew32.dll  
						   #copy assimp dll file
						   ${PROJECT_SOURCE_DIR}/libs/Assimp/bin/assimp-vc143-mt.dll
						   $<TARGET_FILE_DIR:${PROJECT_NAME}>)
else()
	message(STATUS "CRRT as a module")
endif()

#set_target_properties(CRRT_Test ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})


#Copy require dll files to output directory (glew32.dll, assimp-vc143-mt.dll)
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
#				   COMMAND ${CMAKE_COMMAND} -E copy_if_different
#						   #copy glew dll file
#						   ${GLEW_DLL} 
#						   #copy assimp dll file
#						   ${ASSIMP_DLL}
#						   $<TARGET_FILE_DIR:${PROJECT_NAME}_Test>)


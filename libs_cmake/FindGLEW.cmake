
message(STATUS "Found and Loading GLEW package for ${PROJECT_NAME}")

#GLEW root dir
set(GLEW_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/glew)

#finds glew inc dir
find_path(GLEW_INCLUDE_DIR
		  NAMES GL/glew.h
		  HINTS ${GLEW_ROOT_DIR}/include)
		  
find_path(GLEW_LIBRARY_DIR
		  NAMES glew32.lib
		  HINTS ${GLEW_ROOT_DIR}/lib/Release/Win32)
		  

find_library(GLEW_LIBRARY_RELEASE
			 NAMES glew32.lib
			 PATHS ${GLEW_LIBRARY_DIR})
			 
#find dll
find_file(GLEW_DLL
		  NAMES glew32.dll
		  PATHS ${GLEW_ROOT_DIR}/bin/Release/Win32)
			 
if(GLEW_LIBRARY_RELEASE AND GLEW_DLL)
	message(STATUS "Found GLEW release library: ${GLEW_LIBRARY_RELEASE}")
	message(STATUS "Found GLEW DLL: ${GLEW_DLL}")
else()
	message(FATAL_ERROR "Could not found GLEW release library OR DLL")
endif()

#GLEW::GLEW as an interface library
#add_library(GLEW::GLEW INTERFACE)

#target_include_directories(GLEW::GLEW INTERFACE ${GLEW_INCLUDE_DIR})
#target_link_libraries(GLEW::GLEW INTERFACE ${GLEW_LIBRARY_RELEASE})



message(STATUS "My GLEW include dir: ${GLEW_INCLUDE_DIR}")
message(STATUS "My GLEW library dir path : ${GLEW_LIBRARY_DIR}")
message(STATUS "My GLEW library : ${GLEW_LIBRARY_RELEASE}")

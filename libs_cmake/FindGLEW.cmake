
message(STATUS "Found and Loading GLEW package for ${PROJECT_NAME}")

#GLEW root dir
set(GLEW_ROOT_DIR ${CMAKE_SOURCE_DIR}/libs/glew)

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
			 
if(GLEW_LIBRARY_RELEASE)
	message(STATUS "Found GLEW release library: ${GLEW_LIBRARY_RELEASE}")
else()
	message(FATAL_ERROR "Could not found GLEW release library: ${GLEW_LIBRARY_RELEASE}")
endif()



message(STATUS "My GLEW include dir: ${GLEW_INCLUDE_DIR}")
message(STATUS "My GLEW library dir path : ${GLEW_LIBRARY_DIR}")
message(STATUS "My GLEW library : ${GLEW_LIBRARY_RELEASE}")

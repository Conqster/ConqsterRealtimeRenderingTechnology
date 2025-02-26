
message(STATUS "Found and Loading Assimp package for ${PROJECT_NAME}")

#Assimp root dir 
set(ASSIMP_ROOT_DIR ${CMAKE_SOURCE_DIR}/libs/Assimp)

#finds assimp inc dir
find_path(ASSIMP_INCLUDE_DIR 
		  NAMES assimp/anim.h 
		  HINTS ${ASSIMP_ROOT_DIR}/include)

set(ASSIMP_MSVC_VERSION "vc143")
find_path(ASSIMP_LIBRARY_DIR 
		  NAMES assimp-${ASSIMP_MSVC_VERSION}-mt.lib 
		  HINTS ${ASSIMP_ROOT_DIR}/lib)

find_library(ASSIMP_LIBRARY_RELEASE 
			 NAMES assimp-${ASSIMP_MSVC_VERSION}-mt.lib 
			 PATHS ${ASSIMP_LIBRARY_DIR})

set(ASSIMP_LIBRARY 
				  optimized ${ASSIMP_LIBRARY_RELEASE} 
				  debug ${ASSIMP_LIBRARY_RELEASE}) 

message(STATUS "My assimp include dir: ${ASSIMP_INCLUDE_DIR}")
message(STATUS "My msvc version for assimp: ${ASSIMP_MSVC_VERSION}")
message(STATUS "My assimp library dir path : ${ASSIMP_LIBRARY_DIR}")
message(STATUS "My assimp library dir path.. : ${ASSIMP_LIBRARY}")
message(STATUS "My assimp library : ${ASSIMP_LIBRARY_RELEASE}")


if(NOT ASSIMP_LIBRARY_RELEASE)
	message(FATAL_ERROR "Could not find assimp release library: ${ASSIMP_LIBRARY_RELEASE}")
else()
	message(STATUS "Found Assimp release library: ${ASSIMP_LIBRARY_RELEASE}")
endif()
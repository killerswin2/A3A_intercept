cmake_minimum_required (VERSION 3.0)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
option(BUILD_TESTING "" OFF)
add_subdirectory(vendors/glm)

file(GLOB_RECURSE INTERCEPT_PLUGIN_SOURCES *.h *.hpp *.c *.cpp)
SOURCE_GROUP("src" FILES ${INTERCEPT_PLUGIN_SOURCES})

file(GLOB INTERCEPT_plugin_Geometry_SOURCES "Geometry/*.cpp" "Geometry/*.hpp" "Geometry/*.h")
SOURCE_GROUP("src/Geometry" FILES ${INTERCEPT_plugin_Geometry_SOURCES})

file(GLOB INTERCEPT_plugin_debugging_SOURCES "debugging/*.cpp" "debugging/*.hpp" "debugging/*.h")
SOURCE_GROUP("src/debugging" FILES ${INTERCEPT_plugin_debugging_SOURCES})

#If you want to split your source files into different directories you can do so here

#The SOURCE_GROUP string is the directory it will display as inside your visual studio.
#Here is a example of a "utilities" subdirectory.

#file(GLOB INTERCEPT_plugin_utilities_SOURCES "utilities/*.cpp" "utilities/*.hpp" "utilities/*.h")
#SOURCE_GROUP("src/utilities" FILES ${INTERCEPT_plugin_utilities_SOURCES})

#----Don't change anything below this line


#include the Intercept headers from the submodule
set(INTERCEPT_CLIENT_PATH "${CMAKE_SOURCE_DIR}/intercept/src/client")

set(INTERCEPT_INCLUDE_PATH "${INTERCEPT_CLIENT_PATH}/headers" "${INTERCEPT_CLIENT_PATH}/headers/shared" "${INTERCEPT_CLIENT_PATH}/headers/client/" "${INTERCEPT_CLIENT_PATH}/headers/client/sqf")

if(USE_64BIT_BUILD)
    set(INTERCEPT_PLUGIN_NAME "${CMAKE_PROJECT_NAME}_x64")
else()
    set(INTERCEPT_PLUGIN_NAME "${CMAKE_PROJECT_NAME}")
endif()

add_definitions(/DINTERCEPT_NO_THREAD_SAFETY)

if(USE_ENGINE_TYPES)
    add_definitions(/DINTERCEPT_SQF_STRTYPE_RSTRING)
endif()

file(GLOB INTERCEPT_HOST_SOURCES "${INTERCEPT_CLIENT_PATH}/intercept/client/*.cpp"  "${INTERCEPT_CLIENT_PATH}/intercept/client/sqf/*.cpp" "${INTERCEPT_CLIENT_PATH}/intercept/shared/*.cpp")
SOURCE_GROUP("intercept" FILES ${INTERCEPT_HOST_SOURCES})

add_library( ${INTERCEPT_PLUGIN_NAME} SHARED ${INTERCEPT_PLUGIN_SOURCES} ${INTERCEPT_HOST_SOURCES} "hashing/jsonHashMap.cpp"  "hashing/jsonHashMap.hpp" "physics/towing.cpp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${INTERCEPT_INCLUDE_PATH})

# Newton libs and header files
include_directories(../Dependencies/ND/include/ndNewton)
set(CMAKE_LIBRARY_PATH ../Dependencies/ND/lib)

# Ensure the Newton libraries are installed.
find_library(NEWTON_LIB ndNewton REQUIRED)
find_library(AVX_LIB ndSolverAvx2 REQUIRED)

target_link_libraries(${INTERCEPT_PLUGIN_NAME} glm::glm ${NEWTON_LIB} ${AVX_LIB})		# until I have a better understanding of cmake this will work!
target_include_directories(${INTERCEPT_PLUGIN_NAME} PUBLIC vendors/glm/glm PUBLIC Dependencies/ND/include/ndNewton)


set_target_properties(${INTERCEPT_PLUGIN_NAME} PROPERTIES PREFIX "")
set_target_properties(${INTERCEPT_PLUGIN_NAME} PROPERTIES FOLDER "${CMAKE_PROJECT_NAME}")

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "-std=c++1z -O2 -s -fPIC -fpermissive -static-libgcc -static-libstdc++")#-march=i686 -m32
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
	set(CMAKE_SHARED_LINKER_FLAGS "-shared -static-libgcc -static-libstdc++")
else()
	set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1 /MP /EHsc")
	set(CMAKE_CXX_FLAGS_RELEASE "/MT /Zi /O2 /Ob1 /EHsc /MP") #with debug info
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/OPT:REF /DEBUG:FULL") 
endif()

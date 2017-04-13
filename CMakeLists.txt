cmake_minimum_required (VERSION 2.8)
set(PROJECT_NAME_STR zmqGenerator)
project(${PROJECT_NAME_STR} C)

if (NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Debug)
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")


set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g")
set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -Wall -ggdb -O0")
set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -g0 -O3")

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color=auto")

add_custom_target(
  extern_lib
  COMMAND make
)

#0mq
find_path(path_include_zmq NAMES zmq.h)
find_library(path_lib_zmq NAMES zmq)
add_library(libzmq SHARED IMPORTED)
set_property(TARGET libzmq PROPERTY IMPORTED_LOCATION ${path_lib_zmq})
message(STATUS "path_include_zmq: " ${path_include_zmq})
message(STATUS "path_lib_zmq: " ${path_lib_zmq} )

#libsodium
find_library(path_lib_sodium NAMES sodium)
add_library(libsodium SHARED IMPORTED)
set_property(TARGET libsodium PROPERTY IMPORTED_LOCATION ${path_lib_sodium})
message(STATUS "path_lib_sodium: " ${path_lib_sodium} )

link_libraries()

#nexus
find_path(path_include_nexus NAMES nexus/napi.h)
find_library(path_lib_nexus NAMES NeXus)

message(STATUS "path_include_nexus: " ${path_include_nexus})
message(STATUS "path_lib_nexus: " ${path_lib_nexus})
add_library(libNeXus SHARED IMPORTED)
set_property(TARGET libNeXus PROPERTY IMPORTED_LOCATION ${path_lib_nexus})

#hdf5
find_path(path_include_hdf5 NAMES hdf5.h)
find_library(path_lib_hdf5 NAMES hdf5)
add_library(libhdf5 SHARED IMPORTED)
set_property(TARGET libhdf5 PROPERTY IMPORTED_LOCATION ${path_lib_hdf5})

message(STATUS "path_include_hdf5: " ${path_include_hdf5})
message(STATUS "path_lib_hdf5: " ${path_lib_hdf5})

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include_directories(
  ${path_include_zmq}
  ${path_include_hdf5}
  ${path_include_nexus}
  ${CMAKE_CURRENT_BINARY_DIR}
)


add_library(zmqGenerator__objects OBJECT
  neventArray.c
  nexus2event.c 
  md5.c
  posix-timers.c
)

target_compile_definitions(zmqGenerator__objects PRIVATE HAS_REMOTE_API=0)

set(tgt "generator")
set(sources
  zmqGenerator.c
  $<TARGET_OBJECTS:zmqGenerator__objects>
)
add_executable(${tgt} ${sources})
target_link_libraries (${tgt} libzmq libsodium libNeXus libhdf5)

set(tgt "receiver")
set(sources
  zmqReader.c
  $<TARGET_OBJECTS:zmqGenerator__objects>
)
add_executable(${tgt} ${sources})
target_link_libraries (${tgt} libzmq libsodium libNeXus libhdf5)


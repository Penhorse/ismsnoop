cmake_minimum_required(VERSION 3.0)

project(library VERSION 1.0.0.0)

include(../SetupProject.cmake)

list(APPEND
	headers_public
	ismsnoop
)

list(APPEND
	src
	ismsnoop
)

foreach(x ${headers_public})
	list(APPEND headers_public_files ${dir_inc}/ismsnoop/${x}.h)
endforeach()

foreach(x ${src})
	list(APPEND src_files ${dir_src}/${x}.cpp)
endforeach()

if(${system} STREQUAL Windows)
	list(APPEND src_files ${dir_src}/exports.def)
endif()

add_library(ismsnoop SHARED ${headers_public_files} ${src_files})

include_directories(${dir_inc})

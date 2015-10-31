cmake_minimum_required(VERSION 3.0)

project(test)

include(../SetupProject.cmake)

set(dir_bin_isms ${dir_bin}/${CMAKE_CFG_INTDIR}/isms)

list(APPEND
	cpp
	main
)

file(GLOB isms
	${CMAKE_CURRENT_SOURCE_DIR}/isms/*.ism
)

foreach(x ${cpp})
	list(APPEND src_files ${dir_src}/${x}.cpp)
endforeach()

foreach(x ${isms})
	get_filename_component(name ${x} NAME)
	list(APPEND isms_output ${CMAKE_CURRENT_BINARY_DIR}/${dir_bin_isms}/${name})
	add_custom_command(
		COMMAND ${CMAKE_COMMAND} -E copy
		ARGS
			${x}
			${CMAKE_CURRENT_BINARY_DIR}/${dir_bin_isms}/${name}
		DEPENDS
			${x}
		OUTPUT
			${CMAKE_CURRENT_BINARY_DIR}/${dir_bin_isms}/${name}
		COMMENT "copying ${name}..."
	)
	set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${dir_bin_isms}/${name} PROPERTIES GENERATED TRUE)
	set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${dir_bin_isms}/${name} PROPERTIES GENERATED TRUE HEADER_FILE_ONLY TRUE)
endforeach()

add_executable(ismsnoop_test ${src_files} ${isms_output})

if(${system} STREQUAL Linux OR ${system} STREQUAL OSX)
	target_link_libraries(ismsnoop_test dl)
endif()

include_directories(../library/include)
include_directories(../submodules/rtw/include)

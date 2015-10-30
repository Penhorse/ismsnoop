list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

set_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS PROJECT_VERSION="${PROJECT_VERSION}")

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(system Linux)
	set(compiler_flags "--std=c++11 -Wall")
	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		set(compiler_flags "${compiler_flags} -ggdb")
		set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS _DEBUG)
	endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	set(system OSX)
	set(compiler_flags "-std=c++0x -stdlib=libc++ -Wall")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(system Windows)
	set(compiler_flags "/EHsc /MP")
endif()

set(CMAKE_CXX_FLAGS "${compiler_flags}")

set(dir_bin bin)
set(dir_lib lib)
set(dir_inc include)
set(dir_src src)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${dir_lib})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${dir_lib})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir_bin})

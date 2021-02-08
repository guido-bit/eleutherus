CMAKE_MINIMUM_REQUIRED (VERSION 3.1 FATAL_ERROR)
PROJECT (eleutherus)

INCLUDE(ExternalProject)
INCLUDE(CheckFunctionExists)

IF(UNIX)
        IF(APPLE)
        ## Xcode build ##
        set(CMAKE_MACOSX_RPATH 1)
        ELSE(APPLE)
        ## Linux build ##
        ENDIF(APPLE)
	## libsodium ##
	ExternalProject_Add(ext_libsodium
		BUILD_IN_SOURCE 1
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libsodium
		INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/ext_libsodium-prefix
		CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/libsodium/./autogen.sh
			COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/libsodium/./configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/ext_libsodium-prefix --disable-pie --with-pthreads --enable-shared=no --enable-static=yes
		BUILD_COMMAND ${MAKE})
	ExternalProject_Get_Property(ext_libsodium install_dir)
	include_directories(${install_dir}/include/)
	add_library(libsodium STATIC IMPORTED)
	set_property(TARGET libsodium PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libsodium.a)

	## libevent ##
        ExternalProject_Add(ext_libevent
                BUILD_IN_SOURCE 1
                SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libevent
                INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/ext_libevent-prefix
		CMAKE_ARGS "-DCMAKE_C_FLAGS=-fPIC" "-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/ext_libevent-prefix" "-DEVENT_INSTALL_CMAKE_DIR=${CMAKE_CURRENT_BINARY_DIR}/ext_libevent-prefix/cmake" "-DEVENT__DISABLE_BENCHMARK=ON" "-DEVENT__DISABLE_DEBUG_MODE=ON" "-DEVENT__DISABLE_OPENSSL=ON" "-DEVENT__DISABLE_SAMPLES=ON" "-DEVENT__DISABLE_TESTS=ON" "-DEVENT__LIBRARY_TYPE=STATIC"
                )
        ExternalProject_Get_Property(ext_libevent install_dir)
        include_directories(${install_dir}/include)
        add_library(libevent STATIC IMPORTED)
	set_property(TARGET libevent PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libevent.a)
	set_property(TARGET libevent PROPERTY INTERFACE_LINK_LIBRARIES ${install_dir}/lib/libevent_core.a ${install_dir}/lib/libevent_pthreads.a ${install_dir}/lib/libevent_extra.a)

ELSE(UNIX)
	IF(WIN32)
		## libsodium ##
		set(ENV{CL} "/MTd")
		ExternalProject_Add(ext_libsodium
			BUILD_IN_SOURCE 1
			SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libsodium
			CONFIGURE_COMMAND ""
			BUILD_COMMAND msbuild /m /v:n /p:DefineConstants=SODIUM_STATIC=1 /p:Configuration=StaticRelease /p:Platform=x64 /maxcpucount ${CMAKE_CURRENT_SOURCE_DIR}/libsodium/builds/msvc/vs2017/libsodium.sln
			INSTALL_DIR ${CMAKE_BINARY_DIR}/ext_libsodium-prefix
			INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory <INSTALL_DIR>/lib/
			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/libsodium/bin/x64/Release/v141/static/libsodium.lib <INSTALL_DIR>/lib
			COMMAND ${CMAKE_COMMAND} -E make_directory <INSTALL_DIR>/include/
			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/libsodium/src/libsodium/include/sodium.h <INSTALL_DIR>/include/
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/libsodium/src/libsodium/include/sodium <INSTALL_DIR>/include/sodium
			COMMAND ${CMAKE_COMMAND} -E remove_directory <INSTALL_DIR>/include/sodium/private
			)
		ExternalProject_Get_Property(ext_libsodium install_dir)
		include_directories(${install_dir}/include/)
		add_library(libsodium STATIC IMPORTED)
		set_property(TARGET libsodium PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libsodium.lib)

		## libevent ##
                ExternalProject_Add(ext_libevent
                        BUILD_IN_SOURCE 1
                        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libevent
                        INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/ext_libevent-prefix
			CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/ext_libevent-prefix" "-DEVENT_INSTALL_CMAKE_DIR=${CMAKE_CURRENT_BINARY_DIR}/ext_libevent-prefix/cmake" "-DEVENT__DISABLE_OPENSSL=ON" "-DEVENT__DISABLE_BENCHMARK=ON" "-DEVENT__DISABLE_DEBUG_MODE=ON" "-DEVENT__DISABLE_OPENSSL=ON" "-DEVENT__DISABLE_SAMPLES=ON" "-DEVENT__DISABLE_TESTS=ON" "-DEVENT__LIBRARY_TYPE=STATIC "
                        )
                ExternalProject_Get_Property(ext_libevent install_dir)
                include_directories(${install_dir}/include)
                add_library(libevent STATIC IMPORTED)
                set_property(TARGET libevent PROPERTY IMPORTED_LOCATION ${install_dir}/lib/event.lib)
				set_property(TARGET libevent PROPERTY INTERFACE_LINK_LIBRARIES ${install_dir}/lib/event_core.lib)
	ELSE(WIN32)
	ENDIF(WIN32)
ENDIF(UNIX)

## TBPGP ##
set(SOURCES
	src/thread_pool.c
	src/log.c
	src/p2p.c
	src/util.c
	src/context.c
	src/protocol.c
	src/id.c
	src/merkle.c
	)
include_directories(include)
add_library(tbpgp SHARED ${SOURCES})
find_package(Threads REQUIRED)
if(MSVC)
	foreach (flag_var 
		CMAKE_C_FLAGS 
		CMAKE_C_FLAGS_DEBUG 
		CMAKE_C_FLAGS_RELEASE 
		CMAKE_C_FLAGS_MINSIZEREL 
		CMAKE_C_FLAGS_RELWITHDEBUGINFO
		)
			if(${flag_var} MATCHES "/MD")
				string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
			endif()
		endforeach()
endif(MSVC)
#check_function_exists(getaddrinfo _HAVE_GETADDRINFO)
add_dependencies(tbpgp ext_libevent)
add_dependencies(tbpgp ext_libsodium)
add_definitions(-DSODIUM_STATIC=1)
target_link_libraries(tbpgp libevent)
target_link_libraries(tbpgp libsodium)
#target_link_libraries(tbpgp event_pthreads)
target_link_libraries(tbpgp ${CMAKE_THREAD_LIBS_INIT})
if(WIN32)
	target_link_libraries(tbpgp ws2_32)
endif(WIN32)
set_property(TARGET tbpgp PROPERTY POSITION_INDEPENDENT_CODE ON)
set_target_properties(tbpgp PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/libs)

## TBPGP_APPS ##
add_subdirectory(apps)

##TBPGP_UTILS
add_subdirectory(utils)

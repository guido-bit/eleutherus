cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(utils C)

add_executable(genesisblk_gen genesisblk_gen.c)
target_link_libraries(genesisblk_gen libsodium)
target_link_libraries(genesisblk_gen tbpgp)
set(CMAKE_VERBOSE_MAKEFILE ON)

#if(WIN32)
#if(CMAKE_BUILD_TYPE MATCHES DEBUG)
#        add_custom_command(
#                TARGET peer_test
#                COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/Debug/tbpgp.dll ${CMAKE_BINARY_DIR}/apps/Debug
#        )
#else(CMAKE_BUILD_TYPE MATCHES RELEASE) #THIS IS A LITTLE TRICKY FIX IT!
#        add_custom_command(
#                TARGET peer_test
#                COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/Release/tbpgp.dll ${CMAKE_BINARY_DIR}/apps/Release
#        )
#endif(CMAKE_BUILD_TYPE MATCHES DEBUG)
#
#
#endif(WIN32)

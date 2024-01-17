cmake_minimum_required(VERSION 3.20)

project (chaat
	LANGUAGES C)

find_package(GnuTLS MODULE REQUIRED)

add_executable(chaat
	src/main.c
	src/sender.c
	src/receiver.c
	)
target_include_directories(chaat PUBLIC incl)
target_include_directories(chaat PUBLIC ${GNUTLS_INCLUDE_DIR})
target_link_libraries(chaat PUBLIC ${GNUTLS_LIBRARIES})

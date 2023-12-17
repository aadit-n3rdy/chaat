cmake_minimum_required(VERSION 3.20)

project (chaat
	LANGUAGES C)

add_executable(chaat
	src/main.c)
target_include_directories(chaat PUBLIC incl)

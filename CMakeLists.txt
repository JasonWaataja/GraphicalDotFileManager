cmake_minimum_required (VERSION 2.9)
project(GraphicalDotFileManager)

if (NOT ${UNIX})
	message (FATAL_ERROR "Can only compile on UNIX platform, exiting.")
endif (NOT ${UNIX})

add_subdirectory (src)

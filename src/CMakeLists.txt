include (CheckIncludeFiles)

set (SOURCES main.cc)
check_include_files (wordexp.h HAVE_WORDEXP_H)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable (gdfm ${SOURCES})
set_property(TARGET gdfm PROPERTY CXX_STANDARD 11)
install (TARGETS gdfm DESTINATION bin)

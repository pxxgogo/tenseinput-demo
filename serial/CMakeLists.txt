
#project name
PROJECT(Qbot)
cmake_minimum_required(VERSION 2.8)

#head file path
INCLUDE_DIRECTORIES(
	./include
)

#source directory
AUX_SOURCE_DIRECTORY(
	src DIR_SRCS
)

#set environment variable
SET(MAIN
	${DIR_SRCS}
)


#add executable file
ADD_EXECUTABLE(main ${MAIN})

find_package(Threads)
target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})


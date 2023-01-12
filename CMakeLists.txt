cmake_minimum_required(VERSION 3.20)
project(ProgettoSOL C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

add_executable(ProgettoSOL Main.c ./libs/Master_Thread.h ./libs/src/Master_Thread.c ./libs/Collector.h ./libs/src/Collector.c ./libs/Thread_Worker.h ./libs/src/Thread_Worker.c ./libs/utilities.h ./libs/src/utilities.c ./libs/mystring.h ./libs/src/mystring.c ./libs/queue.h ./libs/src/queue.c ./libs/ascending_queue.h ./libs/src/ascending_queue.c ./libs/parameters.h)

cmake_minimum_required(VERSION 3.5.0)
find_package(Threads REQUIRED)
include_directories(/usr/local/include/)
link_directories(/usr/local/lib/)

#~~~~~~~~~~~~~~~~

file(GLOB_RECURSE ALL_BENCH_CPP *.cpp)

foreach(ONE_BENCH_CPP ${ALL_BENCH_CPP})

   get_filename_component(ONE_BENCH_EXEC ${ONE_BENCH_CPP} NAME_WE)

   # Avoid name collision 
   set(TARGET_NAME Bench_${ONE_BENCH_EXEC})

   add_executable(${TARGET_NAME} ${ONE_BENCH_CPP})
   set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${ONE_BENCH_EXEC}) 
   target_link_libraries(${TARGET_NAME} 

# If required, you can add your project library here
# ...
         libsemigroups.a
	 benchmark
     ${CMAKE_THREAD_LIBS_INIT})

   # If you want to run benchmarks with the "make test" command, uncomment me
   add_test(${TARGET_NAME} ${ONE_BENCH_EXEC})
endforeach()

if((CMAKE_CXX_COMPILER_ID MATCHES GNU) OR (CMAKE_CXX_COMPILER_ID MATCHES Clang))
   set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -std=c++11")
   set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3")
   set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

cmake_minimum_required(VERSION 3.2)
project(TransortEq)

find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})

#add_compile_options(-Wall -lm)
add_executable(main parallel_sol.c)
if(MPI_COMPILE_FLAGS)
    set_target_properties(main PROPERTIES
            COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

set(POW_LIBS "")
include(CheckLibraryExists)
check_library_exists(m pow "" LIBM)
if(LIBM)
    list(APPEND POW_LIBS "m")
endif()
target_link_libraries(main PUBLIC ${MPI_LIBRARIES} PUBLIC ${POW_LIBS})
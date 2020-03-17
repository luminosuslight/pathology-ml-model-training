PROJECT(tsne_multicore CXX)
CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

IF(MSVC)
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /fp:fast /O2")
ELSE()
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fPIC -ffast-math -funroll-loops")
ENDIF()

FILE(GLOB src *.cpp)

ADD_LIBRARY(tsne_multicore STATIC ${src})

TARGET_INCLUDE_DIRECTORIES(tsne_multicore INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

FIND_PACKAGE(OpenMP 2)

# If OpenMP wasn't found, see if we can find it in the default Homebrew location
if((NOT OpenMP_CXX_FOUND) AND EXISTS "/usr/local/opt/libomp/lib/libomp.dylib")
    set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include")
    set(OpenMP_C_LIB_NAMES omp)
    set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include")
    set(OpenMP_CXX_LIB_NAMES omp)
    set(OpenMP_omp_LIBRARY /usr/local/opt/libomp/lib/libomp.dylib)

    find_package(OpenMP 2)
    if (OpenMP_CXX_FOUND)
        message(STATUS "Found libomp in homebrew default location.")
    else()
        message(FATAL_ERROR "Failed to use OpenMP which exists in the default Homebrew location.")
    endif()
endif()

if(OpenMP_FOUND)
    if(TARGET OpenMP::OpenMP_CXX)
       target_link_libraries(tsne_multicore PRIVATE OpenMP::OpenMP_CXX)
    else()
       target_compile_options(tsne_multicore PRIVATE ${OpenMP_CXX_FLAGS})
       if(OpenMP_CXX_LIB_NAMES)
          target_link_libraries(tsne_multicore PRIVATE ${OpenMP_CXX_LIB_NAMES}) # OpenMP_CXX_LIBRARIES is empty?!
       else()
          target_link_libraries(tsne_multicore PRIVATE ${OpenMP_CXX_FLAGS}) # This is the case for CMake <= 3.8
       endif()
    endif()

ELSE()
    MESSAGE(WARNING "Not using OpenMP. Performance will suffer.")
ENDIF()

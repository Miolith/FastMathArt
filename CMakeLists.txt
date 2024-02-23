cmake_minimum_required(VERSION 3.12)
project(FastMathArt VERSION 0.1.0
                    DESCRIPTION "A fast and simple math art generator"
                    LANGUAGES CXX
)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(GLOB_RECURSE FMA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/fastmathart/*.cpp)
add_library(fma SHARED ${FMA_SRC})

file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
add_executable(test ${FMA_SRC} ${TEST_SOURCES})

# link to fmt library
find_package(fmt CONFIG REQUIRED)
target_link_libraries(fma PRIVATE fmt::fmt-header-only)
target_link_libraries(test PRIVATE fmt::fmt-header-only)

include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
  set_target_properties(fma PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
  set_target_properties(test PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using GCC
    set(CMAKE_VERBOSE_MAKEFILE 1)

    set(GCC_COMPILE_OPTIONS "-m64 -fPIC -Wall -Wextra")
    set(GCC_COMPILE_DEBUG_OPTIONS "${GCC_COMPILE_OPTIONS} -ggdb -O0")
    set(GCC_COMPILE_RELEASE_OPTIONS "${GCC_COMPILE_OPTIONS} -O3")
    target_compile_options(fma PUBLIC "$<$<CONFIG:Debug>:${GCC_COMPILE_DEBUG_OPTIONS}>")
    target_compile_options(fma PUBLIC "$<$<CONFIG:Release>:${GCC_COMPILE_RELEASE_OPTIONS}>")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
    set(MSVC_COMPILE_OPTIONS "/MP;/W3;/w34710;/Gy;/Zc:wchar_t;/nologo; /EHsc")
    set(MSVC_COMPILE_DEBUG_OPTIONS "${MSVC_COMPILE_OPTIONS} /ZI /Od")
    set(MSVC_COMPILE_RELEASE_OPTIONS "${MSVC_COMPILE_OPTIONS} /O2")
    target_compile_options(fma PUBLIC "$<$<CONFIG:Debug>:${MSVC_COMPILE_DEBUG_OPTIONS}>")
    target_compile_options(fma PUBLIC "$<$<CONFIG:Release>:${MSVC_COMPILE_RELEASE_OPTIONS}>")

endif()
cmake_minimum_required(VERSION 3.12)
project(FastMathArt VERSION 0.1.0
                    DESCRIPTION "A fast and simple math art generator"
                    LANGUAGES CXX
)


set(CMAKE_CXX_STANDARD 20) # C++20 required
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Like really required
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)
set(CMAKE_CXX_EXTENSIONS OFF) # disable std=gnuc++ in favor of std=c++
set_property(GLOBAL PROPERTY USE_FOLDERS ON) # Enable folders in IDEs

file(GLOB_RECURSE FMA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/fastmathart/*.cpp)
add_library(fma SHARED ${FMA_SRC})

find_package(fmt CONFIG REQUIRED)

include(CTest)

if (BUILD_TESTING)
  find_package(doctest REQUIRED)
  file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
  add_executable(fma_test ${FMA_SRC} ${TEST_SOURCES})
  add_test(NAME fma_test COMMAND fma_test)
  target_link_libraries(fma_test PRIVATE fmt::fmt)
  target_link_libraries(fma_test PRIVATE doctest::doctest)
endif()
target_link_libraries(fma PRIVATE fmt::fmt)


include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
  set_target_properties(fma PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
  if(BUILD_TESTING)
    set_target_properties(fma_test PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
  endif()
endif()

if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()


if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using GCC or Clang
    set(CMAKE_VERBOSE_MAKEFILE 1)

    set(GCC_COMPILE_OPTIONS "-m64;-fPIC;-Wall;-Wextra;-pipe;-fno-plt;-flto=auto;-ffat-lto-objects")
    set(GCC_COMPILE_DEBUG_OPTIONS "${GCC_COMPILE_OPTIONS};-ggdb;-O0")
    set(GCC_COMPILE_RELEASE_OPTIONS "${GCC_COMPILE_OPTIONS};-O3")
    target_compile_options(fma PUBLIC "$<$<CONFIG:Debug>:${GCC_COMPILE_DEBUG_OPTIONS}>")
    target_compile_options(fma PUBLIC "$<$<CONFIG:Release>:${GCC_COMPILE_RELEASE_OPTIONS}>")

    if (BUILD_TESTING)
      target_compile_options(fma_test PUBLIC "$<$<CONFIG:Debug>:${GCC_COMPILE_DEBUG_OPTIONS}>")
      target_compile_options(fma_test PUBLIC "$<$<CONFIG:Release>:${GCC_COMPILE_RELEASE_OPTIONS}>")
    endif()

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++

    set(MSVC_COMPILE_OPTIONS "/MP;/W3;/w34710;/Gy;/Zc:wchar_t;/nologo; /EHsc")
    set(MSVC_COMPILE_DEBUG_OPTIONS "${MSVC_COMPILE_OPTIONS} /ZI /Od")
    set(MSVC_COMPILE_RELEASE_OPTIONS "${MSVC_COMPILE_OPTIONS} /O2")

    target_compile_options(fma PUBLIC "$<$<CONFIG:Debug>:${MSVC_COMPILE_DEBUG_OPTIONS}>")
    target_compile_options(fma PUBLIC "$<$<CONFIG:Release>:${MSVC_COMPILE_RELEASE_OPTIONS}>")

    if (BUILD_TESTING)
      target_compile_options(fma_test PUBLIC "$<$<CONFIG:Debug>:${MSVC_COMPILE_DEBUG_OPTIONS}>")
      target_compile_options(fma_test PUBLIC "$<$<CONFIG:Release>:${MSVC_COMPILE_RELEASE_OPTIONS}>")
    endif()

endif()
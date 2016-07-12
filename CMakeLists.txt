cmake_minimum_required(VERSION 3.0)
project(PS4EYECam VERSION 0.0.1)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
IF (NOT MSVC)
    set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch")
ENDIF()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Shared architecture label used for install folder locations
if (${CMAKE_C_SIZEOF_DATA_PTR} EQUAL 8)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set(ARCH_LABEL "Win64")
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(ARCH_LABEL "OSX64")
    else()
        set(ARCH_LABEL "Linux64")
    endif()
else()
    if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set(ARCH_LABEL "Win32")
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(ARCH_LABEL "OSX32")
    else()
        set(ARCH_LABEL "Linux32")
    endif()
endif()

# When not using MSVC, we recommend using system-wide libraries
# (installed via homebrew on Mac or apt-get in Linux/Ubuntu)
# In MSVC, we recommend downloading pre-compiled binaries where
# possible, and building Protobuf yourself (see README.md)

IF(MSVC)
#I cannot remember which one, but one of our dependencies
#links against static runtime, so we need our apps to link
#against the static runtime too.
#https://cmake.org/Wiki/CMake_FAQ#How_can_I_build_my_MSVC_application_with_a_static_runtime.3F

    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

    set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
    foreach(CompilerFlag ${CompilerFlags})
      string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
ENDIF(MSVC)

# Step into the subdirectories
add_subdirectory(driver)
add_subdirectory(sdl)
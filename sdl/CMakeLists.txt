set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
set(SDLTEST_INCL_DIRS)
set(SDLTEST_REQ_LIBS)

# platform independent include paths
list(APPEND SDLTEST_INCL_DIRS ${ROOT_DIR}/driver/include/)

# platform independent libraries
list(APPEND SDLTEST_REQ_LIBS PS4EyeCam)

# Requires libusb
find_package(USB1 REQUIRED)
list(APPEND SDLTEST_INCL_DIRS ${LIBUSB_INCLUDE_DIR})
list(APPEND SDLTEST_REQ_LIBS ${LIBUSB_LIBRARIES})
   
# platform specific libraries/include paths (OpenGL/SDL)    
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(OPENGL_FRAMEWORK OpenGL)
    FIND_PACKAGE(SDL2)
    list(APPEND SDLTEST_INCL_DIRS ${SDL2_INCLUDE_DIR})
    list(APPEND SDLTEST_REQ_LIBS
        ${SDL2_LIBRARY} ${OPENGL_FRAMEWORK} ${GLUT_FRAMEWORK})
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    list(APPEND SDLTEST_REQ_LIBS SDL GL)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET(ENV{SDL2DIR} ${ROOT_DIR}/thirdparty/SDL2/)
    FIND_PACKAGE(SDL2)
    list(APPEND SDLTEST_INCL_DIRS ${SDL2_INCLUDE_DIR})
    list(APPEND SDLTEST_REQ_LIBS 
        ${SDL2_LIBRARY}
        imm32.lib
        version.lib
        winmm.lib)
ENDIF()

# Source files that are needed for the SDLTest tool
file(GLOB SDLTEST_SRC
    "${CMAKE_CURRENT_LIST_DIR}/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
)

# SDLTest
add_executable(SDLTest ${SDLTEST_SRC})
target_include_directories(SDLTest PUBLIC ${SDLTEST_INCL_DIRS})
target_link_libraries(SDLTest ${SDLTEST_REQ_LIBS})

# Post build dependencies (assets and external DLLs)
add_custom_command(TARGET SDLTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_LIST_DIR}/resources"
        $<TARGET_FILE_DIR:SDLTest>/resources)

# Install    
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")   
    install(TARGETS SDLTest
        RUNTIME DESTINATION ${ROOT_DIR}/${ARCH_LABEL}/bin
        LIBRARY DESTINATION ${ROOT_DIR}/${ARCH_LABEL}/lib
        ARCHIVE DESTINATION ${ROOT_DIR}/${ARCH_LABEL}/lib)
    install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/resources/ 
            DESTINATION ${ROOT_DIR}/${ARCH_LABEL}/bin/resources
            FILES_MATCHING PATTERN "*.bin")         
ELSE() #Linux/Darwin
ENDIF()

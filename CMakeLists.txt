cmake_minimum_required(VERSION 3.12)
project(neon_night_street C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(${PROJECT_NAME} 
    glad.c 
    src/main.cpp 
    src/Shader.cpp 
    src/Mesh.cpp
    src/Scene.cpp
    src/Camera.cpp
    src/Texture.cpp
    src/Model.cpp
    src/CityBuilder.cpp
    src/AlleyBuilder.cpp
    src/ShopBuilder.cpp
    src/StreetProps.cpp
    src/Audio.cpp
    src/Rain.cpp
)

if(WIN32)
    find_package(glfw3 CONFIG QUIET)
    if(glfw3_FOUND)
        target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
    else()
        find_package(OpenGL REQUIRED)
        find_library(GLFW_LIB NAMES glfw3 glfw PATHS "${CMAKE_SOURCE_DIR}/../Libraries/lib" "${CMAKE_SOURCE_DIR}/lib")
        if(GLFW_LIB)
            target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL ${GLFW_LIB})
        else()
            target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL glfw3)
        endif()
    endif()
elseif(APPLE)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL glfw "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
else()
    find_package(OpenGL REQUIRED)
    find_package(glfw3 QUIET)
    if(glfw3_FOUND)
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL glfw)
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL glfw X11 pthread Xrandr Xi dl)
    endif()
endif()

if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "${CMAKE_BINARY_DIR}/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/shaders"
        "${CMAKE_BINARY_DIR}/shaders"
)

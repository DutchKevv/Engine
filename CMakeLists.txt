cmake_minimum_required(VERSION 3.8)
project(Engine)

message(${CMAKE_SYSTEM_NAME})

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dist/linux)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dist/windows)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dist/mac)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dist/web)
    set(CMAKE_C_COMPILER "/emscripten/emscripten/incoming/emcc")
endif ()

# LIBS
set(ALL_LIBS assimp)

# DEFENITIONS
add_definitions(
        -DTW_STATIC
        -DTW_NO_LIB_PRAGMA
        -DTW_NO_DIRECT3D
        -DGLEW_STATIC
        -D_CRT_SECURE_NO_WARNINGS
)

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(ALL_LIBS ${OPENGL_LIBRARY} glfw GLEW_1130)
    add_definitions(-DGLEW_STATIC)
endif ()

# Compile external dependencies
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/extern ${CMAKE_CURRENT_BINARY_DIR}/extern)

set(SOURCE_FILES
        ${CMAKE_CURRENT_LIST_DIR}/extern/stb_image.cpp
        ${CMAKE_CURRENT_LIST_DIR}/extern/stb_image.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/baseRenderObj.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/baseRenderObj.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/engine.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/engine.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/renderer.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/renderer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/constants.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/camera.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/camera.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/context.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/context.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/shader.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/shader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/logger.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/logger.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/constants.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/text.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/text.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/baseRenderObj.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/baseRenderObj.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/baseDataObj.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/baseDataObj.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/texture.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/texture.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/resourceManager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/resourceManager.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/spriteRenderer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/spriteRenderer.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/model.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/model.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/mesh.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/mesh.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/world.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/world.h)


include_directories(
        ${CMAKE_CURRENT_LIST_DIR}/extern/glm-0.9.7.1/
        ${CMAKE_CURRENT_LIST_DIR}/extern/assimp-3.0.1270/include/
        ${CMAKE_CURRENT_LIST_DIR}/include
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    include_directories(
            /emscripten/
            /emscripten/emscripten/incoming/system/include
    )
else ()
    include_directories(
            ${CMAKE_CURRENT_LIST_DIR}/extern/glew-1.13.0/include/
            ${CMAKE_CURRENT_LIST_DIR}/extern/glfw-3.1.2/include/
            ${CMAKE_CURRENT_LIST_DIR}/extern/bullet-2.81-rev2613/src/
    )
endif ()

add_library(Engine STATIC ${SOURCE_FILES})
set_target_properties(Engine PROPERTIES LINKER_LANGUAGE CXX)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set_target_properties(Engine PROPERTIES LINK_FLAGS "-o dist/engine.js -s USE_GLFW=3 -s TOTAL_MEMORY=167772160 -s USE_FREETYPE=1 -s OUTLINING_LIMIT=2000 -s DISABLE_EXCEPTION_CATCHING=0 -s NO_EXIT_RUNTIME=1 -s DEMANGLE_SUPPORT=1 -s SAFE_HEAP=1 --bind -s WASM=1 -O2 -s LEGACY_GL_EMULATION=0  -s GL_UNSAFE_OPTS=0 --pre-js pre-module.js --post-js post-module.js -s ASSERTIONS=1 -s GL_ASSERTIONS=1 -s INVOKE_RUN=0  -std=c++11 -s USE_WEBGL2=1 -s FULL_ES3=1 -s GL_DEBUG=1 -s OFFSCREENCANVAS_SUPPORT=0 --use-preload-plugins --preload-file assets --preload-file assets")
else ()
    target_link_libraries(Engine ${ALL_LIBS})
endif ()

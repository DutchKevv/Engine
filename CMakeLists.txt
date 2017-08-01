cmake_minimum_required(VERSION 3.8)
project(Engine)


set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dist)
#set(CMAKE_BUILD_TYPE_INIT "Release")
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

message(${CMAKE_SYSTEM_NAME})
if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_C_COMPILER "emcc")
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(SOURCE_FILES
#        ${CMAKE_CURRENT_LIST_DIR}/library.cpp
        ${CMAKE_CURRENT_LIST_DIR}/extern/stb_image.cpp
        ${CMAKE_CURRENT_LIST_DIR}/extern/stb_image.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/baseRenderObj.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/baseRenderObj.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/engine.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/renderer.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/renderer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/constants.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/camera.h
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/camera.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/context.h
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
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/resourceManager.h include/engine/spriteRenderer.cpp include/engine/spriteRenderer.h)

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    find_package(OPENGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(GLFW3 REQUIRED)
    find_package(assimp REQUIRED)
    find_package(freetype REQUIRED)
    find_package(glm REQUIRED)
endif ()

include_directories(
#        /usr/local/include/
        /usr/local/Cellar/freetype/2.8/include/freetype2
#        ${GLEW_INCLUDE_DIRS}
        ${GLFW_INCLUDE_DIRS}

        ${GLM_INCLUDE_DIRS}
        ${FREETYPE_INCLUDE_DIRS}
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    include_directories(/emscripten/ /emscripten/emscripten/incoming/system/include)
else()
    include_directories()
endif ()

add_library(Engine ${SOURCE_FILES})
set_target_properties(Engine PROPERTIES LINKER_LANGUAGE CXX)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-o dist/engine.js -s USE_GLFW=3 -s TOTAL_MEMORY=167772160 -s USE_FREETYPE=1 -s OUTLINING_LIMIT=2000 -s DISABLE_EXCEPTION_CATCHING=0 -s NO_EXIT_RUNTIME=1 -s DEMANGLE_SUPPORT=1 -s SAFE_HEAP=1 --bind -s WASM=1 -O2 -s LEGACY_GL_EMULATION=0  -s GL_UNSAFE_OPTS=0 --pre-js pre-module.js --post-js post-module.js -s ASSERTIONS=1 -s GL_ASSERTIONS=1 -s INVOKE_RUN=0  -std=c++11 -s USE_WEBGL2=1 -s FULL_ES3=1 -s GL_DEBUG=1 -s OFFSCREENCANVAS_SUPPORT=0 --use-preload-plugins --preload-file assets --preload-file assets")
else ()
    target_link_libraries(${PROJECT_NAME} assimp ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY} glfw freetype ${GLM_LIBRARY})
endif ()

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

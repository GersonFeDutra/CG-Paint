cmake_minimum_required(VERSION 3.16)
project(MyOpenGLApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Coleta automática de todos os arquivos .cpp dentro da pasta src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(MyOpenGLApp ${SOURCES})
target_include_directories(MyOpenGLApp PRIVATE ${PROJECT_SOURCE_DIR}/src)

if(MSVC)
    # Visual Studio (MSVC)
    find_package(OpenGL REQUIRED)
    find_path(FREEGLUT_INCLUDE_DIR GL/freeglut.h)
    find_library(FREEGLUT_LIBRARY NAMES freeglut)

    if (NOT FREEGLUT_INCLUDE_DIR OR NOT FREEGLUT_LIBRARY)
        message(FATAL_ERROR "FreeGLUT não encontrado no MSVC. Instale via vcpkg ou configure os paths manualmente.")
    endif()

    target_include_directories(MyOpenGLApp PRIVATE ${FREEGLUT_INCLUDE_DIR})
    target_link_libraries(MyOpenGLApp
        ${FREEGLUT_LIBRARY}
        OpenGL::GL
        OpenGL::GLU
        opengl32.lib
        glu32.lib
        user32.lib
        gdi32.lib
    )

elseif(WIN32)
    # Windows (MinGW/MSYS2)
    find_package(OpenGL REQUIRED)
    find_path(FREEGLUT_INCLUDE_DIR GL/freeglut.h)
    find_library(FREEGLUT_LIBRARY NAMES freeglut)

    if (NOT FREEGLUT_INCLUDE_DIR OR NOT FREEGLUT_LIBRARY)
        message(FATAL_ERROR "FreeGLUT não encontrado. Instale pelo MSYS2: pacman -S mingw-w64-x86_64-freeglut")
    endif()

    target_include_directories(MyOpenGLApp PRIVATE ${FREEGLUT_INCLUDE_DIR})
    target_link_libraries(MyOpenGLApp
        ${FREEGLUT_LIBRARY}
        OpenGL::GL
        OpenGL::GLU
        -lgdi32 -luser32
    )

else()
    # Linux / WSL
    find_package(OpenGL REQUIRED)
    find_path(FREEGLUT_INCLUDE_DIR GL/freeglut.h)
    find_library(FREEGLUT_LIBRARY NAMES glut freeglut)

    if (NOT FREEGLUT_INCLUDE_DIR OR NOT FREEGLUT_LIBRARY)
        message(FATAL_ERROR "FreeGLUT não encontrado. Instale com: sudo apt install freeglut3-dev")
    endif()

    target_include_directories(MyOpenGLApp PRIVATE ${FREEGLUT_INCLUDE_DIR})
    target_link_libraries(MyOpenGLApp
        ${FREEGLUT_LIBRARY}
        OpenGL::GL
        OpenGL::GLU
    )
endif()

cmake_minimum_required(VERSION 3.16)
project(MyOpenGLApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Coleta automática de todos os arquivos .cpp dentro da pasta src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(MyOpenGLApp ${SOURCES})
target_include_directories(MyOpenGLApp PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/vendor # Third Party Libraries
)

if(MSVC)
    # Configure vcpkg toolchain if not already set
    if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    endif()

    # Visual Studio (MSVC)
    find_package(OpenGL REQUIRED)
    find_package(FreeGLUT REQUIRED)

    target_link_libraries(MyOpenGLApp
        PRIVATE
        FreeGLUT::freeglut
        OpenGL::GL
        OpenGL::GLU
        user32
        gdi32
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

@echo off
setlocal ENABLEEXTENSIONS

set "OUT=MyOpenGLApp.exe"
set "SRC_DIR=src"

:: Verifica se o cmake está disponível
where cmake >nul 2>&1
if %ERRORLEVEL%==0 (
    echo [INFO] CMake encontrado. Iniciando build via CMake...

    if not exist build (
        mkdir build
    )
    pushd "%~dp0\.."
    cmake .. -G "MinGW Makefiles"
    cmake --build .
    popd
    goto :EOF
)

:: Verifica se g++ está disponível (MinGW)
where g++ >nul 2>&1
if %ERRORLEVEL%==0 (
    echo [INFO] CMake não encontrado. Usando MinGW/g++ diretamente...

    echo [INFO] Coletando arquivos .cpp na pasta "%SRC_DIR%"...
    set "FILES="
    for /R "%SRC_DIR%" %%f in (*.cpp) do (
        set "FILES=!FILES! %%f"
    )

    :: Habilita delayed expansion para uso da variável FILES dentro do loop
    setlocal EnableDelayedExpansion
    echo [INFO] Compilando arquivos:
    echo !FILES!
    
    if not exist build (
        mkdir build
    )
    g++ !FILES! -o build\%OUT% -lglu32 -lopengl32 -lfreeglut -lgdi32 -luser32

    if %ERRORLEVEL%==0 (
        echo [SUCESSO] Compilado com sucesso: %OUT%
    ) else (
        echo [ERRO] Falha na compilação com g++
    )
    goto :EOF
)

:: Nenhum compilador encontrado
echo [ERRO] Nenhum compilador encontrado.
echo Instale o CMake ou o MinGW com g++ para compilar este projeto.
exit /b 1

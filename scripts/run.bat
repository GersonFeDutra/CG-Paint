@echo off
setlocal ENABLEEXTENSIONS

set "APP=build\MyOpenGLApp.exe"

:: Verifica se o executável existe
if exist "%APP%" (
    echo [INFO] Executando %APP%...
    "%APP%"
) else (
    echo [ERRO] O executável %APP% não foi encontrado.
    echo Compile o projeto primeiro executando: build.bat
    exit /b 1
)

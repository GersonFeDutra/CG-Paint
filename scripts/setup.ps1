# setup.ps1
$ErrorActionPreference = "Stop"

Write-Host "🚀 Iniciando configuração do ambiente..."

# Caminho padrão do vcpkg
$VCPKG_PATH = "$PSScriptRoot\vcpkg"

# 1. Clonar vcpkg se ainda não existe
if (!(Test-Path $VCPKG_PATH)) {
    Write-Host "📥 Clonando vcpkg..."
    git clone https://github.com/microsoft/vcpkg.git $VCPKG_PATH
    & "$VCPKG_PATH\bootstrap-vcpkg.bat"
}

# 2. Instalar dependências
Write-Host "📦 Instalando bibliotecas necessárias..."
& "$VCPKG_PATH\vcpkg.exe" install freeglut:x64-windows

# 3. Integrar com Visual Studio (uma vez por máquina)
Write-Host "🔗 Integrando vcpkg ao Visual Studio..."
& "$VCPKG_PATH\vcpkg.exe" integrate install

# 4. Criar pasta de build
$buildPath = "$PSScriptRoot\out\build\x64-Debug"
New-Item -ItemType Directory -Force -Path $buildPath | Out-Null

# 5. Rodar CMake com toolchain
Write-Host "🛠️ Gerando arquivos CMake..."
& cmake `
    -B $buildPath `
    -DCMAKE_TOOLCHAIN_FILE="$VCPKG_PATH\scripts\buildsystems\vcpkg.cmake" `
    -DCMAKE_BUILD_TYPE=Debug `
    -G "Ninja Multi-Config" `
    .

Write-Host "`n✅ Ambiente pronto! Abra o projeto no Visual Studio ou compile com CMake diretamente."

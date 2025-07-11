# entra na raiz do projeto
Set-Location -Path (Split-Path $MyInvocation.MyCommand.Path)/..

$ErrorActionPreference = 'Stop'

$srcDir = "src"
$outExe = "MyOpenGLApp.exe"
$outPath = "build/$outExe"

# Verifica se o cmake está disponível
if (Get-Command cmake -ErrorAction SilentlyContinue) {
    Write-Host "[INFO] CMake encontrado. Iniciando build via CMake..."

    if (-not (Test-Path "build")) {
        New-Item -ItemType Directory -Path "build" | Out-Null
    }

    Push-Location build
    cmake .. -G "MinGW Makefiles"
    cmake --build .
    Pop-Location
    return
}

# Verifica se g++ está disponível
if (Get-Command g++ -ErrorAction SilentlyContinue) {
    Write-Host "[INFO] CMake não encontrado. Usando MinGW/g++ diretamente..."

    # Coleta todos os .cpp em src/
    $cppFiles = Get-ChildItem -Recurse -Path $srcDir -Filter *.cpp | ForEach-Object { $_.FullName }

    if ($cppFiles.Count -eq 0) {
        Write-Host "[ERRO] Nenhum arquivo .cpp encontrado na pasta src/"
        exit 1
    }

    Write-Host "[INFO] Compilando arquivos:"
    $cppFiles | ForEach-Object { Write-Host " - $_" }

    # $args = $cppFiles + @("-o", $outExe, "-lglu32", "-lopengl32", "-lfreeglut", "-lgdi32", "-luser32")

    & g++ $cppFiles -o $outPath - lglu32 -lopengl32 -lfreeglut -lgdi32 -luser32

    if ($LASTEXITCODE -eq 0) {
        Write-Host "`n[SUCESSO] Compilado com sucesso: $outExe"
    } else {
        Write-Host "`n[ERRO] Falha na compilação com g++"
        exit $LASTEXITCODE
    }

    return
}

# Nenhum compilador encontrado
Write-Host "[ERRO] Nenhum compilador encontrado."
Write-Host "Instale o CMake ou o MinGW com g++ para compilar este projeto."
exit 1

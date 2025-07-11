#!/bin/bash
set -e

# garante que estamos na raiz do projeto
cd "$( dirname "${BASH_SOURCE[0]}" )"/..

SRC_DIR="src"
OUT="build/MyOpenGLApp"

# Verifica se o cmake está disponível
if command -v cmake &> /dev/null; then
    echo "[INFO] CMake encontrado. Iniciando build via CMake..."

    mkdir -p build
    cd build
    cmake .. -G "Unix Makefiles"
    make
    exit 0
fi

# Verifica se g++ está disponível
if command -v g++ &> /dev/null; then
    echo "[INFO] CMake não encontrado. Usando g++ diretamente..."

    echo "[INFO] Buscando arquivos .cpp na pasta '$SRC_DIR'..."
    SOURCES=$(find "$SRC_DIR" -name '*.cpp')

    if [ -z "$SOURCES" ]; then
        echo "[ERRO] Nenhum arquivo .cpp encontrado em $SRC_DIR"
        exit 1
    fi

    echo "[INFO] Compilando arquivos:"
    echo "$SOURCES"

    g++ $SOURCES -o "$OUT" -lGL -lGLU -lglut

    if [ $? -eq 0 ]; then
        echo "[SUCESSO] Compilado com sucesso: $OUT"
    else
        echo "[ERRO] Falha na compilação com g++"
        exit 1
    fi

    exit 0
fi

# Nenhum compilador encontrado
echo "[ERRO] Nenhum compilador encontrado."
echo "Instale o CMake ou o g++ para compilar este projeto."
exit 1

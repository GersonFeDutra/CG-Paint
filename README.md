# Trabalho de Avaliação (Computação Gráfica [CC0029]) - Paint em OpenGL

## Instalação

Depende do seu sistema operacional. Siga as instruções dadas pela professora: MingW e GLUT no Windows, e G++ com GLU no Linux.
Também recomendo a instalação do CMake para automatizar a build nos seus sistemas.

> [!NOTE]
> Se você possui o Python instalado na sua máquina use o script `build.py`
> para executar o script de instalação correto. Você pode pular a explicação da seção [Build](#build) abaixo.

> [!TIP]
> Se você está usando o VsCode, você pode usar as tasks configuradas no repositório para build e execução. - **Necessário ter o Python e demais requisitos instalados.**

----

> [!IMPORTANT]
> Os scripts referenciados abaixo se encontra na pasta [scripts/](./scripts/). Os comandos devem ser executados na pasta raiz do projeto (onde se encontra `README.md` e `build.py`).
> 

----

## Build

### Windows

Execute `build.bat` ou compile com **MingW** com as flags necessárias. Você também pode usar o script CMake para compilar o seu projeto no Visual Studio com sua própria _solution_.

### Linux

Execute `build.sh`:
```bash
chmod +x ./build.sh
./build.sh
```
que irá invocar o CMake para gerenciar a build, ou compile com G++ (ou outro compilador de sua escolha) com as flags necessárias.

> [!TIP]
> No VSCode: Abra a paleta de comandos `Ctrl+Shift+P` > `Run Task` > `Build (Python Wrapper)`

----

## Execução

### Windows

Execute `run.bat` ou execute o programa .exe compilado diretamente - deve estar em `build/MyOpenGLApp.exe`.

### Linux

Execute `run.sh`:
```bash
chmod +x ./run.sh
./run.sh
```
ou inicie o executável compilado diretamente - deve estar em `build/MyOpenGLApp.exe`.

> [!TIP]
> No VSCode: Abra a paleta de comandos `Ctrl+Shift+P` > `Run Task` > `Run App (Python Wrapper)`

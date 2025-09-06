# Trabalho de Avaliação (Computação Gráfica [CC0029]) - Paint em OpenGL

## Designação [Objetivos]

### 1. Desenvolva uma aplicação em OpenGL usando a linguagem C que permite o usuário a criação de objetos primários, seleção de objetos e transformações geométricas (6,0 pontos). 

- [x] Objetos primários (usuário define quais objetos e quantas vezes deseja criar):
    - [x] Ponto
    - [x] Segmento de Reta
    - [x] Polígono
- [x] Seleção de objetos para poder realizar as transformações geométricas e 
exclusões:
    - [x] Selecionar objetos primários individualmente (ponto, segmento de reta e polígono)
    - [x] Excluir objetos selecionados
- [x] Transformações geométricas (crie a matriz de transformação e matriz composta, quando necessário):
    - [x] Arrastar e soltar (transladar)
    - [x] Rotacionar em relação ao centro do objeto
        - [x] No ponto é em relação a origem do eixo de coordenadas (local)
    - [x] Escalar em relação ao centro do objeto
        - [x] ~No ponto não existe~ No ponto a escala é feita em relação à origem do eixo de coordenadas (local)
    - [x] Reflexão
    - [x] Cisalhamento

> [!NOTE]
> Obs: para a criação de objetos, seleção de objetos e transformações
> geométricas podem ser usados os mais diversos eventos como:
> - Clique do mouse;
> - Botão de rolagem do mouse;
> - Teclado do computador.

> [!TIP]
> Sugestão: use estruturas de dados para construção e manipulação dos objetos
> primários.

### 2. Além das transformações geométricas, implemente também funcionalidades que possam ... (2,0 pontos):

- [x] Salvar os objetos geométricos criados em um arquivo
- [x] Carregar os objetos geométricos de um arquivo.

### 3. Implemente uma funcionalidade para fazer animação com os objetos primários.

- [ ] Use a criatividade!

### Definição das [apresentações](https://docs.google.com/presentation/d/1XVjmBlJticL7nBKuWkazYf_grtQmo3ByLoaVnSsXd0U/edit?usp=sharing)

- [ ] Cada grupo deverá preparar e realizar uma apresentação detalhada sobre o projeto desenvolvido, abordando os seguintes aspectos:
    - [x] Explique a estrutura de dados utilizada para criar e manipular os objetos geométricos e por que foi escolhida
    - [ ] Mostre o funcionamento do projeto, incluindo criação, seleção, transformação dos objetos, etc
    - [x] Apresentação das implementações dos algoritmos de seleção de objetos

> [!NOTE]
> **Tempo de apresentação:** 20-30min <br>
> **Entregar:** Código fonte + Apresentação

---

## Instalação

Depende do seu sistema operacional. Siga as instruções dadas pela professora: MingW e GLUT (FreeGLUT) no Windows, e G++ com GLU no Linux.
Também recomendo a instalação do CMake para automatizar a build nos seus sistemas.

> [!TIP]
> Se você está usando o VsCode, você pode usar as tasks configuradas no repositório para build e execução. - **Necessário ter o Python e demais requisitos instalados.**

### Visual Studio

Na raiz do projeto execute o script abaixo no powershell:
```powershell
.\scripts\setup.ps1
```
Ele fará a instalação e vínculo das bibliotecas na sua IDE usando [vcpkg](https://vcpkg.io/). Após a configuração, abra a janela do Visual Studio e escolha `File` > `Open` > `CMake` e selecione o `CMakeLists.txt` do projeto.

> [!WARNING]
> Os arquivos de configuração do Visual Studio **não devem ser incluídos** no repositório remoto! Crie um branch do projeto se achar necessário!

----

> [!IMPORTANT]
> Os scripts referenciados abaixo se encontra na pasta [scripts/](./scripts/). Os comandos devem ser executados na pasta raiz do projeto (onde se encontra `README.md` e `build.py`).

----

## Build

> [!NOTE]
> Se você possui o Python instalado na sua máquina use o script `build.py`
> para executar o script de compilação correto. Você pode pular a explicação das seções abaixo. - **CMake necessário**.


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

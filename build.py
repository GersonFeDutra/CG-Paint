#!/usr/bin/env python
import os
import platform
import subprocess
import shutil
import sys

# Ativa suporte a ANSI no Windows (somente se necessário)
if os.name == "nt":
    import ctypes
    kernel32 = ctypes.windll.kernel32
    kernel32.SetConsoleMode(kernel32.GetStdHandle(-11), 7)


# Definição de cores ANSI
class Color:
    RESET = "\033[0m"
    RED   = "\033[31m"
    GREEN = "\033[32m"
    YELLOW= "\033[33m"
    BLUE  = "\033[34m"
    MAGENTA = "\033[35m"
    CYAN  = "\033[36m"
    BOLD  = "\033[1m"


def is_wsl():
    return "microsoft" in platform.uname().release.lower()


def print_info(message, file=sys.stderr, *args, **kwargs):
    """
    Imprime uma mensagem de informação formatada com cores ANSI.

    Parameters
    ----------
    message : str
        A mensagem de informação.
    file : file-like, optional
        O arquivo para onde a mensagem será impressa (padrão: sys.stderr).
    *args
        Argumentos adicionais para a função print.
    **kwargs
        Palavras-chave adicionais para a função print.
    """
    print(f"{Color.BLUE}{Color.BOLD}[INFO] {message}{Color.RESET}", file=file, *args, **kwargs)


def print_error(message, file=sys.stderr, *args, **kwargs):
    """
    Imprime uma mensagem de erro formatada com cores ANSI.

    Parameters
    ----------
    message : str
        A mensagem de erro.
    file : file-like, optional
        O arquivo para onde a mensagem será impressa (padrão: sys.stderr).
    *args
        Argumentos adicionais para a função print.
    **kwargs
        Palavras-chave adicionais para a função print.
    """
    print(f"{Color.RED}{Color.BOLD}[ERRO] {message}{Color.RESET}", file=file, *args, **kwargs)


def run_script(command, shell=False):
    try:
        # Mostrar o comando de forma legível
        if isinstance(command, (list, tuple)):
            print_info(f"Executando: {' '.join(map(str, command))}")
        else:
            print_info(f"Executando: {command}")
        subprocess.run(command, shell=shell, check=True)
    except subprocess.CalledProcessError as e:
        print_error(f"Falha ao executar: {command}")
        sys.exit(e.returncode)


def make_executable_if_needed(filepath):
    if not os.access(filepath, os.X_OK):
        print_info(f"Aplicando chmod +x em {filepath}")
        os.chmod(filepath, 0o755)


def run_executable():
    system = platform.system().lower()
    exe = ""

    if system == "windows":
        exe = os.path.join("build", "MyOpenGLApp.exe")
    elif system in ["linux", "darwin"] or is_wsl():
        exe = os.path.join("build", "MyOpenGLApp")
    else:
        print_error("Execução não suportada neste sistema.")
        sys.exit(1)

    if not os.path.exists(exe):
        print_error(f"Executável não encontrado: {exe}")
        sys.exit(1)

    print_info(f"Executando programa: {exe}")
    subprocess.run([exe])

def main():
    # flags
    should_run = "--run" in sys.argv
    debug_flag = "--debug" in sys.argv

    # you may keep the flags in argv, but we only inspect them above

    system = platform.system().lower()

    if system == "windows":
        if shutil.which("powershell"):
            script = os.path.join("scripts", "build.ps1")
            # Passa --debug adiante se solicitado
            command = ["powershell", "-ExecutionPolicy", "Bypass", "-File", script]
            if debug_flag:
                command.append("--debug")
            run_script(command, shell=True)
        else:
            # .bat ou .cmd
            script = os.path.join("scripts", "build.bat")
            # Para .bat, vamos passar uma string para shell=True
            cmd_str = f'"{script}"'
            if debug_flag:
                cmd_str += " --debug"
            run_script(cmd_str, shell=True)

    elif system in ["linux", "darwin"] or is_wsl():
        script = os.path.join("scripts", "build.sh")
        make_executable_if_needed(script)
        # Passa --debug adiante se solicitado
        command = ["bash", script]
        if debug_flag:
            command.append("--debug")
        run_script(command)

    else:
        print_error(f"Sistema operacional '{system}' não suportado.")
        sys.exit(1)

    if should_run:
        run_executable()


if __name__ == "__main__":
    main()

#!/usr/bin/env python
import os
import platform
import subprocess
import shutil
import sys

def is_wsl():
    return "microsoft" in platform.uname().release.lower()

def run_script(command, shell=False):
    try:
        print(f"[INFO] Executando: {command}")
        subprocess.run(command, shell=shell, check=True)
    except subprocess.CalledProcessError as e:
        print(f"[ERRO] Falha ao executar: {command}")
        sys.exit(e.returncode)

def make_executable_if_needed(filepath):
    if not os.access(filepath, os.X_OK):
        print(f"[INFO] Aplicando chmod +x em {filepath}")
        os.chmod(filepath, 0o755)

def run_executable():
    system = platform.system().lower()
    exe = ""

    if system == "windows":
        exe = os.path.join("build", "MyOpenGLApp.exe")
    elif system in ["linux", "darwin"] or is_wsl():
        exe = os.path.join("build", "MyOpenGLApp")
    else:
        print(f"[ERRO] Execução não suportada neste sistema.")
        sys.exit(1)

    if not os.path.exists(exe):
        print(f"[ERRO] Executável não encontrado: {exe}")
        sys.exit(1)

    print(f"[INFO] Executando programa: {exe}")
    subprocess.run([exe])


def main():
    should_run = "--run" in sys.argv

    system = platform.system().lower()

    if system == "windows":
        if shutil.which("powershell"):
            script = os.path.join("scripts", "build.ps1")
            command = ["powershell", "-ExecutionPolicy", "Bypass", "-File", script]
        else:
            script = os.path.join("scripts", "build.bat")
            command = [script]
        run_script(command, shell=True)

    elif system in ["linux", "darwin"] or is_wsl():
        script = os.path.join("scripts", "build.sh")
        make_executable_if_needed(script)
        run_script(["bash", script])

    else:
        print(f"[ERRO] Sistema operacional '{system}' não suportado.")
        sys.exit(1)

    if should_run:
        run_executable()


if __name__ == "__main__":
    main()

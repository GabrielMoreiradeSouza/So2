@echo off
REM Build script para Windows (requer MinGW ou WSL)

echo ============================================
echo   SO-G - Sistema Operacional Simulado
echo   Script de Build para Windows
echo ============================================
echo.

REM Verifica se gcc existe
where gcc >nul 2>nul
if %errorlevel% neq 0 (
    echo ERRO: GCC nao encontrado!
    echo.
    echo Por favor, instale uma das opcoes:
    echo   1. MinGW: https://www.mingw-w64.org/
    echo   2. WSL Ubuntu: wsl --install
    echo.
    echo Ou execute este projeto dentro do WSL com 'make'
    pause
    exit /b 1
)

echo Criando diretorios...
if not exist bin mkdir bin

echo.
echo Compilando componentes...

echo   - Compilando meu_so...
gcc -Wall -Wextra -Iinclude -o bin\meu_so.exe src\meu_so.c
if %errorlevel% neq 0 goto :error

echo   - Compilando login...
gcc -Wall -Wextra -Iinclude -pthread -o bin\login.exe src\login.c src\fs_driver.c -lrt
if %errorlevel% neq 0 goto :error

echo   - Compilando shell...
gcc -Wall -Wextra -Iinclude -o bin\shell.exe src\shell.c
if %errorlevel% neq 0 goto :error

echo   - Compilando formatar...
gcc -Wall -Wextra -Iinclude -o bin\formatar.exe src\formatar.c src\fs_driver.c
if %errorlevel% neq 0 goto :error

echo   - Compilando ls...
gcc -Wall -Wextra -Iinclude -o bin\ls.exe src\ls_so.c src\fs_driver.c
if %errorlevel% neq 0 goto :error

echo   - Compilando cat...
gcc -Wall -Wextra -Iinclude -o bin\cat.exe src\cat_so.c src\fs_driver.c
if %errorlevel% neq 0 goto :error

echo   - Compilando chat...
gcc -Wall -Wextra -Iinclude -pthread -o bin\chat.exe src\chat.c -lrt
if %errorlevel% neq 0 goto :error

echo.
echo ============================================
echo   Compilacao concluida com sucesso!
echo ============================================
echo.
echo Proximos passos:
echo   1. bin\formatar.exe  - Formata o disco virtual
echo   2. bin\meu_so.exe    - Inicia o sistema
echo.
echo NOTA: Este projeto foi desenvolvido para Linux/POSIX.
echo      Para melhor experiencia, use WSL (Windows Subsystem for Linux)
echo      e execute com 'make' ao inves deste script.
echo.
pause
exit /b 0

:error
echo.
echo ============================================
echo   ERRO na compilacao!
echo ============================================
echo.
echo O projeto usa recursos POSIX (fork, shared memory, semaphores)
echo que nao estao disponiveis nativamente no Windows.
echo.
echo SOLUCAO RECOMENDADA:
echo   1. Instale o WSL: wsl --install
echo   2. Abra o Ubuntu WSL
echo   3. Navegue ate este diretorio
echo   4. Execute: make
echo.
pause
exit /b 1

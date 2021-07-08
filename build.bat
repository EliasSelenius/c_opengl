
@echo off

echo Build start...

SETLOCAL EnableDelayedExpansion 

SET cFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
)

clang %cFilenames% -g -o bin/program.exe -Iinclude -luser32.lib -lgdi32.lib -lshell32.lib -lkernel32.lib -lmsvcrt.lib -llibcmt.lib -lbin/glfw3.lib -lbin/glfw3dll.lib

echo %cFilenames%
echo Build done.
@echo off
g++ demo.cpp -o demo.exe -IC:/raylib/raylib/src -LC:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm

if %errorlevel% neq 0 (
    echo Compilation failed!
    pause
    exit /b
)

demo.exe
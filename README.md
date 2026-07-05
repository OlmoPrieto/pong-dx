# pong-dx

A simple C++ project using GCC and VS Code.

## Requirements

- GCC/G++ compiler (installed via MSYS2)
- VS Code with C/C++ extension

## Building

Using VS Code task (Ctrl+Shift+B):
- Compiles with g++ to generate pong-dx.exe

Or from command line:
```bash
g++ src/main.cpp -o pong-dx.exe -std=c++17 -I include
```

## Running

From VS Code:
- Press Ctrl+Shift+D to run the "Run C++ Program" task

Or from command line:
```bash
.\pong-dx.exe
```

## Project Structure

- `src/` - Source files
- `include/` - Header files
- `.vscode/` - VS Code configuration
  - `tasks.json` - Build and run tasks
  - `c_cpp_properties.json` - IntelliSense configuration
  - `settings.json` - Editor settings (2-space indentation)

# SO-TrabalhoFinal

Operating Systems - Final Project

## Project Structure

```
SO-TrabalhoFinal/
├── src/           # Source files (.c)
├── include/       # Header files (.h)
├── bin/           # Compiled executables (generated)
├── obj/           # Object files (generated)
├── lib/           # External libraries (if needed)
├── Makefile       # Build configuration
└── README.md      # This file
```

## Building the Project

To compile the project, simply run:

```bash
make
```

This will:
- Create the necessary directories (`obj/` and `bin/`)
- Compile all `.c` files in the `src/` directory
- Link the object files into an executable in `bin/program`

## Running the Project

To run the compiled program:

```bash
make run
```

Or directly:

```bash
./bin/program
```

## Makefile Targets

- `make all` or `make` - Build the project (default)
- `make run` - Build and run the program
- `make clean` - Remove all build artifacts (obj/ and bin/)
- `make clean-obj` - Remove only object files
- `make rebuild` - Clean and rebuild everything
- `make help` - Show available targets
- `make debug` - Print Makefile variables for debugging

## Adding New Files

To add a new C file to the project:

1. Create your `.c` file in the `src/` directory
2. If you need a header file, create it in the `include/` directory
3. The Makefile will automatically detect and compile new files

## Compiler Settings

- **Compiler**: GCC
- **Flags**: `-Wall -Wextra -g`
  - `-Wall`: Enable all warnings
  - `-Wextra`: Enable extra warnings
  - `-g`: Include debug information
- **Include Path**: `-I./include` (automatically includes headers from `include/`)

## Example Module

The project includes an example module (`utils`) demonstrating how to structure modular code:

- `include/utils.h` - Header file with function declarations
- `src/utils.c` - Implementation file
- `src/main.c` - Main program that uses the utils module

## Requirements

- GCC compiler
- GNU Make
- Linux/Unix environment (or WSL on Windows)
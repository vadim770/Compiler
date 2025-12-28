# C Assembler

This is a two-pass assembler for a hypothetical assembly language, written in C (ANSI standard). The program takes assembly source files as input, processes them, and generates machine code files if the source is valid.

## Features

*   **Pre-assembler stage:** Supports macro definition and expansion.
*   **Two-Pass design:**
    *   **First Pass:** Scans the source code, builds a symbol table, and performs initial syntax validation.
    *   **Second Pass:** Translates assembly instructions into machine code, resolves symbol addresses, and creates output files.
*   **Error Detection:** The assembler checks for various syntax and logical errors in the source files. If errors are found, the output files are not generated.
*   **Output Generation:** Upon successful assembly, the program generates:
    *   An object file (`.ob`) containing the machine code.
    *   An externals file (`.ext`) listing symbols used but defined in other modules.
    *   An entries file (`.ent`) listing symbols defined in this module that can be used by other modules.

## How to Build

A `makefile` is provided for easy compilation on a system with `gcc` and `make`.

1.  **To build the assembler:**
    Open a terminal in the project root directory and run:
    ```sh
    make
    ```
    This will compile the source files and create an executable named `assembler`.

2.  **To clean up build files:**
    To remove the executable and all intermediate object files (`.o`), run:
    ```sh
    make clean
    ```

## Usage

To assemble one or more files, run the compiled program from the terminal, followed by the names of the source files. The source files are expected to have a `.as` extension.

```sh
./assembler file1.as [file2.as ...]
```

### Process Overview

1.  **Input:** The assembler takes one or more files with the `.as` extension.
2.  **Pre-Assembly:** For each input file, it first performs a pre-assembler pass to handle macros. This creates an intermediate file with a `.am` extension.
3.  **First Pass:** It reads the `.am` file, builds a symbol table, and validates the code.
4.  **Second Pass:** If no errors were found in the first pass, it generates the final output files (`.ob`, `.ent`, `.ext`).
5.  **Error Handling:** If any errors are detected during the process, they are printed to standard output, and the final output files for the erroneous source file are deleted.

## Project Structure

The project is organized into several modules, each responsible for a specific part of the assembly process:

-   `assembler.c`: The main program driver that orchestrates the assembly process.
-   `preassembler.c`: Handles the macro expansion pass.
-   `firstpassfuncs.c` / `firstpass_input_check.c`: Contains the logic for the first pass, including symbol table creation and syntax checking.
-   `secondpassfuncs.c` / `secondpass_errors.c`: Contains the logic for the second pass, including instruction encoding and output file generation.
-   `symblist.c`: Manages the linked list for the symbol table.
-   `macrolist.c`: Manages the linked list for macros.
-   `wordlist.c`: Manages the linked list for the machine code words.
-   `file_funcs.c`: Provides utility functions for file operations.
-   `coding_funcs.c`: Contains functions for encoding data into the special base-64 representation.
-   `tables_and_arrays.c`: Initializes tables and arrays used for syntax and instruction validation.

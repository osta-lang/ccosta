# ccosta
Compiler for Osta written in C

## How to compile
### Dependencies
- `make`
- `gcc`

### Compiling
To compile the project, run:
```bash
make
```
This will generate a binary called `costa` in the `.build` directory.
You can run it with:
```bash
.build/dist/costa
```

### Profiles
You can compile with different profiles by setting the `PROFILE` variable.
```bash
make PROFILE=debug
```

The available profiles are:
- `debug`: Compile with debug symbols and without optimizations.
- `release`: Compile with optimizations and debug symbols.
- `dist` (**default**): Compile with optimizations and without debug symbols.

Executable will be generated in the `.build/<profile>` directory.

### Cleaning
To clean the project, run:
```bash
make clean
```

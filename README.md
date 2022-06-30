# Build Your Own Lisp
My take on https://buildyourownlisp.com

## Compiling / Building

### Installing Dependencies

#### Arch Linux
```bash
yay -Sy clang cmake
```

#### Ubuntu
```bash
sudo apt-get install clang cmake
```

## Usage

### Setup
This will build cmake files and download dependencies
```bash
make setup
```

### Build
```bash
# Debug build
make build
# Release build
make release
```

### Run Tests
This will build and run unity tests
```bash
make test
```

### Clean
This will delete generated files for debug and release
```bash
make clean
```

### Run Binary
```bash
# Debug main bin
make run
# Debug prompt bin
make run bin=prompt

# Release bin
./build/release/bin/main
```

## License
This is free and unencumbered software released into the public domain
For more information, please refer to <http://unlicense.org/>

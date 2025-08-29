# Contact Interest Form

A simple cross-platform C application for collecting names and email addresses from interested users.
Written as a simple thing for a university club. 
## Features

- Cross-platform (Linux/Windows/maybe macOS?)
- Real-time saving to `contacts.txt`
- Email validation
- Name validation
- _Fancy_ console interface

## Build Instructions

### Using CMake (recommended)
```bash
mkdir build
cd build
cmake ..
cmake --build .
./bin/contact_form
```

### Direct compilation
```bash
# Linux/macOS
clang -o contact_form main.c -std=c99 -Wall -O2
or
gcc -c contact_form main.c -std=c99 -Wall -02
# Windows (MinGW)
gcc -o contact_form.exe main.c -std=c99 -Wall -O2
```

## Output

Everything is saved to `contacts.txt` in the same directory as the executable.
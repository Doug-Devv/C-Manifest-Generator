# FXManifest Generator

Automatically scans your FiveM resource folder and generates a **fxmanifest.lua** with all scripts, UI files, dependencies, and other resource files. Perfect for developers who want to automate resource setup.

---

## Features

- Detects **client, server, and shared Lua scripts** automatically  
- Detects **UI pages** (`.html`) and other resource files (`.js`, `.css`, images, audio, etc.)  
- Detects **dependencies** from Lua files (look for `dependency 'resource_name'`)  
- Generates a **properly formatted `fxmanifest.lua`**  
- Supports **dry-run** (preview manifest) and **verbose output**  

---

## Prerequisites

- **Windows, Linux, or Mac**  
- **C++17 compatible compiler**  
  - Windows: Visual Studio 2017+ or MinGW-w64  
  - Linux: GCC 9+  
- Basic understanding of **FiveM resource folders**  

---

## Installation

### 1. Clone this repository

```bash
git clone https://github.com/flosseyy/C-Manifest-Generator
cd FXManifestGenerator
```

### 2. Compile the program

#### Windows (Visual Studio)

1. Open Visual Studio → Create a new C++ Console App project
2. Add `main.cpp` to Source Files
3. Right-click your project → Properties → C/C++ → Language → C++17
4. Build Solution (Ctrl+Shift+B)
5. The `.exe` will be in `Debug/` or `x64/Debug/` depending on your configuration

#### Windows (MinGW)

```bash
g++ -std=c++17 main.cpp -o fxmanifest_generator.exe
```

If you get errors with `std::filesystem`, add: `-lstdc++fs`

#### Linux / Mac

```bash
g++ -std=c++17 main.cpp -o fxmanifest_generator
```

Run: `./fxmanifest_generator`

---

## Usage

### Interactive mode

Run the program:

```bash
fxmanifest_generator.exe
```

You will be prompted:

```
Enter the resource folder path:
```

Enter the path to your FiveM resource folder, e.g.:

```
C:\FiveMServer\resources\my_resource
```

The program will scan the folder and generate `fxmanifest.lua` automatically.

### Command line options

**Specify folder directly:**

```bash
fxmanifest_generator.exe "C:\FiveMServer\resources\my_resource"
```

**Verbose output (show every file found):**

```bash
fxmanifest_generator.exe "C:\path\to\resource" --verbose
```

**Dry-run (preview manifest in console without writing file):**

```bash
fxmanifest_generator.exe "C:\path\to\resource" --dry-run
```

**Combine flags:**

```bash
fxmanifest_generator.exe "C:\path\to\resource" --verbose --dry-run
```

---

## Test Resource

For testing, create a folder structure like this:

```
my_test_resource/
├── client/cl_test.lua
├── server/sv_test.lua
├── shared/sh_utils.lua
├── ui/index.html
├── config.json
└── script.js
```

Populate files with simple test code. Running the generator should create a working `fxmanifest.lua` including all files.

---

## Tips

- Always run the generator in the folder containing your resource, not the server root.
- Use `--dry-run` to preview before overwriting existing `fxmanifest.lua`.
- Make sure your Lua filenames follow conventions (`cl_`, `sv_`, `sh_`) for proper categorization.

---

## License

MIT License - feel free to use and modify as needed.

---

## Contributing

Pull requests are welcome! For major changes, please open an issue first to discuss what you would like to change.

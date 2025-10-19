# Simple Calculator (C++/CMake)

## Prerequisites

- **CMake** >= 3.8 (Tested with 3.31.6-msvc6)
- **Ninja** build system
- **C++20** compatible compiler (e.g., MSVC, GCC, Clang)
- **WiX Toolset** 4.0.1 (for packaging, Windows only)
- (Optional) **Jenkins** for CI/CD

## Setup & Build

1. **Clone the repository**
   ```sh
   git clone <repo-url>
   cd CMakeProject1
   ```

2. **Configure the project**
   ```sh
   cmake -S . -B build -G Ninja
   ```

3. **Build the project**
   ```sh
   cmake --build build
   ```

4. **Run the calculator**
   ```sh
   ./build/CMakeProject1/CMakeProject1
   ```

## Packaging with WiX 4.0.1

1. **Install WiX Toolset 4.0.1**
   - Download from https://wixtoolset.org/releases/
   - Add WiX tools (e.g., `candle.exe`, `light.exe`) to your PATH

2. **Build the installer**
   ```sh
   cd installer
   candle Product.wxs
   light -o CalculatorInstaller.msi Product.wixobj
   ```
   The MSI will be generated as `CalculatorInstaller.msi`.

## Jenkins CI/CD

- See `Jenkinsfile` for a sample pipeline that builds and packages the project.
- Configure Jenkins with CMake, Ninja, and WiX in the agent PATH.

---

**Project Structure:**
- `CMakeProject1/` - Source code
- `installer/Product.wxs` - WiX installer script
- `Jenkinsfile` - Jenkins pipeline
- `README.md` - This file

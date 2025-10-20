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


## Code signing (Windows)

This project supports signing the generated MSI with Microsoft's SignTool (signtool.exe) from the Windows SDK. The Jenkins pipeline includes a `Code Sign` stage which expects a PFX (personal information exchange) certificate uploaded to Jenkins as credentials.

What you need:
- A code signing certificate in `.pfx` format (can be a commercial EV/OV certificate or a test cert for development).
- The password for the `.pfx` file.
- Windows agent(s) with the Windows SDK (to provide `signtool.exe`) or ensure `signtool.exe` is on PATH.
- Jenkins credentials configured as described below.

Jenkins credential setup
1. In Jenkins, go to Credentials -> System -> Global credentials (unrestricted) -> Add Credentials.
2. Add a "Secret file" credential containing your `.pfx` file. Give it the ID: `SIGN_PFX`.
3. Add a "Secret text" credential with the PFX password. Give it the ID: `SIGN_PFX_PASSWORD`.
4. (Optional) Add a string credential for SonarQube token with ID: `SONAR_TOKEN`.

How the pipeline uses credentials
- `SIGN_PFX` (file): The pipeline uses `withCredentials([file(credentialsId: 'SIGN_PFX', variable: 'SIGN_PFX_FILE')])` to access the PFX file path.
- `SIGN_PFX_PASSWORD` (string): The pipeline uses `withCredentials([string(credentialsId: 'SIGN_PFX_PASSWORD', variable: 'SIGN_PFX_PASSWORD')])` to provide the password.

Local testing of signing (PowerShell)
1. Install the Windows 10/11 SDK which contains `signtool.exe` (or install Visual Studio with the SDK components).
2. From an elevated PowerShell prompt, run:

```powershell
# Path to signtool.exe may vary depending on SDK version and install location

---

**Project Structure:**

# Verify signature
& "C:\Program Files (x86)\Windows Kits\10\bin\x64\signtool.exe" verify /pa /v "installer\CalculatorInstaller.msi"
```

Notes and best practices
- For production builds, use an EV (Extended Validation) code signing certificate and protect the PFX using a secure hardware token or HSM where possible.
- Prefer time-stamping the signature (`/tr`) so signatures remain valid after the certificate expires.
- If you require dual-signing or MSIX signing, extend the pipeline accordingly.

Pipeline parameters and agent labels
- The `Jenkinsfile` defines parameters `SONAR_SCAN`, `SIGN`, and `PUBLISH` to toggle Sonar analysis, code signing, and publishing.
- The packaging and signing stages run on an agent labeled `windows`. Ensure at least one Windows agent with WiX and Windows SDK installed is available.

Troubleshooting
- If `signtool.exe` is not found on the Windows agent, verify the Windows SDK installation and update PATH or point directly to the executable in the pipeline.
- If signing fails with certificate errors, confirm the PFX password and that the PFX contains a private key.
- `CMakeProject1/` - Source code
- `installer/Product.wxs` - WiX installer script
- `Jenkinsfile` - Jenkins pipeline
- `README.md` - This file

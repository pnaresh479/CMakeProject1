pipeline {
    agent { label 'built-in' } // Use the Jenkins controller or specify 'windows' if you have Windows agents

    environment {
        WIX_PATH           = "C:/users/admin/.dotnet/tools"
        CMAKE_PATH         = "C:/DevTools/CMake3_39_1/bin"
        NNJA_PATH          = "C:/DevTools/ninja1_12_1"
        GCC                = "C:/DevTools/mingw-w64/mingw64/bin"
        BUILD_WRAPPER_PATH = "C:/DevTools/sonar-cpp-build-wrapper/build-wrapper-win-x86"
        SONARQUBE_SERVER   = 'sonarcloud'
        PROJECT_KEY        = 'CMakeProject1'
        ORGANIZATION       = 'pnaresh479'
        BUILD_CONFIG       = 'Release'
        APP_PATH           = 'CMakeProject1'
        INSTALLER_PATH     = 'installer'
        SONAR_CLI_PATH     = "C:/DevTools/sonar-cli/sonar-scanner-7.2.0.5079-windows-x64/bin"
        DOTNET_PATH        = "C:/Program Files/dotnet/dotnet.exe"
        CODE_SIGN_CERT     = "C://code-sign-certificates"  
        SIGN_TOOL_PATH     = "C://Program Files (x86)//Windows Kits//10//bin//10.0.19041.0//x64"
    }

    options {
        buildDiscarder(logRotator(numToKeepStr: '20'))
        timestamps()
    }

    parameters {
        booleanParam(name: 'SONAR_SCAN', defaultValue: false, description: 'Run SonarQube scan')
        booleanParam(name: 'SIGN', defaultValue: false, description: 'Sign installer using provided PFX')
        booleanParam(name: 'PUBLISH', defaultValue: false, description: 'Publish artifacts after build')
    }

    stages {
        stage('Clean Workspace') {
            steps {
                deleteDir()
            }
        }

        stage('Checkout') {
            steps {
                echo '📦 Checking out code from GitHub...'
                git branch: 'main', url: 'https://github.com/pnaresh479/CMakeProject1.git'
            }
        }

        stage('Install Dependencies') {
            steps {
                echo 'Ensure CMake, Ninja, and compiler are installed on Windows agents.'
            }
        }

        stage('verify CMake and Ninja') {
            steps {
                echo '🔧 Verifying CMake and Ninja installation...'
                withEnv(["PATH=${env.PATH};${env.CMAKE_PATH};${env.NNJA_PATH}"]) {
                    bat '''
                        cmake --version
                        ninja --version
                    '''
                }
            }
        }

        stage('verify Compiler') {
            steps {
                echo '🔧 Verifying GCC/G++ installation...'
                withEnv(["PATH=${env.PATH};${env.GCC}"]) {
                bat '''
                    gcc --version
                '''
                }
            }
        }

        stage('Build Application') {
            steps {
                echo '🏗️ Building the C++ project...'
                dir("${APP_PATH}") {
                    withEnv(["PATH=${env.PATH};${env.CMAKE_PATH};${env.NNJA_PATH};${env.GCC};${env.BUILD_WRAPPER_PATH}"]) {
                    bat '''
                        echo cleaning build directory...
                        if exist build rmdir /s /q build

                        echo creating build directory...
                        mkdir build

                        echo Running CMake configuration...
                        cmake -S . -B build -G Ninja -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

                        echo performing clean build...
                        build-wrapper-win-x86-64.exe --out-dir bw-output cmake --build build

                        echo "APP_PATH is ${APP_PATH}"
                        dir /s /b
                    '''
                    }
                }
                stash name: 'cpp-build-output', includes: 'CMakeProject1/build/**'
            }
        }

        stage('Display Jenkins workspace') {
            steps {
                echo ' Displaying Jenkins workspace structure...'
                bat '''
                    echo 'Current working directory is:' + %CD%
                    echo Directory structure:
                    dir /s
                '''
            }
        }

        // stage('Build Wrapper for SonarQube') {
        //     when { expression { return params.SONAR_SCAN == true } }
        //     steps {
        //         echo '🏗️ Generating compile_commands.json for SonarQube CFamily...'
        //         dir("${APP_PATH}") {
        //             withEnv(["PATH=${env.PATH};${env.CMAKE_PATH};${env.NNJA_PATH};${env.BUILD_WRAPPER_PATH};${env.GCC}"]) {
        //             bat '''
        //                 build-wrapper-win-x86-64.exe --out-dir bw-output cmake --build build
        //             '''
        //             }
        //         }
        //     }
        // }

        stage('SonarQube Analysis') {
            when { expression { return params.SONAR_SCAN == true } }
            steps {
                echo '🔍 Running SonarQube analysis...'
                dir("${APP_PATH}") {
                    withCredentials([string(credentialsId: 'sonarcloud-token', variable: 'SONAR_TOKEN')]) {
                        withEnv(["PATH=${env.PATH};${env.SONAR_CLI_PATH}"]) {
                            script {
                                def sonarclipath = "${env.SONAR_CLI_PATH}/sonar-scanner.bat"
                                def sonarCmd = """
                                    "${sonarclipath}" ^
                                    -Dsonar.token=${SONAR_TOKEN} ^
                                    -Dsonar.projectKey=${PROJECT_KEY} ^
                                    -Dsonar.organization=${ORGANIZATION} ^
                                    -Dsonar.host.url=https://sonarcloud.io ^
                                    -Dsonar.cfamily.compile-commands=build/compile_commands.json
                                """
                                bat sonarCmd
                            }
                        }
                    }
                }
            }
        }

        stage('Display Build Artifacts') {
            steps {
                echo '📂 Displaying build artifacts...'
                dir("${APP_PATH}/build/CMakeProject1/Release") {
                    bat 'dir'
                }
            }
        }

        stage('Binary copy') {
            agent { label 'built-in' }
            steps {
                // Ensure the output directory exists
                script {
                    echo '📦 Packaging application into MSI installer using WiX...'
                    echo 'unstashing previous step output here...'
                    unstash 'cpp-build-output'
                }
                
                bat '''
                    if not exist "%WORKSPACE%\\installer" mkdir "%WORKSPACE%\\installer"
                '''
                bat 'dir "%WORKSPACE%\\CMakeProject1\\build\\CMakeProject1.exe"'
                bat 'copy %WORKSPACE%\\CMakeProject1\\build\\CMakeProject1.exe %WORKSPACE%\\installer\\CMakeProject1.exe'
                
                // Run WiX build from the workspace root so relative paths work correctly
               
            }
        }

        stage('Package (WiX)') {
            agent { label 'built-in' }
            steps {
                // Ensure the output directory exists
                script {
                    echo '📦 Packaging application into MSI installer using WiX...'
                    echo 'unstashing previous step output here...'
                    unstash 'cpp-build-output'
                }
                dir("${INSTALLER_PATH}") {
                    withEnv(["PATH=${env.PATH};${env.WIX_PATH}"]) {
                        bat '''
                            echo current directory is... : %CD% 
                            dir
                            wix build product.wxs  -o calculatorCppApp.msi"
                        '''
                    }
                }
                
                // bat '''
                //     if not exist "%WORKSPACE%\\installer" mkdir "%WORKSPACE%\\installer"
                // '''
                // bat 'dir "%WORKSPACE%\\CMakeProject1\\build\\CMakeProject1.exe"'
                
                // // Run WiX build from the workspace root so relative paths work correctly
                // withEnv(["PATH=${env.PATH};${env.WIX_PATH};${env.DOTNET_PATH}"]) {
                //     bat '''
                //         echo "Building MSI installer..."
                //         echo "Current directory: %CD%"
                //         echo "Checking source exe exists..."
                //         wix build -src %WORKSPACE%/installer/product.wxs  -o %WORKSPACE%/installer/calculatorCppApp.msi"
                //     '''
                // }
                
                stash name: 'installer-msi', includes: 'installer/calculatorCppApp.msi'
            }
        }

        // stage('Code Sign (Windows)') {
        //     agent { label 'built-in' }
        //     when { expression { return params.SIGN == true } }
        //     steps {
        //         script {
        //             withCredentials([
        //                 file(credentialsId: 'SIGN_PFX', variable: 'SIGN_PFX_FILE'),
        //                 string(credentialsId: 'SIGN_PFX_PASSWORD', variable: 'SIGN_PFX_PASSWORD')
        //             ]) {
        //                 unstash 'installer-msi'
        //                 powershell """
        //                     & "$env:ProgramFiles\\Windows Kits\\10\\bin\\x64\\signtool.exe" sign `
        //                         /f "${SIGN_PFX_FILE}" `
        //                         /p "${SIGN_PFX_PASSWORD}" `
        //                         /tr http://timestamp.digicert.com `
        //                         /td sha256 `
        //                         /fd sha256 `
        //                         "${INSTALLER_PATH}\\calculatorCppApp.msi"
        //                 """
        //             }
        //         }
        //     }
        // }

        stage('code sign the installaer..') {
            when { expression { retun param.SIGN == true } }
            steps {
                echo 'Signing the installer using SignTool...'
                dir('${INSTALLER_PATH}') {
                    withEnv(["PATH=${env.PATH};${env.SIGN_TOOL_PATH}"]) {
                        bat """
                            signtool sign /f "${env.CODE_SIGN_CERT}/my_cert.pfx" /p "${SIGN_PFX_PASSWORD}" /tr http://timestamp.digicert.com /td sha256 /fd sha256 "calculatorCppApp.msi"
                            """
                    }
                }
            }
        }

        stage('verification of signed installer') {
            when { expression { return param.SIGN == true } }
            steps {
                echo 'Verifying the signed installer...'
                dir('${INSTALLER_PATH}') {
                    withEnv(["PATH=${env.PATH};${env.SIGN_TOOL_PATH}"]) {
                        bat '''
                            signtool verify /pa /v "calculatorCppApp.msi"
                            '''
                    }
                }
            }
        }


        stage('Archive') {
            steps {
                unstash 'installer-msi'
                archiveArtifacts artifacts: "${INSTALLER_PATH}/calculcatorcplusapp.msi", fingerprint: true
            }
        }

        // stage('Archive Artifacts') {
        //     steps {
        //         echo '🗄️ Archiving installer output...'
        //         // dir("${INSTALLER_PATH}") {
        //         archiveArtifacts artifacts: 'installer/*.msi', fingerprint: true
        //         // }
        //     }
        // }

        stage('Publish (Optional)') {
            when { expression { return params.PUBLISH == true } }
            steps {
                echo 'Publishing artifacts to configured repository or storage (add upload logic here).'
            }
        }
    }

    post {
        always {
            junit allowEmptyResults: true, testResults: 'build/**/TestResults/*.xml'
            // cleanWs()
        }
        success {
            echo '✅ Build completed successfully.'
        }
        failure {
            echo '❌ Build failed. See logs for details.'
        }
    }
}
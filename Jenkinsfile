pipeline {
    agent { label 'built-in' } // Use the Jenkins controller or specify 'windows' if you have Windows agents

    environment {
        WIX_PATH           = "C:/users/admin/.dotnet/tools"
        CMAKE_PATH         = "C:/DevTools/CMake3_39_1/bin"
        MINJA_PATH         = "C:/DevTools/ninja1_12_1"
        GCC                = "C:/DevTools/mingw-w64/mingw64/bin"
        BUILD_WRAPPER_PATH = "C:/DevTools/sonar-cpp-build-wrapper/build-wrapper-win-x86"
        SONARQUBE_SERVER   = 'sonarcloud'
        PROJECT_KEY        = 'CMakeProject1'
        ORGANIZATION       = 'pnaresh479'
        BUILD_CONFIG       = 'Release'
        APP_PATH           = 'CMakeProject1'
        INSTALLER_PATH     = 'installer'
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
                withEnv(["PATH=${env.PATH};${env.CMAKE_PATH};${env.MINJA_PATH}"]) {
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
                    withEnv(["PATH=${env.PATH};${env.CMAKE_PATH};${env.MINJA_PATH};${env.GCC}"]) {
                    bat '''
                        cmake -S . -B build -G Ninja -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
                        cmake --build build --config Release
                    '''
                    }
                }
            }
        }

        stage('Build Wrapper for SonarQube') {
            when { expression { return params.SONAR_SCAN == true } }
            steps {
                echo '🏗️ Generating compile_commands.json for SonarQube CFamily...'
                dir("${APP_PATH}") {
                    withEnv(["PATH=${env.PATH};${env.CMAKE_PATH};${env.MINJA_PATH};${env.BUILD_WRAPPER_PATH};${env.GCC}"]) {
                    bat '''
                        build-wrapper-win-x86-64.exe --out-dir bw-output cmake --build build
                    '''
                    }
                }
            }
        }

        stage('SonarQube Analysis') {
            when { expression { return params.SONAR_SCAN == true } }
            steps {
                echo '🔍 Running SonarQube analysis...'
                // dir("${APP_PATH}") {
                //     withCredentials([string(credentialsId: 'sonarcloud-token-jenkins', variable: 'SONAR_TOKEN')]) {
                //         script {
                //             def sonarCmd = """
                //                 sonar-scanner ^
                //                   -Dsonar.token=${SONAR_TOKEN} ^
                //                   -Dsonar.projectKey=${PROJECT_KEY} ^
                //                   -Dsonar.organization=${ORGANIZATION} ^
                //                   -Dsonar.host.url=https://sonarcloud.io ^
                //                   -Dsonar.cfamily.compile-commands=bw-output/compile_commands.json
                //             """
                //             bat sonarCmd
                //         }
                //     }
                // }
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

        stage('Package (WiX)') {
            agent { label 'built-in' }
            steps {
                dir("${INSTALLER_PATH}") {
                    withEnv(["PATH=${env.PATH};${env.WIX_PATH}"]) {
                        bat '''
                            powershell -NoProfile -ExecutionPolicy Bypass -Command ^
                            "wix build calculcatorcplusapp.wxs -o calculcatorcplusapp.msi"
                        '''
                    }
                }
                stash name: 'installer-msi', includes: "${INSTALLER_PATH}/calculcatorcplusapp.msi"
            }
        }

        stage('Code Sign (Windows)') {
            agent { label 'built-in' }
            when { expression { return params.SIGN == true } }
            steps {
                script {
                    withCredentials([
                        file(credentialsId: 'SIGN_PFX', variable: 'SIGN_PFX_FILE'),
                        string(credentialsId: 'SIGN_PFX_PASSWORD', variable: 'SIGN_PFX_PASSWORD')
                    ]) {
                        unstash 'installer-msi'
                        powershell """
                            & "$env:ProgramFiles\\Windows Kits\\10\\bin\\x64\\signtool.exe" sign `
                                /f "${SIGN_PFX_FILE}" `
                                /p "${SIGN_PFX_PASSWORD}" `
                                /tr http://timestamp.digicert.com `
                                /td sha256 `
                                /fd sha256 `
                                "${INSTALLER_PATH}\\calculcatorcplusapp.msi"
                        """
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
            cleanWs()
        }
        success {
            echo '✅ Build completed successfully.'
        }
        failure {
            echo '❌ Build failed. See logs for details.'
        }
    }
}
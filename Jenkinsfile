pipeline {
    agent { label 'built-in' }  // Force the pipeline to run on the master node

    environment {
        // WiX and Windows SDK tools will be needed on the Windows agent(s)
        WIFI_PATH = "C:/Program Files (x86)/WiX Toolset v4.0/bin"
        SONARQUBE_SERVER = 'sonarcloud'
        PROJECT_KEY = 'CMakeProject1'
        ORGANIZATION = 'pnaresh479'
        BUILD_CONFIG = 'Release' 
        APP_PATH = 'CMakeProject1'
        INSTALLER_PATH = 'Installer'
        // SonarQube env vars are provided by Jenkins SonarQube plugin or pipeline credentials
    }

    options {
        buildDiscarder(logRotator(numToKeepStr: '20'))
        timestamps()
        ansiColor('xterm')
    }

    stages {
        stage('Clean workspace') {
            steps { deleteDir() }
        }

        stage('Checkout') {
            steps {
                echo '📦 Checking out code from GitHub...'
                git branch: 'main', url: 'https://github.com/pnaresh479/CMakeProject1.git'
            }
        }

        stage('Install dependencies') {
            steps {
                script {
                    // Install or prepare any build prerequisites. For a simple C++ project
                    // we assume cmake and ninja are available on the agents. If not, add
                    // installer steps here per platform.
                    echo 'Ensure cmake, ninja and compiler are installed on agents.'
                }
            }
        }

        // stage('Configure & Build') {
        //     parallel {
        //         stage('Linux/macOS Build') {
        //             agent { label 'linux || mac' }
        //             steps {
        //                 sh 'cmake -S . -B build -G Ninja'
        //                 sh 'cmake --build build --config Release'
        //                 // Optionally run tests here if added
        //             }
        //         }

        //         stage('Windows Build') {
        //             agent { label 'windows' }
        //             steps {
        //                 // Use PowerShell for Windows commands
        //                 powershell 'cmake -S . -B build -G Ninja'
        //                 powershell 'cmake --build build --config Release'
        //             }
        //         }
        //     }
        // }

        stage('Build Application') {
            steps {
                echo '🏗️ Building the c++  project...'
                dir("${APP_PATH}") {
                    // Use PowerShell for Windows commands
                        powershell 'cmake -S . -B build -G Ninja'
                        powershell 'cmake --build build --config Release'
                        powershell 'build-wrapper-win-x86-64.exe --out-dir bw-output cmake --build build'
                }
            }
        }

        stage('Build Wrapper for SonarQube') {
            steps {
                echo '🏗️ Building  warpper compiler comamnds for the c++  project...'
                dir("${APP_PATH}") {
                    // Use PowerShell for Windows commands
                        powershell 'build-wrapper-win-x86-64.exe --out-dir bw-output cmake --build build'
                }
            }
        }

        stage('SonarQube Analysis') {
            when { expression { return env.SONAR_HOST_URL != null || params.SONAR_SCAN == 'true' } }
            steps {
                script {
                    // Assumes SonarQube scanner CLI or Jenkins SonarQube plugin is configured
                    echo 'Running SonarQube analysis (configure SONAR_TOKEN and SONAR_HOST_URL in Jenkins)'
                    withCredentials([string(credentialsId: 'sonarcloud-token-jenkins', variable: 'SONAR_TOKEN')]) {
                        // Example using sonar-scanner CLI if available on agent
                       //sh "sonar-scanner -Dsonar.login=${env.SONAR_TOKEN} -Dsonar.projectKey=${env.PROJECT_KEY}"
                       bat """
                          sonar-scanner -Dsonar.token=${env.SONAR_TOKEN} -Dsonar.projectKey=${env.PROJECT_KEY}^
                           -Dsonar.organization=${env.ORGANIZATION} -Dsonar.host.url=https://sonarcloud.io ^
                           -Dsonar.cfamily.compile-commands=bw-output/compile_commands.json
                          """
                    }
                }
            }
        }

        stage('Package (WiX)') {
            steps {
                dir('installer') {
                    // Ensure WiX toolset is on PATH for the Windows agent
                    withEnv(["PATH=${env.PATH};${WIFI_PATH}"]) {
                        powershell 'if (-not (Test-Path -Path "calculcatorcplusapp.wxs")) { Write-Error "calculcatorcplusapp.wxs not found in installer/"; exit 1 }
                                     wix build calculcatorcplusapp.wxs -o CalculatorcplusApp.msi
                    }
                }
                stash name: 'installer-msi', includes: 'installer/CalculatorcplusApp.msi'
            }
        }

        stage('Code Sign (Windows)') {
            agent { label 'windows' }
            when { expression { return params.SIGN == true || env.SIGN_PFX_CREDENTIAL_ID != null } }
            steps {
                script {
                    // Expecting a PFX file uploaded as Jenkins "Secret file" credential
                    // with id SIGN_PFX and password stored in SIGN_PFX_PASSWORD
                    withCredentials([
                        file(credentialsId: 'SIGN_PFX', variable: 'SIGN_PFX_FILE'),
                        string(credentialsId: 'SIGN_PFX_PASSWORD', variable: 'SIGN_PFX_PASSWORD')
                    ]) {
                        unstash 'installer-msi'
                        powershell "& $env:ProgramFiles\\Windows Kits\\10\\bin\\x64\\signtool.exe sign /f ${SIGN_PFX_FILE} /p ${SIGN_PFX_PASSWORD} /tr http://timestamp.digicert.com /td sha256 /fd sha256 installer\\CalculatorInstaller.msi"
                    }
                }
            }
        }

        stage('Archive') {
            steps {
                unstash 'installer-msi'
                archiveArtifacts artifacts: 'installer/CalculatorcplusApp.msi', fingerprint: true
            }
        }

        stage('Publish (optional)') {
            steps {
                script {
                    if (params.PUBLISH == true) {
                        echo 'Publishing artifacts to configured repository or storage. Implement as needed.'
                        // Add publishing steps: upload to artifact server, S3, Nexus, etc.
                    } else {
                        echo 'Publish skipped (PUBLISH param not set)'
                    }
                }
            }
        }
    }

    post {
        always {
            junit allowEmptyResults: true, testResults: 'build/**/TestResults/*.xml'
            cleanWs()
        }
        success {
            echo 'Build completed successfully.'
        }
        failure {
            echo 'Build failed. See logs.'
        }
    }

    parameters {
        booleanParam(defaultValue: false, description: 'Run SonarQube scan', name: 'SONAR_SCAN')
        booleanParam(defaultValue: false, description: 'Sign installer using provided PFX', name: 'SIGN')
        booleanParam(defaultValue: false, description: 'Publish artifacts after build', name: 'PUBLISH')
    }
}

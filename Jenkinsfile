pipeline {
    agent any
    environment {
        PATH = "${env.PATH};C:/Program Files (x86)/WiX Toolset v4.0/bin" // Adjust WiX path as needed
    }
    stages {
        stage('Clean') {
            steps {
                deleteDir()
            }
        }
        stage('Checkout') {
            steps {
                checkout scm
            }
        }
        stage('Configure') {
            steps {
                sh 'cmake -S . -B build -G Ninja'
            }
        }
        stage('Build') {
            steps {
                sh 'cmake --build build'
            }
        }
        stage('Package') {
            steps {
                dir('installer') {
                    sh 'candle Product.wxs'
                    sh 'light -o CalculatorInstaller.msi Product.wixobj'
                }
            }
        }
        stage('Archive') {
            steps {
                archiveArtifacts artifacts: 'installer/CalculatorInstaller.msi', fingerprint: true
            }
        }
    }
}

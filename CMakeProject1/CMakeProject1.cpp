// CMakeProject1.cpp : Defines the entry point for the application.
//

#include "CMakeProject1.h"

using namespace std;

double Calculator::add(double a, double b) {
    return a + b;
}

double Calculator::subtract(double a, double b) {
    return a - b;
}

double Calculator::multiply(double a, double b) {
    return a * b;
}

double Calculator::divide(double a, double b) {
    if (b == 0) throw invalid_argument("Division by zero");
    return a / b;
}

int main() {
    Calculator calc;
    double a, b;
    int choice;
    cout << "Simple Calculator\n";
    cout << "Enter first number: ";
    cin >> a;
    cout << "Enter second number: ";
    cin >> b;
    cout << "Choose operation:\n1. Add\n2. Subtract\n3. Multiply\n4. Divide\nChoice: ";
    cin >> choice;
    try {
        double result = 0;
        switch (choice) {
            case 1: result = calc.add(a, b); break;
            case 2: result = calc.subtract(a, b); break;
            case 3: result = calc.multiply(a, b); break;
            case 4: result = calc.divide(a, b); break;
            default: cout << "Invalid choice." << endl; return 1;
        }
        cout << "Result: " << result << endl;
    } catch (const exception& ex) {
        cout << "Error: " << ex.what() << endl;
        return 1;
    }
    return 0;
}

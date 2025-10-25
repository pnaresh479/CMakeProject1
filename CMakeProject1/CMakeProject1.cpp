#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>
using namespace std;

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

        // Sleep for 20 seconds
        std::this_thread::sleep_for(std::chrono::seconds(20));
    } catch (const exception& ex) {
        cout << "Error: " << ex.what() << endl;
        return 1;
    }
    return 0;
}
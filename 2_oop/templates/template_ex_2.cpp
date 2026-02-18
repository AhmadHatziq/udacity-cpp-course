#include <iostream>

// 1️⃣ Function template — prints any array
template <typename T>

void printArray(const T* arr, int size) {

    for (int i = 0; i < size; ++i)
        std::cout << arr[i] << " ";

    std::cout << "\n";
}

// 2️⃣ Class template — holds a pair of values

template <typename T1, typename T2>

class Pair {

private:
    T1 first;
    T2 second;

public:
    Pair(T1 a, T2 b) : first(a), second(b) {}

    void show() const {
        std::cout << "(" << first << ", " << second << ")\n";
    }
};

int main() {

    // --- Using the function template ---
    int numbers[] = {1, 2, 3, 4, 5};
    std::string words[] = {"hello", "templates"};
    std::cout << "Integer array: ";
    printArray(numbers, 5);

    std::cout << "String array: ";
    printArray(words, 2);

    // --- Using the class template ---
    Pair<int, std::string> person(25, "Alice");
    Pair<double, double> point(3.5, 7.2);
    std::cout << "Person age and name: ";

    person.show();
    std::cout << "Point coordinates: ";
    point.show();

}

/*
Expected output:
    Integer array: 1 2 3 4 5 
    String array: hello templates 
    Person age and name: (25, Alice)
    Point coordinates: (3.5, 7.2)
*/
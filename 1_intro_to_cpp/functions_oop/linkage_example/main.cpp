#include <iostream>
#include "utils.h"

int main() {
    std::cout << "=== main.cpp ===\n";

    // External Linkage
    incrementExternal();
    incrementExternal();
    std::cout << "[main] externalCounter = " << externalCounter << "\n\n";

    // Direct access to internal symbols
    incrementInternal();
    incrementAnon();        
    std::cout << internalCounter; 
    std::cout << anonCounter;    

    return 0;
}

#include <iostream>
#include "utils.h"

// =================== INTERNAL LINKAGE ===================

// Static variable (internal linkage)
static int internalCounter = 0;

static void incrementInternal() {
    internalCounter++;
    std::cout << "[internal static] internalCounter = " << internalCounter << '\n';
}

// Anonymous namespace (also internal linkage)
namespace {
    int anonCounter = 0;

    void incrementAnon() {
        anonCounter++;
        std::cout << "[internal anon] anonCounter = " << anonCounter << '\n';
    }
}

// =================== EXTERNAL LINKAGE ===================

// Global variable with external linkage
int externalCounter = 0;

void incrementExternal() {
    externalCounter++;
    std::cout << "[external] externalCounter = " << externalCounter << '\n';
}

// Wrapper function to show internal behavior from outside
void incrementInternalVariables() {
    incrementInternal();
    incrementAnon();
}

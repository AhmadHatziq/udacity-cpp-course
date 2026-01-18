#ifndef UTILS_H
#define UTILS_H

// External linkage: declare a global variable
extern int externalCounter;

// External linkage: declare a function
void incrementExternal();

// Expose a wrapper to call internal functions (indirect access)
void incrementInternalVariables();

#endif


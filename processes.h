#ifndef PROCESSES_H
#define PROCESSES_H

#include <sys/types.h>
#include "types.h"

#define FINISHED 0
#define STOPPED 1
#define SIGNALED 2
#define ACTIVE 3

extern char** environ;

void doShowEnvironment(char* env[], char* env_name);
int doSearchVariable(char* env[], char* var_name);
void printVarArg3(char* val);
void printVarEnv(int index);
void printVarGetenv(char* val);
void doChangeVar(char* env[], char* var, char* value);
void doChangeVarPutenv(char* var, char* value);

#endif

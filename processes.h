#ifndef PROCESSES_H
#define PROCESSES_H

#include <sys/types.h>
#include "types.h"

#define FINISHED 0
#define STOPPED 1
#define SIGNALED 2
#define ACTIVE 3

extern char** environ;

void ShowEnvironment(char** env, char* env_name);


#endif

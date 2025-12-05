#include "processes.h"
#include "types.h"

#include <stdio.h>


tProcSignal proc_signals[] = {
    {"FINISHED", FINISHED},
    {"STOPPED", STOPPED},
    {"SIGNALED", SIGNALED},
    {"ACTIVE", ACTIVE},
    {NULL, -1}
};

void ShowEnvironment(char** env, char* env_name) {
	int i = 0;
	while (env[i] != NULL) {
		printf ("%p -> %s[%d] = (%p) %s\n", 
			&env[i], env_name, i, env[i], env[i]);
		i++;
	}
}

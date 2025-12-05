#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "processes.h"
#include "types.h"
#include "limits.h"


tProcSignal proc_signals[] = {
    {"FINISHED", FINISHED},
    {"STOPPED", STOPPED},
    {"SIGNALED", SIGNALED},
    {"ACTIVE", ACTIVE},
    {NULL, -1}
};

void printVar(char* env[], char* env_name, int index) {
	printf ("%p -> %s[%d] = (%p) %s\n", 
		&env[index], env_name, index, env[index], env[index]);
}

void doShowEnvironment(char* env[], char* env_name) {
	int i = 0;
	while (env[i] != NULL) {
		printVar(env, env_name, i);
		//printf ("%p -> %s[%d] = (%p) %s\n", 
		//	&env[i], env_name, i, env[i], env[i]);
		i++;
	}
}

int doSearchVariable(char* env[], char* var_name) {
	int pos = 0;
	char aux[CHAR_MAX];
	
	strcpy(aux, var_name);
	strcat(aux, "=");
  
	while (env[pos] != NULL)
		if (!strncmp(env[pos], aux, strlen(aux)))
			return (pos);
		else
			pos++;
				
	errno = ENOENT;
	return(-1);
}

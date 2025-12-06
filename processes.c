#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
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

void doShowEnvironment(char* env[], char* env_name) {
	int i = 0;
	while (env[i] != NULL) {
		printf ("%p -> %s[%d] = (%p) %s\n", 
			&env[i], env_name, i, env[i], env[i]);
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

void printVarArg3(char* var) {
	printf ("arg3 -> %s (%p) @ %p\n", var, var, &var);
}
void printVarEnv(int index) {
	printf ("environ -> %s (%p) @ %p\n", environ[index], environ[index], &environ[index]);
}
void printVarGetenv(char* var_name) {
	char* var = getenv(var_name);
	if (var == NULL) {
		errno = ENOENT;
		perror("getenv could not access variable");
		return;
	}
	printf ("getenv -> %s (%p)\n", var, &var);
}

void doChangeVar(char* env[], char* var, char* value) {
	int index;
	char* aux;

	if ((index = doSearchVariable(env, var)) == -1) {
		perror("Could not find variable in environment");
		return;
	}
	
	if ((aux=(char *)malloc(strlen(var)+strlen(value)+2))==NULL) {
		perror("malloc failed");
		return;
	}
	strcpy(aux, var);
	strcat(aux, "=");
	strcat(aux, value);
	env[index] = aux;
}

void doChangeVarPutenv(char* var, char* value) {

}



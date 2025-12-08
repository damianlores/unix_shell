#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <sys/wait.h>
#include "processes.h"
#include "types.h"
#include "dynamic_list.h"



static struct signal procstatus[] = {
    {"FINISHED", FINISHED},
    {"STOPPED", STOPPED},
    {"SIGNALED", SIGNALED},
    {"ACTIVE", ACTIVE},
    {NULL, -1}
};
// This functions allow us to obtain the name of a signal out of its value and viceversa
static struct signal sigstrnum[] = {   
	{"HUP", SIGHUP},
	{"INT", SIGINT},
	{"QUIT", SIGQUIT},
	{"ILL", SIGILL}, 
	{"TRAP", SIGTRAP},
	{"ABRT", SIGABRT},
	{"IOT", SIGIOT},
	{"BUS", SIGBUS},
	{"FPE", SIGFPE},
	{"KILL", SIGKILL},
	{"USR1", SIGUSR1},
	{"SEGV", SIGSEGV},
	{"USR2", SIGUSR2}, 
	{"PIPE", SIGPIPE},
	{"ALRM", SIGALRM},
	{"TERM", SIGTERM},
	{"CHLD", SIGCHLD},
	{"CONT", SIGCONT},
	{"STOP", SIGSTOP},
	{"TSTP", SIGTSTP}, 
	{"TTIN", SIGTTIN},
	{"TTOU", SIGTTOU},
	{"URG", SIGURG},
	{"XCPU", SIGXCPU},
	{"XFSZ", SIGXFSZ},
	{"VTALRM", SIGVTALRM},
	{"PROF", SIGPROF},
	{"WINCH", SIGWINCH}, 
	{"IO", SIGIO},
	{"SYS", SIGSYS},
// Signals which are not present everywhere
#ifdef SIGPOLL
	{"POLL", SIGPOLL},
#endif
#ifdef SIGPWR
	{"PWR", SIGPWR},
#endif
#ifdef SIGEMT
	{"EMT", SIGEMT},
#endif
#ifdef SIGINFO
	{"INFO", SIGINFO},
#endif
#ifdef SIGSTKFLT
	{"STKFLT", SIGSTKFLT},
#endif
#ifdef SIGCLD
	{"CLD", SIGCLD},
#endif
#ifdef SIGLOST
	{"LOST", SIGLOST},
#endif
#ifdef SIGCANCEL
	{"CANCEL", SIGCANCEL},
#endif
#ifdef SIGTHAW
	{"THAW", SIGTHAW},
#endif
#ifdef SIGFREEZE
	{"FREEZE", SIGFREEZE},
#endif
#ifdef SIGLWP
	{"LWP", SIGLWP},
#endif
#ifdef SIGWAITING
	{"WAITING", SIGWAITING},
#endif
 	{NULL,-1},
};


int str_to_signal(char* signal) {
	int i;
	for (i = 0; sigstrnum[i].name != NULL; i++)
	  	if (!strcmp(signal, sigstrnum[i].name))
			return sigstrnum[i].signal;
	return -1;
}
char* signal_to_str(int signal) {
	int i;
	for (i  =0 ; sigstrnum[i].name != NULL; i++)
		if (signal == sigstrnum[i].signal)
			return sigstrnum[i].name;
	return ("SIGUNKNOWN");
}
int str_to_status(char* status) {
	int i;
	for (i = 0; procstatus[i].name != NULL; i++)
	  	if (!strcmp(status, procstatus[i].name))
			return procstatus[i].signal;
	return -1;
}
char* status_to_str(int status) {
	int i;
	for (i  =0 ; procstatus[i].name != NULL; i++)
		if (status == procstatus[i].signal)
			return procstatus[i].name;
	return ("STATUKNOWN");
}

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
	printf("%s\n", aux);
}

void doChangeVarPutenv(char* var, char* value) {
	/*char* aux;
	if ((aux=(char *)malloc(strlen(var)+strlen(value)+2))==NULL) {
		perror("malloc failed");
		return;
	}*/
	char aux[CHAR_MAX];
	strcpy(aux, var);
	strcat(aux, "=");
	strcat(aux, value);
	if (putenv(aux) != 0) return perror("Could not allocate new environment");
	printf("%s\n", aux);
}

void doExec(char* file, char* argv[], tShellState* ShellState) {
	int i = 0;
	pid_t PID;
	
	if (argv[i] != NULL) {
		while (argv[i+1] != NULL) i++;
		
		if (strcmp(argv[i], "&") == 0) {	// Execution in background

			if ((PID = fork()) == 0) {		// If PID == 0 we are working in the child process
		        if(execvp(file, argv) == -1) {
		            perror("Cannot execute program");
		            exit(EXIT_FAILURE);
		        }
        }
		    else {							// PID not 0 is the value of the pid of the child process
		    								// but it is returned in the parent process
		    	time_t raw_time = time(NULL);
				struct tm *tm_info;
				char time_buffer[TIME_BUFFER_MAX];
				tm_info = localtime(&raw_time); 
				strftime(time_buffer, sizeof(time_buffer), "%d/%m/%Y, %H:%M:%S", tm_info);
				
				int i = 0;
				char command[CHAR_MAX];
				snprintf(command, CHAR_MAX, "%s", file);
				while (argv[i] != NULL) {
					strcat(command, " ");
					strcat(command, argv[i]);
					i++;
				}
		    	tItemP process = DEFAULT_ITEM_P;
		    	strcpy(process.command, command);					// set command
		    	process.pid = PID;									// set pid
		    	strcpy(process.launch_time, time_buffer);			// set time
		    	
		    	insertItemP(&ShellState->ProcList, process);		// insert item
		    	
		        waitpid(PID, NULL, WNOHANG);
		        return;
		    }
		}
	}
	if (execvp(file, argv) == -1)
		return perror("Not executed");
}

void doDeleteTerminatedProcesses() {

}
void doDeleteSignaledProcesses() {

}


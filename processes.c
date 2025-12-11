#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/resource.h>
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
	char aux[MAX_PATH];
	
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
	char* aux;
	if ((aux=(char *)malloc(strlen(var)+strlen(value)+2))==NULL) {
		perror("malloc failed");
		return;
	}
	strcpy(aux, var);
	strcat(aux, "=");
	strcat(aux, value);
	if (putenv(aux) != 0) return perror("Could not allocate new environment");
	printf("%s\n", aux);
}

void doExec(char* file, char* argv[], tShellState* ShellState) {
	int i = 0, priority = 0;
	pid_t PID;
	bool background = false;
	
	while (argv[i] != NULL) {	// Parsing background and priority args
		if (argv[i][0] == '@') {
			int aux = i;
			priority = (int)strtol(&argv[i][1], NULL, 10);
			while (argv[aux] != NULL) {		// As the ars in argv are shifted 1 position to the left
											// we use continue so that is '&' is present gets evaluated too,
											// and thus we also avoid segmentation fault if no further args than '@'
				argv[aux] = argv[aux+1]; 
				aux++;
			}
			continue;
		}
		if (argv[i][0] == '&') {
			int aux = i;
			background = true;
			while (argv[aux] != NULL) {
				argv[aux] = argv[aux+1]; 
				aux++;
			}
			continue;
		}
		i++;
	}
	if (background) {	// Execution in background
		if ((PID = fork()) == 0) {		// If PID == 0 we are working in the child process
			if (setpriority(PRIO_PROCESS, 0, priority) == -1) perror("Error setting priority");
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
			
			char command[MAX_INPUT];
			strcpy(command, argv[0]);
			int i = 1;
			while (argv[i] != NULL) {
				if (argv[i][0] == '&' || argv[i][0] == '@' ) break;	// Stop adding command to buffer when encountered with @ or &
				strcat(command, " ");
				strcat(command, argv[i]);
				i++;
			}
			tItemP process = DEFAULT_ITEM_P;
			strcpy(process.command, command);					// set command
			process.pid = PID;									// set pid
			strcpy(process.launch_time, time_buffer);			// set time
		    waitpid(PID, &process.signal, WNOHANG);				// set signal value
			
			insertItemP(&ShellState->ProcList, process);		// insert item
			
		    return;
		}
	}
	// EXECUTION IN FOREGROUND
	if ((PID = fork()) == 0) {
		if (setpriority(PRIO_PROCESS, 0, priority) == -1) perror("Error setting priority");
	    if(execvp(file, argv) == -1) {
	        perror("Cannot execute program");
            exit(EXIT_FAILURE);
        }
    } else
        waitpid(PID, NULL, WUNTRACED);
}

void updateProcessList(tListP* L) {
	tPosP pos = firstP(*L);
	tItemP process;
	int result;
	
	while (pos != LNULL) {
		process = getItemP(*L, pos);
		if (process.status == FINISHED) {	// Process has terminated -> no possible status update
			pos = nextP(*L, pos);
			continue;
		}
		result = waitpid(process.pid, &process.signal, WNOHANG);
		if (result == 0) {
            // process is ACTIVE and no status update report by waitpid -> next element
        } else if (result == -1) {
            process.status = FINISHED;
        } else {
			if (WIFEXITED(process.signal)) {
				process.status = FINISHED;
				process.signal = WEXITSTATUS(process.signal);
			} else if (WIFSIGNALED(process.signal)) {
				process.status = FINISHED;
				process.signal = WTERMSIG(process.signal);
			} else if (WIFSTOPPED(process.signal)) {
				process.status = STOPPED;
				process.signal = WSTOPSIG(process.signal);
			} else if (WIFCONTINUED(process.signal)) {
		        process.status = ACTIVE;
		        process.signal = SIGCONT;
		    } else process.status = process.signal = -1;
		}
		updateItemP(L, pos, process);
		pos = nextP(*L, pos);
	}
}

void doDeleteTerminatedProcesses(tListP* L) {
	tPosP aux, pos = firstP(*L);
	tItemP process;
	while (pos != NULL) {
		process = getItemP(*L, pos);
		aux = pos;
		pos = nextP(*L, pos);
		if (process.status == FINISHED) {
			deleteAtPosP(L, aux);
		}
	}
}

void doDeleteSignaledProcesses(tListP* L) {
	tPosP aux, pos = firstP(*L);
	tItemP process;
	while (pos != NULL) {
		process = getItemP(*L, pos);
		if (process.status == SIGNALED) aux = pos;

		pos = nextP(*L, pos);
		deleteAtPosP(L, aux);
	}
}


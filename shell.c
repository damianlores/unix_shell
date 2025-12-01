#include <fcntl.h>
 
#include "shell.h"
#include "types.h"


int sliceChain(char* chain, char *slices[])
{
    size_t i=1;

    if ((slices[0]=strtok(chain," \n\t"))==NULL)
        return 0;
    while ((slices[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

void printPrompt() {
    printf("shell > ");
}

bool inputRead(char* input) {
	if (fgets(input, MAX_INPUT, stdin)==NULL) return false;
	if (strchr(input, '\n') == NULL) {
        int c;
        bool exceed = false;
        while ((c = getchar()) != '\n' && c != EOF) exceed = true; 	// Flush overflow buffer
        if (exceed) printf("Warning: Input buffer overflow! Behavior might be unexpected.\n");
    }
    return true;
}

void inputProcess(char* input , tShellState* ShellState) {

    char* args[MAX_INPUT/2];
    short unsigned int argc = sliceChain(input,args);

    if (argc == 0) return;

    tHistoricItem temp;

    strcpy(temp.cmd,input);

    if (!insertItemH(&ShellState->HistoricList, temp)) {
        printf("There has been a problem loading the command in memory\n");
        return;
    }

    if (!cmd_dispatcher(args, ShellState)) {
        printf("Invalid command.\n");
    }

}

void inherit_std_descriptors(tListF* OFList) {

    const char *names[] = { "standard input", "standard output", "standard error" };

	tOFilesItem item;

    for (int fd = 0; fd <= 2; fd++) {
    	if (fcntl(fd, F_GETFD) != -1) {
    	    item.fd = fd;
        	item.dup_of=-1;
        	strcpy(item.name, names[fd]);
        	strcpy(item.mode, "rw");
        
        	insertItemF(OFList, item);
    	}
    }
}

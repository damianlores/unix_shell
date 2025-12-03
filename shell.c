#include <fcntl.h>
 
#include "shell.h"
#include "types.h"
#include "file_system.h"


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

void inputProcess(char* input, tShellState* ShellState) {    
    char* args[MAX_INPUT/2];
    
    tHistoricItem temp;
    snprintf(temp.input, MAX_INPUT, "%s", input);

    if (sliceChain(input, args) == 0) return;
    
    temp.input[strlen(temp.input)-1] = '\0';
    if (!insertItemH(&ShellState->HistoricList, temp)) {
        printf("There has been a problem loading the command in memory\n");
        return;
    }

    if (!cmd_dispatcher(args, ShellState)) {
        printf("Invalid command.\n");
    }

}

void inherit_std_descriptors(tListF* OFList) {
	const char* std_descriptor_name[3] =
    	{"standard input", "standard output", "standard error"};
    	
	struct stat st;
	tOFilesItem item;
	char* flags[2];
		flags[0] = "O_RDWR";
		flags[1] = NULL;
		
    for (int fd = 0; fd <= 2; fd++) {
		if (fstat(fd, &st) == -1) { perror("Couldn't access standard descriptor"); return; };
		
    	item.fd = fd;
        item.dup_of = -1;
    	strcpy(item.name, std_descriptor_name[fd]);
    	copy_open_file_flags(item.mode, flags);
        
        insertItemF(OFList, item);
    }
}

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "commands.h"
#include "types.h"
#include "dynamic_list.h"
#include "shell.h"
#include "file_system.h"
#include "mem_management.h"
#include "processes.h"


#define DEFAULT_DUMP_SIZE 25


//Glogal variables (for mem)
int g1 = 111;
int g2 = 222;
int g3 = 333;

//Global variables not initizlized
int ng1;
int ng2;
int ng3;


tCommand commands[] = {
    {"authors", cmd_authors},
    {"getpid", cmd_getpid},
    {"chdir", cmd_chdir},
    {"getcwd", cmd_getcwd},
    {"date", cmd_date},
    {"hour", cmd_hour},
    {"historic", cmd_historic},
    {"open", cmd_open},
    {"close", cmd_close},
    {"dup", cmd_dup},
    {"listopen", cmd_listopen},
    {"infosys", cmd_infosys},
    {"help", cmd_help},
    {"exit", cmd_exit},
    {"quit", cmd_exit},
    {"bye", cmd_exit},
    // P1
    {"create", cmd_create},
    {"setdirparams", cmd_setdirparams},
    {"getdirparams", cmd_getdirparams},
    {"dir", cmd_dir},
    {"erase", cmd_erase},
    {"delrec", cmd_delrec},
    {"lseek", cmd_lseek},
    {"writestr", cmd_writestr},
    // P2
    {"malloc", cmd_malloc},
    {"mmap", cmd_mmap},
    {"shared", cmd_shared},
    {"free", cmd_free},
    {"mem", cmd_mem},
    {"memfill", cmd_memfill},
    {"memdump", cmd_memdump},
    {"readfile", cmd_readfile},
    {"writefile", cmd_writefile},
    {"read", cmd_read},
    {"write", cmd_write},
    {"recurse", cmd_recurse},
    // P3
    {"uid", cmd_uid},
    {"envvar", cmd_envvar},
    {"showenv", cmd_showenv},
    {"fork", cmd_fork},
    {"exec", cmd_exec},
    {"jobs", cmd_jobs},
    {"del_jobs", cmd_deljobs},
    {NULL, NULL}
};


    	
void print_invalid_usage() { 
	printf("Wrong usage. Run with '--help' to show help.\n");
}
void print_too_few_arguments() { 
	printf("Too few arguments. Run with '--help' to show help.\n");
}
void print_invalid_args(char* args) { 
	printf("Invalid arguments: %s\n", args);
}


bool cmd_dispatcher(char *args[], tShellState* ShellState) {
    for (size_t i = 0; commands[i].name != NULL; i++) {
        if (strcmp(args[0], commands[i].name) == 0) {
            commands[i].func(args, ShellState);
            return true;
        }
    }
    return false;
}




// ======================== P0 FUNCTION (NON-HELP FUNCTIONS) ========================




void cmd_authors(char *args[], tShellState *ShellState) {
    bool show_names = false, show_logins = false;
    for (int i = 1; args[i]; i++) {
        if (strcmp(args[i], "-l") == 0) show_logins = true;
    	else if (strcmp(args[i], "-n") == 0) show_names = true;
    	else if ((args[1]!=NULL) && strcmp(args[1], "--help") == 0) return help_authors();
    	else return print_invalid_usage();
    } 
    if (!show_names && !show_logins)
        show_names = show_logins = true;
    if (show_logins) {
    	const char* logins[] = {"d.lores@udc.es", "m.pazos2@udc.es"};
        for (int i = 0; i < 2; i++)
            puts(logins[i]);
    }
    if (show_names) {
        const char *names[] = {"Damián", "Marcos"};
        for (int i = 0; i < 2; i++)
            puts(names[i]);
    }
}

void cmd_getpid(char *args[], tShellState *ShellState) {
    if (args[1]==NULL) {
        printf("Shell's pid %d\n", getpid());
    } else if (strcmp(args[1],"-p")==0) {
        printf("Shell's father pid: %d\n", getppid());
    } else if (strcmp(args[1], "--help") == 0) return help_getpid();
    else print_invalid_usage();
}

void cmd_chdir(char *args[], tShellState *ShellState) {
    if ((args[1] != NULL) && (strcmp(args[1], "--help") == 0)) return help_chdir();
    
    char cwd[PATH_MAX];
    
    if (getcwd(cwd, sizeof(cwd)) != NULL) printf("Current directory: %s\n", cwd);
    
    if (chdir(args[1]) == 0) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            printf("%s\n", cwd);
    } else perror(args[1]);
}

void cmd_getcwd(char *args[], tShellState *ShellState) {
	if ((args[1] != NULL) && strcmp(args[1], "--help") == 0) return help_getcwd();
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("Current directory: %s\n", cwd);
}

void cmd_date(char *args[], tShellState *ShellState) {
    time_t t = time(NULL);
    struct tm *tm_read = localtime(&t);

    char date[11];
    strftime(date, sizeof(date), "%d/%m/%Y", tm_read);
    char hour[9];
    strftime(hour, sizeof(hour), "%H:%M:%S", tm_read);

    bool show_date = false;
    bool show_time = false;

    for (int i = 1; args[i] != NULL; i++) {
        if (strcmp(args[i], "-d") == 0) show_date = true;
        if (strcmp(args[i], "-t") == 0) show_time = true;
        if (strcmp(args[1], "--help") == 0) return help_date();
    }

    if (!show_date && !show_time) {
        show_date = show_time = true;
    }

    if (show_date) printf("%s\n", date);
    if (show_time) printf("%s\n", hour);
}

void cmd_hour(char *args[], tShellState *ShellState) {
	if ((args[1]!=NULL) && strcmp(args[1], "--help") == 0) return help_hour();
	
    time_t t = time(NULL);
    struct tm *tm_read = localtime(&t);
    char time[9];
    strftime(time, sizeof(time), "%H:%M:%S", tm_read);

    printf("%s\n", time);
}

void cmd_historic(char *args[], tShellState *ShellState) {
    // Given n to the printLastNH function is -1 if no flags are received, as a result
    // the whole list is printed.
    if (args[1]==NULL) printLastNH(ShellState->HistoricList, -1);
    
    else if (strcmp(args[1], "--help") == 0) return help_historic();
    
    else if (strcmp(args[1],"-count")==0) printf("Historic number of commands: %ld\n", countH(ShellState->HistoricList));
    
    else if (strcmp(args[1],"-clear")==0) clearListH(&ShellState->HistoricList);
    
    // If none of the other flags was a match, start parsing by char '-'
    else if (args[1][0] == '-') {
        // Format: "-<number>"
        ssize_t n = (ssize_t)strtoul(args[1] + 1 /* this skips the '-' input*/ , NULL, 10);
        if (n <= 0) { 
        	errno = EINVAL;
        	perror("Invalid input");
        	return;
        }
        printLastNH(ShellState->HistoricList, n);
    }
    else {
        size_t id = strtoul(args[1], NULL, 10);
        if (countH(ShellState->HistoricList) <= id) {
        	errno = EFAULT;
        	perror("Invalid input");
        	return;
        }
            
    // If every conditional was unsuccesful we can finally execute the n-th command of historic
    tPosH temp = findItemH(ShellState->HistoricList,id);
    inputProcess(getItemH(ShellState->HistoricList,temp).input, ShellState);
    }
}

void cmd_open(char *args[], tShellState* ShellState) {
	// no args given -> list of open files
    if (args[1] == NULL) { cmd_listopen(args, ShellState); return; }
	// error if no open mode was input
    if (args[2] == NULL) { print_too_few_arguments(); return; }

    int mode = 0;
    modes_to_flags(args+2, &mode);
	// error if input matches no flag
    int fd = open(args[1], mode, 0777);
    if (fd == -1) return perror("Imposible to open file");

    tOFilesItem item;
    item.fd = fd;
    item.dup_of=-1;
    snprintf(item.name, PATH_MAX, "%s", args[1]);
    flags_to_str_arr(mode, item.mode);
    
    if (insertItemF(&ShellState->OFList, item))
        printf("Added entry %d to open files list\n",fd);
    else {
    	printf("Error: couldn't insert open file in list\n");
    	close(fd);
    }
}

void cmd_close(char *args[], tShellState *ShellState) {
    if (args[1] == NULL) return print_too_few_arguments();
    if (strcmp(args[1], "--help") == 0) return help_close();

    ssize_t fd = (ssize_t)strtol(args[1], NULL, 10);
    if (close(fd) == -1) return perror("Imposible to close descriptor'");

    deleteAtPosF(&ShellState->OFList,
    	findItemF(ShellState->OFList, fd)); // No need to check if file descriptor exists as of thanks to beforehand close syscall
}


void cmd_dup(char *args[], tShellState *ShellState) {
    if (args[1] == NULL) return print_too_few_arguments();
	if (strcmp(args[1], "--help") == 0) return help_dup();

    int newfd, oldfd = strtol(args[1], NULL, 10);
    newfd = dup(oldfd);
    
    if (newfd == -1) return perror("Imposible to make dup of descriptor");

    tPosF pos = findItemF(ShellState->OFList, oldfd);
    tOFilesItem oldItem = getItemF(ShellState->OFList, pos);
    
    tOFilesItem newItem;
    newItem.fd = newfd;
    newItem.dup_of = oldfd;
    strcpy(newItem.name, oldItem.name);
    copy_open_file_flags(newItem.mode, oldItem.mode);

    if (!insertItemF(&ShellState->OFList, newItem)) {
        printf("Error: could not insert duplicated fd into list\n");
        close(newfd);
    }
}

void cmd_infosys(char *args[], tShellState *ShellState) {
	if ((args[1] != NULL) && strcmp(args[1], "--help") == 0) return help_infosys();
    struct utsname info;
    if (uname(&info) == -1)
        return perror("Imposible to retrieve system info'");

    printf("%s (%s), OS: %s-%s-%s\n",info.nodename, info.machine, info.sysname, info.release, info.version);
}

void cmd_listopen(char *args[], tShellState *ShellState) {
	if ((args[1] != NULL) && strcmp(args[1], "--help") == 0) return help_listopen();
	
    printListF(ShellState->OFList);
}


void cmd_exit(char *args[], tShellState *ShellState) {
    if ((args[1] != NULL) && strcmp(args[1], "--help") == 0) return help_exit();
    clearListH(&ShellState -> HistoricList);
    clearListF(&ShellState -> OFList);
    freeBlocks(&ShellState -> MemList);
    clearListP(&ShellState -> ProcList);
    printf("Ending shell...\n");
    exit(0);
}




// ======================== P1 FUNCTIONS (NON-HELP FUNCTIONS) ========================




void cmd_create(char *args[], tShellState *ShellState) {
    //Checks the -f argument is passed
    if (args[1] == NULL) print_invalid_usage();
    	
    if (strcmp(args[1], "--help") == 0) return help_create();
    	
    if (strcmp(args[1], "-f") == 0) {
		if (args[2] == NULL) return print_invalid_usage();
	    
		const char* filename = args[2];   //Tries to open the file in read/write mode
	    FILE* fp = fopen(filename, "w+");
	    if (!fp) 
	        return perror("Imposible to create file");
	    
	    fclose(fp);
	    return;
	}
    
    //Case -f not passed
    const char* dirname = args[1];
    // Creates the directory with permissions read/write/execute
    
	if (mkdir(dirname, 0777) != 0)
        perror("Imposible to create directory");
}


void cmd_setdirparams(char *args[], tShellState *ShellState) {
    if (args[1] != NULL && (strcmp(args[1], "--help")) == 0) return help_setdirparams();
    
	size_t i;
	for (i = 1 ; args[i] != NULL ; i++) {
		if (strcmp(args[i], "long") == 0)
            ShellState->dirParams.long_format = true;
        else if (strcmp(args[i], "short") == 0)
            ShellState->dirParams.long_format = false;
        else if (strcmp(args[i], "hid") == 0)
            ShellState->dirParams.show_hid = true;
        else if (strcmp(args[i], "nohid") == 0)
            ShellState->dirParams.show_hid = false;
        else if (strcmp(args[i], "link") == 0)
            ShellState->dirParams.link = true;
        else if (strcmp(args[i], "nolink") == 0)
            ShellState->dirParams.link = false;
        else if (strcmp(args[i], "reca") == 0)
            ShellState->dirParams.rec_mode = REC_AFTER;
        else if (strcmp(args[i], "recb") == 0)
            ShellState->dirParams.rec_mode = REC_BEFORE;
        else if (strcmp(args[i], "norec") == 0)
            ShellState->dirParams.rec_mode = REC_OFF;
    }
        //Displays the current settings if no parameters were provided
	if (i == 1) cmd_getdirparams(args, ShellState);
}


void cmd_getdirparams(char *args[], tShellState *ShellState) {
    char buffer[64];
    
    if (args[1] != NULL && (strcmp(args[1], "--help")) == 0) return help_getdirparams();
    
    strcpy(buffer, "Listing: ");
    strcat(buffer, ShellState->dirParams.long_format ? "Long " : "Short ");
    strcat(buffer, ShellState->dirParams.link ? "with links, " : "no links, ");
    
    
    //Append information about the recursive mode
    switch(ShellState->dirParams.rec_mode) {
        case REC_OFF :
            strcat(buffer, "non-recursive ");
            break;
        case REC_BEFORE :
            strcat(buffer, "recursive (before) ");
            break;
        case REC_AFTER :
            strcat(buffer, "recursive (after) ");
            break;
    }
    
    
    //Append information about the hidden files
    strcat(buffer, ShellState->dirParams.show_hid ? "with hidden files\n" : "\n");
    
    printf("%s", buffer);

}	

void cmd_dir(char *args[], tShellState *ShellState) {

    //Show the curent directory if there are no args
    if (args[1] == NULL) {
        char path[PATH_MAX];
        
       	if (getcwd(path, sizeof(path)) == NULL) return perror("Imposible to access directory");
       	
        return print_dir(path, ShellState->dirParams);
    }
    
    if (strcmp(args[1], "--help") == 0) return help_dir();
    
    //When the arg is -d
    if (strcmp(args[1], "-d") == 0) {
        if (args[2] == NULL) return print_invalid_usage();
        
        for (int i = 2; args[i] != NULL; i++) {
            if (isDir(args[i])) {
                print_dir(args[i], ShellState->dirParams);
            } else {
                print_file(args[i], ShellState->dirParams);
            }
        }
        return;
    }
    // Without -d
    for (int i = 1; args[i] != NULL; i++) {
        print_file(args[i], ShellState->dirParams);
    }
}


void cmd_erase(char *args[], tShellState *ShellState) {
    if (args[1] == NULL) return print_invalid_usage();
    if (strcmp(args[1], "--help") == 0) return help_erase();
    
    for (int i = 1; args[i] != NULL; i++) {
        //Check if it is a directory
        if ( isDir(args[i]) ) {
            //Removes only if empty
            if (rmdir(args[i]) == -1)
                perror(args[i]);
            else {
                printf("Directory %s deleted\n", args[i]);
         	}
         	continue;   //Check the next argument
        }

        struct stat info;
        
        if (lstat(args[i], &info) == -1) {
            perror(args[i]);
            continue;
        }
        else {
        	if (info.st_size == 0) {
        	//Deletes the empty file
            	if (unlink(args[i]) == -1)
                	perror(args[i]);
	            else
	            	printf("File %s deleted\n", args[i]);
        	}
        	else {
        		printf("File is not empty\n");
			}        
        }
    }
}


void cmd_delrec(char *args[], tShellState *ShellState) {
    if (args[1] == NULL) return print_invalid_usage();
	if (strcmp(args[1], "--help") == 0) return help_delrec();
	
    for (int i = 1; args[i] != NULL; i++) {
        struct stat st;

        if (lstat(args[i], &st) == -1) {
            perror(args[i]);
            continue;
        }
		
        if (isDir(args[i])) {
            delete_dir(args[i]);
        }
    
        else {
            if (unlink(args[i]) == -1) {
                perror(args[i]);
            }
        }
    }
}


void cmd_lseek(char *args[], tShellState *ShellState) {
    if (args[1] == NULL) return print_invalid_usage();
    if (strcmp(args[1], "--help") == 0) return help_lseek();
	if (args[2] == NULL || args[3] == NULL) return print_too_few_arguments();
    
    ssize_t fd = (ssize_t)strtol(args[1], NULL, 10);
    
    if (fd < 0) return print_invalid_args(args[1]);
    
    long off = strtol(args[2], NULL, 10);
    int where;   // Used like a reference point

    if (strcmp(args[3], "SEEK_SET") == 0) where = SEEK_SET;    //Offset from the beginning of the file
    else if (strcmp(args[3], "SEEK_CUR") == 0) where = SEEK_CUR;   //Offset from the current file position 
    else if (strcmp(args[3], "SEEK_END") == 0) where = SEEK_END;   // Offset from the end of the file
    else return print_invalid_args(args[3]);
    
    tPosF pos = findItemF(ShellState->OFList, fd);
    if (pos == LNULL) { printf("Descriptor %ld not found in open files list\n", fd); return; }
    
    // Perform the seek operation on the file descriptor
    off_t new_offset = lseek(fd, off, where);
    
    if (new_offset == (off_t)-1) return perror("Error setting offset"); // (off_t)-1 is the value returned by lseek on error
    
    printf("New offset for descriptor %ld is %ld\n", fd, (long)new_offset);

}


void cmd_writestr(char *args[], tShellState *ShellState) {
    if (args[1] == NULL) return print_invalid_usage();
	if (strcmp(args[1], "--help") == 0) return help_writestr();
	if (args[2] == NULL) return print_too_few_arguments();
	
    ssize_t fd = (ssize_t)strtol(args[1], NULL, 10);
    
    if (fd < 0) return print_invalid_args(args[1]);

    //Initialization off an empty buffer
    char str[MAX_INPUT] = "";
	
    for (int i = 2; args[i] != NULL; i++) {
        strcat(str, args[i]);
        if (args[i + 1] != NULL)    // Add space between words
            strcat(str, " ");
    }
    ssize_t len = strlen(str);
    ssize_t written = write(fd, str, len);	// Returns the number of bytes successfully written (or -1 if error)
    
    if (written == -1) return perror("writestr");

    printf("Written at descriptor %ld: %s\n", fd, str);
}





// ======================== P2 FUNCTIONS (NON-HELP FUNCTIONS) ========================





void cmd_malloc(char *args[], tShellState *ShellState) {
	// No args passed -> Print list of allocated blocks
    if (args[1] == NULL) {
    	printf("****** List of malloc blocks assigned to process %d\n", getpid());
        printListTypeM(ShellState->MemList, MEM_MALLOC);
        return;
    }   	
	if (strcmp(args[1], "--help") == 0) return help_malloc();

    if (strcmp(args[1], "-free") == 0) {
    	// malloc -free needs block size argument:
		if (args[2] == NULL) return print_too_few_arguments();
		
		ssize_t n = (ssize_t)strtol(args[2], NULL, 10);
		if (errno == ERANGE) return perror(args[2]); 	// If overflow occurs, strtol returns  LONG_MAX,
														// so we make use of errno number to stop function
		tPosM pos = findMallocItemM(ShellState->MemList, n);
		if (pos == LNULL) { printf("Block of size %ld not found.\n",n); return; }
		
		tItemM item = getItemM(ShellState->MemList, pos);
		freeMalloc(&ShellState->MemList, pos);
		
		printf("Freed block of size %ld at address %p\n", n, item.addr);
		return;
    }
	
	long unsigned int n = strtoul(args[1], NULL, 10);    
    if (n == 0) { 
    	errno = EINVAL;
    	return perror("Could not allocate memory");
    }

    void *ptr = malloc(n);
	if (!ptr) return perror("Could not allocate memory");
	
	time_t raw_time = time(NULL);
    struct tm *tm_info;
	char time_buffer[TIME_BUFFER_MAX];
	tm_info = localtime(&raw_time); 
	strftime(time_buffer, sizeof(time_buffer), "%a %b %d %H:%M:%S %Y", tm_info);
    	
    tItemM item = DEFAULT_ITEM_M;
    item.addr = ptr;
    item.size = n;
    item.alloc_mode = MEM_MALLOC;
    strcpy(item.time, time_buffer);
    insertItemM(&ShellState->MemList, item);

    printf("Allocated %ld bytes at %p\n", n, ptr);
    
}

void cmd_mmap(char *args[], tShellState *ShellState) {
	// No args shows list of mapped files
    if (args[1] == NULL) {
		printf("****** List of malloc blocks assigned to process %d:\n", getpid());
    	printListTypeM(ShellState->MemList, MEM_MMAP); 
    	return;
    }   	
    if (strcmp(args[1], "--help") == 0) return help_mmap();
    
    if (strcmp(args[1], "-free") == 0) {
    	// Command called with -free needs filename to unmmap
        if (args[2] == NULL) return print_too_few_arguments();
        
        char *filename = args[2];
        tPosM pos = findMMapItemM(ShellState->MemList, filename);
        // If file is mapped and -free was set, call aux unmapping function
        if (pos != LNULL) { 
        	freeMMap(&ShellState->MemList, pos); 
        	return; 
    	}
    	errno = ENOENT;
        return perror("Could not unmap file");
    }
    // No permissions given? Return:
    if (args[2] == NULL) return print_too_few_arguments();
    // File is mapped, args are present -> call mapping aux function:
    doMmap(args, &ShellState->OFList, &ShellState->MemList);
}

void cmd_mem(char *args[], tShellState *ShellState) {
    int l1, l2, l3;
    static int si1 = 11, si2 = 22, si3 = 33;
    static int sni1, sni2, sni3;

    if (args[1] == NULL) return print_invalid_usage();
    if (strcmp(args[1], "--help") == 0) return help_mem();
    
    bool vars = false, blocks = false, funcs = false;
        
    if (strcmp(args[1], "-funcs") == 0) funcs = true;
    else if (strcmp(args[1], "-vars") == 0) vars = true;
    else if (strcmp(args[1], "-blocks") == 0) blocks = true;
    else if (strcmp(args[1], "-all") == 0) funcs = vars = blocks = true;
    else if (args[1] && strcmp(args[1], "-pmap") == 0) return doPmap();
    else return print_invalid_usage();
    
    if (funcs) {
    	printf("Program functions   %p    %p    %p\n", cmd_malloc, cmd_read, cmd_write);
		printf("Library functions   %p    %p    %p\n", printf, malloc, strcpy);
	}
    if (vars) {
     	printf("Local variables    %p   %p    %p\n", &l1, &l2, &l3);
        printf("Global variables    %p    %p    %p\n", &g1, &g2, &g3);
        printf("Var (N.I.) global   %p    %p    %p\n", &ng1, &ng2, &ng3);
        printf("Static variables    %p    %p    %p\n", &si1, &si2, &si3);
        printf("Var (N.I.) static   %p    %p    %p\n", &sni1, &sni2, &sni3);
    }  
	if (blocks) {
		printf("****** List of blocks assign to the process %d\n", getpid());
		printListM(ShellState->MemList);
	}
}

void cmd_free(char *args[], tShellState *ShellState) {
	if (args[1] == NULL) return print_invalid_usage();
	if (strcmp(args[1], "--help") == 0) return help_free();
    
    void *p = stringToPointer(args[1]);
    tPosM pos = findItemM(ShellState->MemList, p);
    if (pos == LNULL) return print_invalid_args(args[1]);
    
    tItemM block = getItemM(ShellState->MemList, pos);
    
    if (block.alloc_mode == MEM_MALLOC) {
    	freeMalloc(&ShellState->MemList, pos);
    }
    else if (block.alloc_mode == MEM_MMAP) {
		freeMMap(&ShellState->MemList, pos);
    }
    else if (block.alloc_mode == MEM_SHARED) {
		freeShared(&ShellState->MemList, pos);
    }
}

void cmd_memfill(char *args[], tShellState *ShellState) {
    if (!args[1]) return print_invalid_usage();
    if (strcmp(args[1], "--help") == 0) return help_memfill();
    if(!args[2] || !args[3]) return print_too_few_arguments();
    
    void *addr = stringToPointer(args[1]);
    if (!addr) return perror("Invalid pointer");
    
    ssize_t cont = (ssize_t)strtol(args[2], NULL, 10);
    if (cont <= 0) return print_invalid_args(args[2]);
    
    unsigned char ch = (unsigned char)args[3][0];
    
    fillMem(addr, cont, ch);
    printf("Filled %zu bytes at %p with character '%c'\n", cont, addr, ch);   
}

void cmd_memdump(char *args[], tShellState *ShellState) {
    if (!args[1]) return print_invalid_usage();
    if (strcmp(args[1], "--help") == 0) return help_memdump(); 
    if(!args[2]) return print_too_few_arguments(); 
    
    void *addr = stringToPointer(args[1]);
    if (!addr) return perror("Invalid pointer"); 
    
    ssize_t cont = (ssize_t)strtoul(args[2], NULL, 10);
    if (cont <= 0) return print_invalid_args(args[2]);
    
    unsigned char *paddr = (unsigned char *)addr; // Convert so we can iterate

    for (size_t offset = 0; offset < cont; offset += DEFAULT_DUMP_SIZE) {
    
        size_t block_size = (cont - offset > DEFAULT_DUMP_SIZE) ? DEFAULT_DUMP_SIZE : cont - offset;
      
        for (size_t i = 0; i < block_size; i++) {
              unsigned char c = paddr[offset + i];
              if (c == '\n')
                  printf("\\n ");
              else if (c == '\t')
                  printf("\\t ");
              else if (c == '\r')
                  printf("\\r ");
              else if (isprint(c))
                  printf("%c  ", c);
              else
                  printf(" ");
          }
          
          printf("\n");
          
          for (size_t i = 0; i < block_size; i++) {
            printf("%02X ", paddr[offset + i]);
        }
        printf("\n");
    }
}

void cmd_readfile(char *args[], tShellState* ShellState) {
    if (args[1] == NULL) return print_too_few_arguments(); 
    if (strcmp(args[1], "--help") == 0) return help_readfile(); 

    void *p = stringToPointer(args[2]);
    if (p == NULL) return perror("Invalid pointer"); 
    
    if (!findItemM(ShellState->MemList, p)) { 
    	errno = ENOENT;
    	return perror("Could not access file");
    }
    	
	ssize_t cont = -1;
	if (args[2] != NULL) cont = (ssize_t)strtol(args[2], NULL, 10);

    ssize_t bytes_read = readFile(args[1], p, cont);
    if (bytes_read == -1)
        perror("Impossible to read file");
    else
        printf("Read %lld bytes of %s into %p\n", (long long)bytes_read, args[0], p);
}

void cmd_writefile(char* args[], tShellState* ShellState) {
    if (!args[1]) return print_too_few_arguments();
    if (strcmp(args[1], "--help") == 0) return help_writefile();
    if(!args[2] || !args[3]) return print_too_few_arguments(); 

    void *p = stringToPointer(args[2]);
    if (p == NULL) return perror("Invalid pointer");

    ssize_t cont = (ssize_t)strtol(args[2], NULL, 10);
    if (cont <= 0) return print_invalid_args(args[2]);
    
    ssize_t bytes_write = writeFile(args[0], p, cont);
    if (bytes_write == -1)
        perror("Impossible to write file\n");
    else
        printf("Write %lld bytes of %p into %s\n", (long long)bytes_write, p, args[0]);
}

void cmd_read(char* args[], tShellState* ShellState) {
    if (!args[1]) return print_too_few_arguments();
    if (strcmp(args[1], "--help") == 0) help_read();
    if(!args[2] || !args[3]) return print_too_few_arguments(); 

    int fd = strtol(args[1], NULL, 10);
    tPosF pos = findItemF(ShellState->OFList, fd);
    if (pos == LNULL) { 
    	errno = ENOENT;
    	return perror("Could not access descriptor");
    }
    
    void *p = stringToPointer(args[2]);
    if (!p) return perror("Invalid pointer");
    
    if (!findItemM(ShellState->MemList, p)) return print_invalid_args(args[1]); 
    
    ssize_t cont = (ssize_t)strtol(args[3], NULL, 10);
    if (cont <= 0) return print_invalid_args(args[3]); 

    ssize_t bytes_read = read(fd, p, cont);
    if (bytes_read == -1) return perror("Error reading file"); 

    printf("Read %zd bytes from descriptor %d into address %p\n", bytes_read, fd, p);
}

void cmd_write(char* args[], tShellState* ShellState) {
    if (!args[1]) return print_too_few_arguments();
    if (strcmp(args[1], "--help") == 0) return help_write(); 
    if(!args[2] || !args[3]) return print_too_few_arguments(); 
    
    ssize_t fd = (ssize_t)strtol(args[3], NULL, 10);
    if (fd < 0) return print_invalid_args(args[1]); 
    tPosF pos = findItemF(ShellState->OFList, fd);
    if (pos == LNULL) {
    	errno = ENOENT;
    	return perror("Could not access descriptor");
    }
    
    void *p = stringToPointer(args[2]);
    if (!p) { perror("Invalid pointer"); return; }
    
    ssize_t cont = (ssize_t)strtol(args[3], NULL, 10);
    if (cont <= 0) { print_invalid_args(args[3]); return; }

    ssize_t bytes_write = write(fd, p, cont);
    if (bytes_write == -1) return perror("Error writting file");

    printf("Write %zd bytes from descriptor %ld into address %p\n", bytes_write, fd, p);
}

void cmd_recurse(char *args[], tShellState *ShellState) {
    if (!args[1]) return print_too_few_arguments();
    
    if (strcmp(args[1], "--help") == 0) return help_recurse();
    
    ssize_t n = strtol(args[1], NULL, 10);
    if (n <= 0) return print_invalid_args(args[1]); 

    recursive(n);
}

void cmd_shared(char *args[], tShellState *ShellState) {
	// Using code from help code
	// No args -> print shared memory blocks list
	if (args[1]==NULL) { 
		printf("****** List of shared blocks assigned to process %d\n", getpid());
		printListTypeM(ShellState->MemList, MEM_SHARED);
		return;
	}
	
	if (strcmp(args[1], "--help") == 0) return help_shared();
	
	key_t key;
	
	if (strcmp(args[1], "-create") == 0) {
	
		if (args[2]==NULL || args[3]==NULL) return print_too_few_arguments();

		ssize_t size;
		void *p;
		
	   	key = (key_t) strtoul(args[2], NULL, 10);
		size = (ssize_t) strtoul(args[3], NULL, 10);
		
		if (size<=0) return print_invalid_args(args[3]);
		
		if ((p = doObtainMemoryShmget(&ShellState->MemList, key, size)) != NULL) {
			printf ("Allocated %lu bytes in %p\n",(unsigned long) size, p);
			return; 
		}
		else {
			printf("Impossible to allocate key shared memory %lu: %s\n",(unsigned long) key, strerror(errno));
			return;
		}
	} else if (strcmp(args[1], "-free") == 0) {
		if (args[2] == NULL) 
			return print_invalid_usage();
		else {
			key = strtoul(args[2], NULL, 10);
			tPosM pos = findSharedItemM(ShellState->MemList, key); // Retrieves position in list out of shared mem key
    		if (pos == LNULL) { 
    			errno = ENOENT;
        		return perror("Could not free memory");
        	}
        	freeShared(&ShellState->MemList, pos);
		}
	} else if (strcmp(args[1], "-delkey") == 0) {
		if (args[2] == NULL) 
			return print_invalid_usage(); 
		else
			return doDeleteKeyShared(strtoul(args[2], NULL, 10)); 
	} else doSharedAttach(args, &ShellState->MemList);

}




// ======================== P3 FUNCTIONS (NON-HELP FUNCTIONS) ========================




void cmd_uid(char *args[], tShellState *ShellState) {
	struct passwd* info;
	uid_t target_uid;
	// Input is only command or -get -> show real and effective UID
	if (args[1] == NULL || strcmp(args[1],"-get") == 0) {
		if ( ( info = getpwuid( getuid() ) ) == NULL) // Access real UID info
			return perror("Could not access real UID information");
		
		printf("Real credential: %d (%s)\n", info->pw_uid, info->pw_name);
		
		if ((info = getpwuid( geteuid() )) == NULL) 	// Access effective UID info
			return perror("Could not access effective UID information");
			
		printf("Effective credential: %d (%s)\n", info->pw_uid, info->pw_name);	
	}
	else if (strcmp(args[1], "--help") == 0) return help_uid();
	else if (strcmp(args[1], "-set") == 0) {	// Input is -set command
		if (args[2] == NULL) return print_too_few_arguments();
		
		if (strcmp(args[2], "-l") == 0) {
			if (args[3] == NULL) return print_too_few_arguments();
			
			if ( ( info = getpwnam(args[3]) ) == NULL)	// Get info of the input login name
				return perror("Could not retrieve login data");
			// If no failure, set target UID
			target_uid = info->pw_uid;
		} else {
		// No -l given, and entry not NULL -> set target UID normally
		target_uid = strtol(args[2], NULL, 10);
		} 
		if (setuid(target_uid) == -1)	// Set UID
			perror("Could not set uid");
		return; // Return both on success and on error
	}
	else print_invalid_args(args[1]);
}

void cmd_envvar(char *args[], tShellState *ShellState) {
	if (args[1] == NULL) return doShowEnvironment(ShellState->env, "env");

	if (strcmp(args[1], "-show") == 0) {
		if (args[2] == NULL)
			return doShowEnvironment(ShellState->env, "env");
		else {
			int var_index;
			if ((var_index = doSearchVariable(ShellState->env, args[2])) == -1)
				return perror("Could not find variable in environment");

			printVarArg3(ShellState->env[var_index]);
			printVarEnv(var_index);
			printVarGetenv(args[2]);
		}
	}
	else if (strcmp(args[1], "-change") == 0) {
		if (!args[2] || !args[3] || !args[4]) return print_too_few_arguments();
		if (args[2][0] != '-') return print_invalid_usage();
		else switch(args[2][1]) {
			case 'a': return doChangeVar(ShellState->env, args[3], args[4]);
			case 'e': return doChangeVar(environ, args[3], args[4]);;
			case 'p': return doChangeVarPutenv(args[3], args[4]);
			default: return print_invalid_args(args[2]);
		}
	}
}

void cmd_showenv(char *args[], tShellState *ShellState) {
	if (args[1] != NULL) {
		if (strcmp(args[1], "-environ") == 0)
			return doShowEnvironment(environ, "environ");
		else if (strcmp(args[1], "-addr") == 0) {
			printf("environ:\t%p (stored in %p)\n"
					"main arg3:\t%p (stored in %p)\n",
					environ, &environ,
					ShellState->env, &ShellState->env);
			return;
		}
	}
	else return doShowEnvironment(ShellState->env, "env");
}

void cmd_fork(char *args[], tShellState *ShellState) {
	if (args[1] != NULL && (strcmp(args[1],"--help") == 0))
		return help_fork();
	pid_t pid;
	if ((pid = fork()) == 0) {
		clearListP(&ShellState->ProcList);
		printf("Executing process %d\n", getpid());
	}
	else if (pid != -1)
		waitpid(pid, NULL, 0);
}

void cmd_exec(char *args[], tShellState *ShellState) {
	if (args[1] == NULL) return print_invalid_usage(); 
	if (strcmp(args[1],"--help") == 0) return help_exec();	

	if (execvp(args[1], args+2) == -1)
		return perror("execvp failure");
}

void cmd_jobs(char *args[], tShellState *ShellState) {
	if ( (args[1] != NULL) && (strcmp(args[1], "--help") == 0) ) return help_jobs();
	printListP(ShellState->ProcList);
}

void cmd_deljobs(char *args[], tShellState *ShellState) {
	if (args[1] == NULL) return printListP(ShellState->ProcList);
	if (strcmp(args[1], "--help")) return help_deljobs();
	if (strcmp(args[1], "-term")) return doDeleteTerminatedProcesses();
	if (strcmp(args[1], "-sig")) return doDeleteSignaledProcesses();
	print_invalid_args(args[1]);
}



// ======================== HELP FUNCTIONS ========================





void cmd_help(char *args[], tShellState *ShellState) {
    if (args[1]==NULL) {
        help_dispatcher("help");
        return;
    }
    help_dispatcher(args[1]);
}
tHelp helps[] = {
    {"authors",help_authors},
    {"getpid",help_getpid},
    {"chdir",help_chdir},
    {"getcwd",help_getcwd},
    {"date",help_date},
    {"hour",help_hour},
    {"historic",help_historic},
    {"open",help_open},
    {"close",help_close},
    {"dup",help_dup},
    {"listopen",help_listopen},
    {"infosys",help_infosys},
    {"exit",help_exit},
    {"help",help_help},
    // P1 HELP FUNCTIONS
    {"create",help_create},
    {"setdirparams",help_setdirparams},
    {"getdirparams",help_getdirparams},
    {"dir",help_dir},
    {"erase",help_erase},
    {"delrec",help_delrec},
    {"lseek",help_lseek},
    {"writestr",help_writestr},
    //P2 HELP FUNCTIONS
    {"malloc", help_malloc},
    {"mmap", help_mmap},
    {"shared", help_shared},
    {"free", help_free},
    {"mem", help_mem},
    {"memfill", help_memfill},
    {"memdump", help_memdump},
    {"readfile", help_readfile},
    {"writefile", help_writefile},
    {"read", help_read},
    {"write", help_write},
    {"recurse", help_recurse},
    //P3 HELP FUNCTIONS
    {"uid", help_uid},
    {"envvar", help_envvar},
    {"showenv", help_showenv},
    {"fork", help_fork},
    {"jobs", help_jobs},
    {"deljobs", help_deljobs},
    {NULL,help_help},
};
void help_dispatcher(char *args) {
    unsigned short int i;
    for (i = 0; helps[i].name != NULL; i++) {
        if (strcmp(args,helps[i].name) == 0) {
            helps[i].func();
            return;
        }
    }
    helps[i].func();
}
void help_authors() {
    printf("\nauthors - List authors' names and/or logins\n"
    	"\n'authors [OPTION]'\n"
    	"\t-n\tOnly names\n"
    	"\t-l\tOnly logins\n"
    	);
}
void help_getpid() {
    printf("\ngetpid - Show pid of shell\n"
    	"\n'getpid [OPTION]'\n"
    	"\t-p\tShow pid shell's parent process\n"
    	);
}
void help_chdir() {
    printf("\nchdir - Changes current shell's directory\n"
    	"\n'chdir [PATH]'\n"
    	"\tno args shows current directory\n"
    	);
}
void help_getcwd() {
    printf("\ngetcwd - Shows shell's current directory\n");
}
void help_date() {
    printf("\ndate - Shows the date and/or the actual time\n"
    	"\n'date [OPTION]'\n"
    	"\t-d\tShow date (DD:MM:YY format)\n"
    	"\t-h\tShow hour (hh:mm:ss format)\n"
    	"\tno args shows both\n"
    	);
}
void help_hour() {
    printf("\ndate - Shows the actual time\n\n"
    	"\thh:mm:ss format\n"
    	);
}
void help_historic() {
    printf("\nhistoric - List commands history.\n"
    	"\n'historic [OPTION]'\n"
           "\t-count\thistory dimension\n"
           "\t-N\tshows N last commands\n"
           "\tN\trepeats command N\n"
           "\t-clear\tdeletes history\n"
           );
}
void help_open() {
    printf("\nopen - Opens file and adds it to open files list.\n"
        "\n'open [PATH] [MODE]'\n"
        "\tMODE - Opening mode\n"
        "\t\tcr: O_CREAT\t ap: O_APPEND\n"
        "\t\tex: O_EXCL\t ro: O_RDONLY\n"
        "\t\trw: O_RDWR\t wo: O_WRONLY\n"
        "\t\ttr: O_TRUNC\n"
        );
}
void help_close() {
    printf("\nclose - Closes file and deletes its entry from open files list\n"
    	"\n'close [FD]'\n"
    	"\tFD - File descriptor of file to be closed (use listopen to check opened files)\n"
    );
}
void help_dup() {
    printf("\ndup - Duplicates descriptor of file fd and adds one new entry to open files list\n"
    	"\n'dup [FD]'\n"
   		"\tFD - File descriptor of file to be duplicated (use listopen to check opened files)\n"
    );
}
void help_listopen() {
    printf("\nlistopen - List opened files\n"
    	"\n'listopen'\n"
    );
}
void help_infosys() {
    printf("\ninfosys - Show information of the machine running the shell\n"
    	"\n'infosys'\n"
    );
}
void help_exit() {
    printf("\nexit - End shell\n"
    	"\n'exit'\n"
    );
}
void help_bye() {
    printf("\nbye - End shell.\n"
    	"\n'bye'\n"
    );
}
void help_quit() {
    printf("\nquit - End shell.\n"
    	"\n'quit'\n"
    );
}
void help_help() {
    printf("\nhelp - Show help.\n"
    	"\n'help [CMD]'\n"
        "\tCMD - Command of which help should be showed. List of commands:\n"
        "\t\tP0: authors, getpid, chdir, getcwd, date, hour, historic.\n"
        "\t\t    open, close, dup, listopen, infosys, exit, bye, quit, help.\n"
        "\t\tP1: create, setdirparams, getdirparams, dir, erase, delrec, lseek, writestr.\n"
        "\t\tP2: malloc, mmap, shared, free, memfill, memdump, mem, readfile, writefile, read, write, recurse.\n"
        "\tuse '--help' as OPTION in any command to show help.\n"
        );
}
// ======================== P1 HELP FUNCTIONS ========================
void help_create() {
    printf("\ncreate - Create directory or file.\n"
		"\n'create [OPTION] [NAME]'\n"
		"\t-f\t create a file"
		"\tno option will create a directory.\n"
		);
}
void help_setdirparams() {
    printf ("\nsetdirparams - Stablish listing parameters for dir command.\n"
		"\n'setdirparams [PARAMS]'\n"
		"\tPARAMS - Parameters to be set."
		"\t\tlong|short\tlong or short format.\n"
		"\t\thid|nohid\tlist hidden files.\n"
		"\t\tlink|nolink\tlist dest symlinks.\n"
		"\t\trecb|reca|norec\trecursively listing (recurse before, after or not.)\n"
		"\tno params will show active parameters.\n"
		);
}
void help_getdirparams() {
    printf("\ngetdirparams - Show active listing parameters for dir command.\n"
    	"\n'getdirparams\n'"
    	);
}
void help_dir() {
    printf("\ndir - List files.\n"
    "\n'dir [OPTIONS] [PATH1] [PATH2] ...'\n"
	"\t-d\tIf PATH is a directory, show its contents.\n"
	);
}
void help_erase() {
    printf("\nerase - Delete empty files and/or directories.\n"
    	"\n'erase [PATH1] [PATH2] ...'\n"
    	);
}
void help_delrec() {
    printf("\nRecursively delete files and/or directories.\n"
    	"\n'delrec [PATH1] [PATH2] ...'\n"
    	);
}
void help_lseek() {
    printf("\nlseek - Set offset in file.\n"
    	"\n'lseek [FD] [OFF] [REF]'\n"
    	"\tFD - File descriptor of file.\n"
    	"\tOFF - Offset to be set.\n"
    	"\tREF - Offset's reference.\n"
        "\t\t\tSEEK_SET - beginning of the file.\n"
		"\t\t\tSEEK_CUR - actual position.\n"
		"\t\t\tSEEK_END - end of the file.\n");
}
void help_writestr() {
    printf("\nwritestr - Write string in file.\n"
    	"\n'writestr [FD] [STRING]'\n"
    	"\nFD - File descriptor of file to be written.\n"
    	);
}
// ======================== P2 HELP FUNCTIONS ========================
void help_malloc() {
    printf("\nmalloc - Asign a block of memory with malloc.\n"
    	"\n'malloc [OPTIONS] [SIZE]'\n"
    	"\t-free\tDeallocates ONE block of size SIZE (previously allocated). Fails if no block of such size exists.\n"
        "\tno OPTIONS allocates a block of memory of size SIZE asigned with malloc.\n"
        );
}
void help_mmap() {
    printf("\nmmap - Map a file.\n"
    	"\n'mmap [OPTIONS] [PATH] [PERM]'\n"
    	"\t-free\tunmaps file PATH.\n"
    	"\tno option normally maps the file PATH.\n"
    	"\tPERM - permissions to map the file with.\n"
        "\t\tr|w|x\t Read and/or write and/or execute permission.\n");
}
void help_shared() {
    printf("\nshared - Create a block of shared memory.\n"
    	"\n'shared [OPTIONS] [KEY] [SIZE]'\n"
    	"\t-create\tCreates block of shared memory and attaches it to the process address space. Updates the list of memory blocks\n"
    	"\t\tKEY - Key of block created\n"
    	"\t\tSIZE - SIZE of block created\n"
    	"\t-free\tDetaches block of shared memory of key KEY (previously allocated) and deletes it from memory blocks list.\n"
    	"\t-delkey\tRemoves block of shared memory of key KEY from system. The shared memory stays attached to the key.\n"
    	"\tno OPTIONS nor KEY lists shared memory blocks allocated and their respective keys.\n"
    	);
}
void help_free() {
	printf("\nfree - Deallocate memory block and update blocks list.\n"
		"\n'free [ADDR]'\n"
			"\tADDR - Address of block to free\n"
		);
}
void help_memfill() {
	printf("\nmemfill - Fill memory with some character\n"
		"\n'memfill [ADDR] [COUNT] [CH]'\n"
			"\tADDR - Starting address to fill\n"
			"\tCOUNT - Amount of bytes to fill\n"
			"\tCH - Character to fill with\n"
		);
}
void help_memdump() {
	printf("\nmemdump - Dumps contents at some address to screen.\n"
		"\n'memdump [ADDR] [COUNT]'\n"
			"\tADDR - Address of memory at which contents are located\n"
			"\tCOUNT - Amount of bytes to dump to screen\n"
		);
}
void help_mem() {
    printf("\nmem - Print addresses of different elements.\n"
    	"\n'mem [OPTION]'\n"
    		"\t-funcs\tPrint addresses of 3 program functions and 3 library functions.\n"
    		"\t-vars\tPrint the addresses of 3 external, 3 external initialized, 3 static, 3 static initialized and 3 automatic variables.\n"
    		"\t-blocks\tPrint the list of allocated blocks (smalloc, shared and pmap).\n"
    		"\t-all\tPrint all of above.\n"
    		"\t-pmap\tShow output of command pmap for shell's process.\n"
    	);
}
void help_readfile() {
    printf("\nreadfile - Read from a file.\n"
    "\n'readfile [PATH] [ADDR] [COUNT]'\n"
    	"\tPATH - File to be read.\n"
    	"\tADDR - Address of memory where to read.\n"
    	"\tCOUNT - Amount of BYTES to read.\n"
    );
}
void help_writefile() {
    printf("\nwritefile - Write to a file.\n"
    "\n'writefile [PATH] [ADDR] [COUNT]'\n"
    	"\tPATH - File to be written.\n"
    	"\tADDR - Address of memory where to write.\n"
    	"\tCOUNT - Amount of BYTES to write.\n"
    );
}
void help_read() {
    printf("\nread - Read from a file.\n"
    "\n'read [FD] [ADDR] [COUNT]'\n"
    	"\tFD - File descriptor of (already opened) file to be read.\n"
    	"\tADDR - Address of memory where to read.\n"
    	"\tCOUNT - Amount of BYTES to read.\n"
    );
}
void help_write() {
    printf("\nwrite - Write from a file.\n"
    "\n'write [FD] [ADDR] [COUNT]'\n"
    "\tFD - File descriptor of (already opened) file to be written.\n"
    "\tADDR - Address of memory where to write.\n"
    "\tCOUNT - Amount of BYTES to write.\n"
    );
}
void help_recurse() {
    printf("\nrecurse - Executes recursive function.\n"
	    "\n'recurse [N]'\n"
	    "\tN - number of times to be executed.\n"
	    );
}
void help_uid() {
	printf("\nuid - Show or set process's credentials (real and effective).\n"
	    "\n'uid [OPTIONS] [VALUE]'\n"
	    	"\t-get\tShow process's credentials.\n"
	    	"\t-set\tSet process's credentials.\n"
	    		"\t\t-l\tSet process loginame to VALUE (must be string).\n"
	    		"\t\tno further option sets UID to VALUE (must be integer).\n"
	    );
}
void help_envvar() {
	printf("\nenvvar - Change environment variable\n"
	    "\n'envvar [OPTIONS] [VALUES]'\n"
	    	"\t-show\tShow value and address of environment variable of name given.\n"
	    	"\t-change\tSet environment variable of given name.\n"
	    		"\t\t-a|-e|-p\tType of access (through arg3, enrivon or putenv function).\n"
	    		"\t\t\tputenv accesses may create new variables if given was not found.\n"
	    		"\t\t\targ3 and environ accesses shall not add new variables.\n"
	    		"\t\tno further option sets UID to VALUE (must be integer).\n"
	    );
}
void help_showenv() {
	printf("\nshowenv - Show process's environment.\n"
	    "\n'showenv [OPTION]'\n"
	    	"\t-environ\tShow environment accessed throught environ.\n"
	    	"\t-addr\tShow value of environ and arg3, as pointers, and their addresses.\n"
	    	"\tno option set shows environemnt accessed by main's third arg'"
	    );
}
void help_fork() {

}
void help_exec() {

}
void help_jobs() {

}
void help_deljobs() {

}




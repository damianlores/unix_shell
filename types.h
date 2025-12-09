#ifndef TYPES_H
#define TYPES_H 

#include <limits.h>
#include <stdbool.h>
#include <sys/types.h>

#define FLAGS_MAX 7
#define TIME_BUFFER_MAX 25


// ADT of historic list item
typedef struct {
    char input[MAX_INPUT];
    size_t id;
} tHistoricItem;

// ADT of open files list item
typedef struct {
    int fd;
    int dup_of;
    char name[NAME_MAX];
    char* mode[FLAGS_MAX];
} tOFilesItem;

// Definition of constants for mmap function
typedef enum { MEM_MALLOC, MEM_MMAP, MEM_SHARED , ALLOC_MODE_COUNT } tMemType;
// ADT of memory blocks list item
typedef struct {
    void *addr;
    size_t size;
	char time[TIME_BUFFER_MAX];
    tMemType alloc_mode;
    char file[PATH_MAX];
    int fd;
    key_t key;
} tItemM;
#define DEFAULT_ITEM_M (tItemM){ \
    .addr = NULL, \
    .size = 0, \
    .time = "", \
    .alloc_mode = -1, \
    .file = "", \
    .fd = -1, \
    .key = -1 \
}

// ADT of process list item
typedef struct {
    pid_t pid;
    char launch_time[TIME_BUFFER_MAX];
    int status;
    int signal;
    char command[CHAR_MAX];
} tItemP;
#define DEFAULT_ITEM_P (tItemP){ \
    .pid = -1, \
    .launch_time = "", \
    .status = ACTIVE, \
    .signal = -1, \
    .command = "", \
}


// Forward declarations for list types (they are defined in dynamic_list.h)
typedef struct tNodeH* tListH;
typedef struct tNodeF* tListF;
typedef struct tNodeM* tListM;
typedef struct tNodeP* tListP;

// Definition of constants for recursive format used in dir command
typedef enum {REC_OFF , REC_BEFORE , REC_AFTER} tRecFormat;
// Definition of directory info-printing data
typedef struct {
	bool long_format;
	bool link;
	bool show_hid;
	tRecFormat rec_mode;
} tDirParams;

// ADT of shell data
typedef struct {
    tDirParams dirParams;
    tListH HistoricList;
    tListF OFList;
    tListM MemList;
    tListP ProcList;
    char** env;
} tShellState;


// ADT of command functions for dispatch table
typedef void (*tCommandFunc) (char *args[], tShellState *ShellState);
// ADT of commands
typedef struct  {
    const char* name;
    tCommandFunc func;
} tCommand;

// ADT of help functions for dispatch table
typedef void (*tHelpFunc) ();
// ADT of help commands
typedef struct  {
    const char* name;
    tHelpFunc func;
} tHelp;



#endif

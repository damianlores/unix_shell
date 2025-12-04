#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <sys/stat.h>
#include "types.h"

void init_dir_params(tDirParams* dirParams);

int isDir(char* dir);
char* mode_to_str(mode_t m, char* permissions);

void modes_to_flags(char** args, int* mode);
void flags_to_str_arr(int flag, char** modes);
void copy_open_file_flags(char** dest, char** src);

// Historic
void printLastNH(tListH L,int n);
// Open files
void printListF(tListF L);
// Memory
void printListM(tListM L);
void printListTypeM(tListM L, tMemType type);

void print_dir(char* path , tDirParams activeParams);
void print_file(char* path, tDirParams activeParams);
void delete_dir(char* args);




#endif

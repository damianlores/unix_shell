#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <sys/stat.h>
#include "types.h"

void init_dir_params(tDirParams* dirParams);

int isDir(char* dir);

void modes_to_flags(char** args, int* mode);
char* mode_to_str(mode_t m, char* permissions);
void flags_to_str_arr(int flag, char** modes);
void copy_open_file_flags(char** dest, char** src);

void print_dir(char* path , tDirParams activeParams);
void print_file(char* path, tDirParams activeParams);
void delete_dir(char* args);




#endif

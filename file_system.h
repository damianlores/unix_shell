#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <sys/stat.h>
#include "types.h"

void init_dir_params(tDirParams* dirParams);

int isDir(char* dir);

char* mode_to_str(mode_t m, char* permissions);

void print_dir(char* path , tDirParams activeParams);
void print_file(char* path, tDirParams activeParams);
void delete_dir(char* args);




#endif

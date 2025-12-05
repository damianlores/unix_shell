#ifndef COMMANDS_H
#define COMMANDS_H

#include "dynamic_list.h"
#include "types.h"

bool cmd_dispatcher(char *args[], tShellState* ShellState);

void cmd_authors(char *args[], tShellState *ShellState);
void cmd_getpid(char *args[], tShellState *ShellState);
void cmd_chdir(char *args[], tShellState *ShellState);
void cmd_getcwd(char *args[], tShellState *ShellState);
void cmd_date(char *args[], tShellState *ShellState);
void cmd_hour(char *args[], tShellState *ShellState);
void cmd_historic(char *args[], tShellState *ShellState);
void cmd_open(char *args[], tShellState *ShellState);
void cmd_close(char *args[], tShellState *ShellState);
void cmd_dup(char *args[], tShellState *ShellState);
void cmd_listopen(char *args[], tShellState *ShellState);
void cmd_infosys(char *args[], tShellState *ShellState);
void cmd_help(char *args[], tShellState *ShellState);
void cmd_exit(char *args[], tShellState *ShellState);
// P1 CMD FUNCTIONS 
void cmd_create(char *args[], tShellState *ShellState);
void cmd_setdirparams(char *args[], tShellState *ShellState);
void cmd_getdirparams(char *args[], tShellState *ShellState);
void cmd_dir(char *args[], tShellState *ShellState);
void cmd_erase(char *args[], tShellState *ShellState);
void cmd_delrec(char *args[], tShellState *ShellState);
void cmd_lseek(char *args[], tShellState *ShellState);
void cmd_writestr(char *args[], tShellState *ShellState);
// P2 CMD FUNCTIONS 
void cmd_mmap(char *args[], tShellState *ShellState);
void cmd_malloc(char *args[], tShellState *ShellState);
void cmd_free(char *args[], tShellState *ShellState);
void cmd_mem(char *args[], tShellState *ShellState);
void cmd_memfill(char *args[], tShellState *ShellState);
void cmd_memdump(char *args[], tShellState *ShellState);
void cmd_shared(char *args[], tShellState *ShellState);
void cmd_readfile(char* args[], tShellState* ShellState);
void cmd_writefile(char* args[], tShellState* ShellState);
void cmd_read(char* args[], tShellState* ShellState);
void cmd_write(char* args[], tShellState* ShellState);
void cmd_recurse(char* args[], tShellState* ShellState);
// P3
void cmd_uid(char* args[], tShellState* ShellState);
void cmd_envvar(char* args[], tShellState* ShellState);
void cmd_showenv(char* args[], tShellState* ShellState);
void cmd_fork(char* args[], tShellState* ShellState);
void cmd_exec(char* args[], tShellState* ShellState);
void cmd_jobs(char* args[], tShellState* ShellState);
void cmd_deljobs(char* args[], tShellState* ShellState);

// 	HELP FUNCTIONS

void help_dispatcher(char* args);

void help_authors();
void help_getpid();
void help_chdir();
void help_getcwd();
void help_date();
void help_hour();
void help_historic();
void help_open();
void help_close();
void help_dup();
void help_listopen();
void help_infosys();
void help_help();
void help_exit();
// P1 HELP FUNCTIONS
void help_create();
void help_setdirparams();
void help_getdirparams();
void help_dir();
void help_erase();
void help_delrec();
void help_lseek();
void help_writestr();
// P2 HELP FUNCTIONS
void help_malloc();
void help_mmap();
void help_mem();
void help_shared();
void help_free();
void help_memdump();
void help_memfill();
void help_readfile();
void help_writefile();
void help_read();
void help_write();
void help_recurse();
//
void help_uid();
void help_envvar();
void help_showenv();
void help_fork();
void help_exec();
void help_deljobs();
void help_jobs();

#endif

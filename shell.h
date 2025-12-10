#ifndef SHELL_H
#define SHELL_H

#include "commands.h"
#include "dynamic_list.h"

void printPrompt();

bool inputRead(char* input);

void inputProcess(char* input , tShellState *ShellState);

void inherit_std_descriptors(tListF* OFList);

#endif

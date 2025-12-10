// Damian Lores Fernandez d.lores@udc.es
// Marcos Pazos Fernandez m.pazos2@udc.es

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "dynamic_list.h"
#include "shell.h"
#include "commands.h"
#include "file_system.h"
#include "processes.h"



int main(int envc, char* argv[], char* env[]) {
	tShellState ShellState;

	createEmptyListH(&ShellState.HistoricList);
	createEmptyListF(&ShellState.OFList);
	createEmptyListM(&ShellState.MemList);
	createEmptyListP(&ShellState.ProcList);
	ShellState.env = env;
	
	inherit_std_descriptors(&ShellState.OFList);
	init_dir_params(&ShellState.dirParams);


	while (1) {
		char input[MAX_INPUT];
		printPrompt();
		if (!inputRead(input)) {
		        cmd_exit(argv, &ShellState);
		}
		inputProcess(input,&ShellState);
	}
}

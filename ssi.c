#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#define MAX_DIRECTORY_PATH_SIZE 512
#define MAX_COMMAND_SIZE 1024

char *getPrompt();
void parseInput(char* input);
void changeDirectory(char *argv[]);
void backgroundExecution(char *argv[]);
void forkExecution(char *argv[]);

/*
* This function gives the prompt to main.
* Returns the prompt string, NULL if there's a buffer size error.
*/
char *getPrompt(){
	char currentDirectory[MAX_DIRECTORY_PATH_SIZE];
	if(getcwd(currentDirectory,MAX_DIRECTORY_PATH_SIZE)==NULL){
		puts("ERROR: Directory path name is too long.");
		abort();
	}
	char *prompt = malloc(sizeof(char)*(MAX_DIRECTORY_PATH_SIZE+10));
	strncpy(prompt,"SSI: ",6);
	strncat(prompt,currentDirectory,MAX_DIRECTORY_PATH_SIZE);
	strncat(prompt," > ",3);
	return prompt;
}

/*
* This function reads input from a user
* and determines what to do based on it.
*/
void parseInput(char* input){
	printf("You said: %s",input);

	char *token = strtok(input, " \n");
	char *argv[]; //Please add number of possible args.
	argv[0] = token;
	for(size_t i = 0;argv[i] != NULL;i++){
		token = strtok(NULL, " \n");
		argv[i+1] = token;
	}

	//Exiting the program
	if(strcmp(argv[0],"exit")==0){
		puts("Now exiting the simple shell. Goodbye!");
		exit(EXIT_SUCCESS);
	}

	//Changing the directory
	else if(strcmp(argv[0],"cd")==0){
		changeDirectory(argv);
	}

	//Background processing
	else if(strcmp(argv[0],"bg")==0){
		backgroundExecution(argv);
	}

	//Forking and executing
	else{
		forkExecution(argv);
	}

	return;
}

/*
* This function changes the current directory.
* It is called with "cd".
*/
void changeDirectory(char *argv[]){
	
	//Home directory
	if(argv[1]==NULL || strcmp(argv[1],"~")){
		char *homeDir = getenv();
		if(chdir(homeDir)==-1){
			puts("ERROR: Could not change directory.");
		}
	}

	//Move up by one directory
	else if(strcmp(argv[1],"..")){
		asm("nop");
	}

	else{
		
	}

}

void backgroundExecution(char *argv[]){
	return;
}

void forkExecution(char *argv[]){

	pid_t p = fork();

	//Parent process
	if(p > 0){
		waitpid(p,NULL,0);
	}

	//Child process
	else if(p = 0){
		execvp(argv[0],argv[1]);
	}

	else{
		puts("Error: forking into child process failed.");
	}

	return;
}

int main(){
	while(1){
		char *prompt = getPrompt();
		/*
		printf("%s ",prompt);
		free(prompt);
		char *inputted_command = malloc(sizeof(char)*MAX_COMMAND_SIZE);
		if (fgets(inputted_command, MAX_COMMAND_SIZE, stdin) == NULL){
			puts("Error: command is too big. Please enter something smaller");
			printf("The maximum command size allowed is %d characters.\n",MAX_COMMAND_SIZE);
			free(inputted_command);
			continue;
		}
		parseInput(inputted_command);
		free(inputted_command);
		*/
		char* inputted_command = readline(prompt);
		free(prompt);
		parseInput(inputted_command);
		free(inputted_command);
	}	
}

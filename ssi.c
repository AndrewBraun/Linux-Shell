#include <stdlib.h>
#include <stdio.h>
//#include <readline/readline.h>
//#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#define MAX_DIRECTORY_PATH_SIZE 512
#define MAX_COMMAND_SIZE 1024

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
	strncat(prompt," >",2);
	return prompt;
}

/*
* This function reads input from a user
* and determines what to do based on it.
*/
void parseInput(char* input){
	printf("You said: %s",input);
	char *token = strtok(input, " ");

	//strcmp()
	switch(token){
		case "cd":
			token = strtok(NULL, " ");
			changeDirectory(token);
			break;
		case "bg":
			backgroundExecution(input);
			break;
		default:
			puts("Error: invalid command.");
	}
	return;
}

/*
* This function changes the current directory.
* It is called with "cd".
*/
void changeDirectory(char *newDirectory){
	return;
}

void backgroundExecution(char *input){
	return;
}

int main(){
	while(1){
		char *prompt = getPrompt();
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
	}	
}

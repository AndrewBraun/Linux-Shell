#include <stdlib.h>
#include <stdio.h>
//#include <readline/readline.h>
//#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#define MAX_DIRECTORY_PATH_SIZE 512

/*
* This function gives the prompt to main.
* Returns the prompt string, NULL if there's a buffer size error.
*/
char *getPrompt(){
	char currentDirectory[MAX_DIRECTORY_PATH_SIZE];
	if(getcwd(currentDirectory,MAX_DIRECTORY_PATH_SIZE)==NULL){
		puts("ERROR: Directory path name is too long.");
		return NULL;
	}
	char *prompt = malloc(sizeof(char)*(MAX_DIRECTORY_PATH_SIZE+7));
	strncpy(prompt,"SSI: ",5);
	strncat(prompt,currentDirectory,MAX_DIRECTORY_PATH_SIZE);
	strncat(prompt," >",2);
	return prompt;
}

/*
* This function reads input from a user
* and determines what to do based on it.
*/
void parseLine(char* input){
	return;
}

/*
* This function changes the current directory.
* It is called with "cd".
*/
void changeDirectory(){
	return;
}

int main(){
	/*
	char* prompt[MAX_PROMPT_SIZE];
	while(1){
		
		return 0;
	}
	*/
	char *prompt = getPrompt();
	puts(prompt);
	free(prompt);	
}

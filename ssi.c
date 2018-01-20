#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#define MAX_COMMAND_SIZE 1024

char *getPrompt();
void parseInput(char* input);
void changeDirectory(char *argv[]);
void forkExecution(char *argv[]);
void backgroundExecution(char *oldArgv[], size_t oldArgvSize);
void listBGProcesses();
void checkBGProcesses();

// Struct for background processes.
// Linked list structure.
struct bgprocess{
	pid_t pid;
	char *program;
	char *argument;
	struct bgprocess *next;
};

typedef struct bgprocess BGProcess;//Renames bgprocess to BGProcess.
typedef BGProcess *BGProcessPtr;//Shorthand for *BGProcess.

BGProcessPtr rootNode;//Global root node of linked list of BGProcesses.
size_t numOfBGProcesses;//Number of background processes currently processing.

/*
* This function gives the prompt to main.
* Returns the prompt string.
*/
char *getPrompt(){

	//The current directory that the shell is in.
	char *currentDirectory = getcwd(NULL,0);

	//Allocates memory for the prompt string.
	char *prompt = malloc(sizeof(char)*(strlen(currentDirectory)+12));

	//Adds the proper format to the prompt string.
	strncpy(prompt,"SSI: ",6);
	strncat(prompt,currentDirectory,strlen(currentDirectory));
	strncat(prompt," > ",3);

	//Frees the memory allocated for the current directory string.
	free(currentDirectory);
	return prompt;
}

/*
* This function reads input from a user
* and determines what to do based on it.
* It also checks on the background processes after it executes the command.
*/
void parseInput(char* input){

	//Splits up the command string into separate arguments.
	//Leaves the space after the last argument as NULL.
	char *token = strtok(input, " \n");

	//Splits the user command into strings, representing different arguments.
	//Will resize itself to allow "unlimited" number of arguments.
	size_t argvSize = 1024;
	char **argv = malloc(sizeof(char*)*argvSize);
	argv[0] = token;
	for(size_t i = 0; argv[i] != NULL; i++){
		token = strtok(NULL, " \n");

		if(i==argvSize-1){
			argv = realloc(argv,2*argvSize);
			argvSize *= 2;
			if(argv == NULL){
				fprintf(stderr,"ERROR: not enough memory");
				abort();
			}
		}
		argv[i+1] = token;
	}


	//Exiting the program.
	//Both "exit" and "quit" work.
	if(!strcmp(argv[0],"exit") || !strcmp(argv[0],"quit")){
		puts("Now exiting the shell. Goodbye!");
		exit(EXIT_SUCCESS);
	}

	//Changing the directory.
	else if(!strcmp(argv[0],"cd")){
		changeDirectory(argv);
	}

	//Execute a background process.
	else if(!strcmp(argv[0],"bg")){
		backgroundExecution(argv,argvSize);
	}

	//List the background processes.
	else if(!strcmp(argv[0],"bglist")){
		listBGProcesses();
	}

	//Simple forking and executing.
	//The child process does not run in the background.
	else{
		forkExecution(argv);
	}

	/*
	Regardless of the command (unless it's exiting the shell),
	this checks the status of the background processes.
	*/
	checkBGProcesses();

	free(argv);

	return;
}

/*
* This function changes the current directory.
* It is called with "cd".
* If there is no second argument or if it is "~", then it goes to the home directory.
* If the second argument is "..", it goes up one directory.
* Otherwise, it goes down to the specified directory.
* It also prints an error message if the directory is invalid.
*/
void changeDirectory(char *argv[]){
	
	//The current directory that the shell is in.
	char *curDir = getcwd(NULL,0);

	//Change to the home directory
	if(argv[1]==NULL || !strcmp(argv[1],"~")){

		//The home directory
		char *homeDir = getenv("HOME");
		if(chdir(homeDir)==-1){
			puts("ERROR: Could not change directory.");
		}
	}

	//Move up by one directory
	else if(!strcmp(argv[1],"..")){

		//This modifies the current directory string to remove the last directory.
		int i = strlen(curDir)-1;
		for(;i >= 0 && curDir[i]!='/';i--);
		curDir[i]='\0';
		chdir(curDir);
	}

	//Directory specified by user as not home directory or one up
	else{
		//newDir is the string containing the new directory.
		char newDir[strlen(curDir)+strlen(argv[1])+3];

		strncpy(newDir,curDir,strlen(curDir));
		strncat(newDir,"/",1);
		strncat(newDir,argv[1],strlen(argv[1]));
		if(chdir(newDir)!= 0 ){
			puts("ERROR: Invalid directory");
		}	
	}

	//Frees the current directory string from memory.
	free(curDir);

}

//Basic fork and execute function
//(Non-background process child execution)
void forkExecution(char *argv[]){

	//The process id of the currently executing child process.
	pid_t p = fork();

	//If this is the parent process...
	if(p > 0){
		waitpid(p,NULL,0);
	}

	//If this is the child process...
	else if(p == 0){
		execvp(argv[0],argv);
		fprintf(stderr,"ERROR: starting child process failed.\n");
		abort();
	}

	//If there was an error in forking...
	else{
		puts("Error: forking into child process failed.");
	}
}

/*
* Starts a process and runs it in the background.
* Also adds a node reporesenting it to a linked list of BG processes.
*/
void backgroundExecution(char *oldArgv[], size_t oldArgvSize){

	//This makes a new list of arguments without the "bg" command in it.	
	char *newArgv[oldArgvSize * sizeof(char*)];
	for(size_t i = 0; oldArgv[i]!=NULL;i++){
		newArgv[i] = oldArgv[i+1];
	}

	//The process id of the new child process.
        pid_t p = fork();

        //If this is the parent process...
        if(p > 0){
		BGProcessPtr newProcess = malloc(sizeof(BGProcess));
		if(newProcess == NULL){
			fprintf(stderr,"ERROR: Out of memory");
			abort();
		}

		numOfBGProcesses++;

		newProcess->pid = p;
		newProcess->program = "Process";
		newProcess->argument = "Argument";
		newProcess->next = NULL;

		if(rootNode == NULL){
			rootNode = newProcess;
			return;
		}

		BGProcessPtr curNode = rootNode;
		while(curNode->next != NULL){
			curNode = curNode->next;
		}

		curNode->next = newProcess;
        }

        //If this is the child process...
        else if(p == 0){
                execvp(newArgv[0],newArgv);
                fprintf(stderr,"ERROR: starting child process failed.\n");
		abort();
        }

        //If there was an error in forking...
        else{
                puts("Error: forking into child process failed.");
        }


        return;
}

/*
* Lists all background processes.
*/
void listBGProcesses(){

	BGProcessPtr curNode = rootNode;
	while(curNode != NULL){
		printf("%d: %s %s",curNode->pid, curNode->program, curNode->argument);
		curNode = curNode->next;
	}
	printf("Total Background jobs: %zu",numOfBGProcesses);
}

/*
* Checks the statuses of all background processes.
* If one has terminated, it prints a message indocating it.
*/
void checkBGProcesses(){

	//Nothing to check of there are no background processes.
	if (!numOfBGProcesses) return;

	pid_t pidFin = waitpid(0,NULL,WNOHANG);

	while(pidFin > 0){
	
		BGProcessPtr curNode = rootNode;
		BGProcessPtr prevNode;
		while(curNode->pid != pidFin){
			prevNode = curNode;
			curNode = curNode->next;
		}

		if(curNode->pid == rootNode->pid){
			rootNode = rootNode->next;
		}

		else{
			prevNode->next = curNode->next;
		}

		printf("%u %s %s has finished.",curNode->pid, curNode->program, curNode->argument);
		free(curNode);
		numOfBGProcesses--;	
		pidFin = waitpid(0,NULL,WNOHANG);
	}
}

int main(){

	//Initializes root node of linked list to NULL.
	rootNode = NULL;
	numOfBGProcesses = 0;

	while(1){

		/*
		//Gets string for the prompt.
		char *prompt = getPrompt();
		
		printf("%s ",prompt);
		free(prompt);

		//Gets the command inputted from the user.
		char *inputted_command = malloc(sizeof(char)*MAX_COMMAND_SIZE);
		if (fgets(inputted_command, MAX_COMMAND_SIZE, stdin) == NULL){
			printf("Error: command is too big. Please enter something smaller.\n");
			printf("The maximum command size allowed is %d characters.\n",MAX_COMMAND_SIZE);
			free(inputted_command);
			continue;
		}
		parseInput(inputted_command);
		free(inputted_command);
		*/
		
		char *prompt = getPrompt();	
		char* inputted_command = readline(prompt);
		free(prompt);
		parseInput(inputted_command);
		free(inputted_command);
	}	
}


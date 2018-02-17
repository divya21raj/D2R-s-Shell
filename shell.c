
/* Name: DIVYA RAJ
   ID: 1610110123
*/

/*This is a simple shell implementation*/

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pwd.h>

#define MAXLINE 80 /* The maximum length of a command*/
#define HISTORY_LIMIT 1024 /*Maximum number of commands to be stored in history*/

void tokenizeInput(char input[MAXLINE], char *args[MAXLINE/2 + 1]);

int isInbuilt(char input[MAXLINE]);/*To check if command is inbuilt
									returns 1, 2, 3, 0 :exit, cd, history, not inbuilt*/
void handleInbuilt(int inbuilt, char *args[MAXLINE/2 + 1]);

void runCommand(char *args[41]);

int checkAmpersand(char input[MAXLINE]);

void addToHistory(char input[MAXLINE]);

void loadHistory();

void saveHistory();

void showHistory();

int shouldRun = 1; /* flag indicating if exit was the input*/
int inbuilt = 0; /* flag indicating if command is inbuilt */
int hasAmpersand = 0; /*flag indicating if input fas precede by &*/

char historyPath[1024];

char *history[HISTORY_LIMIT];
int historySize = 0;  //number of commands in history

int main(void)
{
	char input[MAXLINE];
	char *args[MAXLINE/2 + 1]; /* command line arguments */

	struct passwd *pw = getpwuid(getuid());  /*getting the user's home directory*/
	char *homedir = pw->pw_dir;

	strcpy(historyPath, homedir);
	strcat(historyPath, "/history.txt");   //initialising path to history
	
	loadHistory();  //load the history from file

	while (shouldRun)
	{
		printf(">>");
		fflush(stdout);
		fflush(stdin);

		fgets(input, MAXLINE, stdin);  //taking the input
		input[strlen(input)-1] = '\0'; //shaving off the \n at the end

		inbuilt = isInbuilt(input);		//checking if input is for an inbuilt command
		hasAmpersand = checkAmpersand(input); //checking if & at the end

		addToHistory(input);  //add the input to the history

		tokenizeInput(input, args);  //tokenize the input to fill args

		runCommand(args);  //run the command
	}

	saveHistory();  //save the history to file
	
	return 0;
}

void runCommand(char *args[MAXLINE/2 + 1])
{
	if(!inbuilt)
	{
		pid_t pid;

		pid = fork(); /* fork a child process */

		if (pid < 0)
		{    /* error occurred */
			printf("Error occurred while forking\n");
		}
		else if (pid == 0)
		{    /* child process */
			if (execvp(args[0], args) < 0)
				printf("Could not execute command..\n");

			hasAmpersand = 0;
			//exit(0);
		}
		else
		{    /* parent process */
			if(!hasAmpersand)
				wait(NULL); /* parent will wait for the child to complete */

			else
			{
				printf("Process created with PID: %d\n",pid);
				sleep(1);
			}
		}
	}

	else
		handleInbuilt(inbuilt, args);

}

void loadHistory()
{
	char command[MAXLINE];

	FILE *fptr;

	if ((fptr = fopen(historyPath, "r")) == NULL)
	{
		fptr = fopen(historyPath, "w");
		fclose(fptr);
		fptr = fopen(historyPath, "r");
	}

	while(fgets(command, sizeof(command), fptr))
	{
		command[strlen(command)-1] = '\0';
		history[historySize++] = strdup(command);
	}

	fclose(fptr);
}

void saveHistory()
{
	int i;

	FILE *fptr;

	if((fptr = fopen(historyPath,"w")) == NULL)
	{
		printf("Error!");exit(1);
	}

	for(i = 0; i < historySize; i++)
	{
		fprintf(fptr, "%s\n", history[i]);
	}

	fclose(fptr);
}

int isInbuilt(char input[MAXLINE])
{
	int flag = 0;

	if(strcmp(input, "exit") == 0)
		flag = 1;

	else if(input[0] == 'c' && input[1] == 'd')
		flag = 2;

	else if(strcmp(input, "history") == 0)
		flag = 3;

	else if(strcmp(input, "!!") == 0)
		strcpy(input, history[historySize-1]);

	else if(input[0] == '!' && input[1] != '!')
	{
		memmove(input, input+1, strlen(input));  //removing the first !, so we have only the number left
		int num = atoi(input);

		if(num)
		{
			strcpy(input, history[historySize-num]);
			flag = isInbuilt(input);		//checking if input is for an inbuilt command
		}
	}

	return flag;
}

void handleInbuilt(int inbuilt, char *args[MAXLINE/2 + 1])
{
	switch(inbuilt)
	{
		case 1:
			shouldRun = 0;
			printf("Bye!\n");
			break;

		case 2:
			chdir(args[1]);
			break;

		case 3:
			showHistory();

		default:
			break;
	}
}

void showHistory()
{
	int i;

	if(historySize != 0)
	{
		for(i =0; i < historySize; i++)
			printf("%d.%s\n", historySize - i,history[i]);
	}
}

void addToHistory(char input[MAXLINE])
{
	if(historySize < 1024)
		history[historySize++] = strdup(input);

	else
		history[1024 - historySize++] = strdup(input);
}

void tokenizeInput(char input[MAXLINE], char *args[MAXLINE/2 + 1])
{
	int i = 0;

	char* token = strtok(input, " ");
	while (token)
	{
		args[i++] = token;
		token = strtok(NULL, " ");
	}

	args[i] = NULL;
}

int checkAmpersand(char input[MAXLINE])
{
	size_t size = strlen(input);

	if(input[size-1] == '&')
	{
		input[size - 1] = '\0';
		return 1;
	}

	else return 0;
}


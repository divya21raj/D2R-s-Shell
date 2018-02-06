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

#define MAXLINE 80 /* The maximum length of a command*/

void tokenizeInput(char input[MAXLINE], char *args[MAXLINE/2 + 1]);

int isInbuilt(char input[MAXLINE]);/*To check if command is inbuilt
									returns 1, 2, 0 :exit, cd, not inbuilt*/
void handleInbuilt(int inbuilt, char *args[MAXLINE/2 + 1]);

void runCommand(char *args[41]);

int checkAmpersand(char input[MAXLINE]);

int shouldRun = 1; /* flag indicating if exit was the input*/
int inbuilt = 0; /* flag indicating if command is inbuilt */
int hasAmpersand = 0; /*flag indicating if input fas precede by &*/


int main(void)
{
	char input[MAXLINE];
	char *args[MAXLINE/2 + 1]; /* command line arguments */

	while (shouldRun)
	{
		printf(">>");
		fflush(stdout);
		fflush(stdin);

		fgets(input, MAXLINE, stdin);  //taking the input
		input[strlen(input)-1] = '\0'; //shaving off the \n at the end

		inbuilt = isInbuilt(input);
		hasAmpersand = checkAmpersand(input);

		tokenizeInput(input, args);

		runCommand(args);
	}

	return 0;
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

void runCommand(char *args[MAXLINE/2 + 1])
{
	if(!inbuilt)
	{
		pid_t pid;

		pid = fork(); /* fork a child process */

		if (pid < 0)
		{    /* error occurred */
			fprintf(stderr, "Fork Failed");
		}
		else if (pid == 0)
		{    /* child process */
			if (execvp(args[0], args) < 0)
				printf("\nCould not execute command..");

			hasAmpersand = 0;
			exit(0);
		}
		else
		{    /* parent process */
			/* parent will wait for the child to complete */
			if(!hasAmpersand)
				wait(NULL);

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

		default:
			break;
	}
}

int isInbuilt(char input[MAXLINE])
{
	int flag = 0;

	if(strcmp(input, "exit") == 0)
		flag = 1;

	else if(input[0] == 'c' && input[1] == 'd')
		flag = 2;

	return flag;
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


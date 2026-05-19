#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_TASKS 100
#define TASKS_FILENAME ".tasks.txt"

char *tasks[MAX_TASKS];
int tasksLen = sizeof(tasks) / sizeof(tasks[0]);

FILE *tasksFile;

int add_task() {
	enum Priority {
		LOW = 3,
		MEDIUM = 2,
		HIGH = 1
	};

	enum Status{
		TODO = 0,
		DONE = 1
	};

	struct Task {
		int num;
		char name[100];
		enum Priority priority;
		enum Status status;
	};

	struct Task task;

	// Get number of tasks
	char c;
	task.num = 1;
	while(!feof(tasksFile))
	{
		c = fgetc(tasksFile);
		if(c == '\n')
		{
			task.num++;
		}
	}

	// Set status as TODO
	task.status = 0;

	printf("Type your task: ");
	fgets(task.name, sizeof(task.name), stdin);
	task.name[strcspn(task.name, "\n")] = 0;
	printf("Select priority:\n");
	printf("[1] HIGH\n");
	printf("[2] MEDIUM\n");
	printf("[3] LOW\n");
	int priorityInput;
	scanf("%d", &priorityInput);
	// Consume newline
	getchar();

	switch(priorityInput) {
		case 1:
			task.priority = HIGH;
			break;
		case 2:
			task.priority = MEDIUM;
			break;
		case 3:
			task.priority = LOW;
			break;
		default:
			task.priority = MEDIUM;
	}

	//printf("Task: %s\n", task.name);
	//printf("Priority %d\n", task.priority);
	//printf("Status %d\n", task.status);

	tasksFile = fopen(TASKS_FILENAME, "a");
	if (tasksFile == NULL) return 1;
	fprintf(tasksFile, "%d,\"%s\",%d,%d\n", task.num, task.name, task.priority, task.status);
	fclose(tasksFile);

	return 0;
}

int complete_task() {
	return 0;
}

int delete_task() {
	return 0;
}

int draw_home() {
	char input[10];

	printf("Tasks\n");
	printf("==========\n");

	// Display tasks from file
	char buffer[MAX_TASKS];
	tasksFile = fopen(TASKS_FILENAME, "r");
	if (tasksFile == NULL) {
		tasksFile = fopen(TASKS_FILENAME, "w");
		fprintf(tasksFile, "\0");
	}
	while (fgets(buffer, MAX_TASKS, tasksFile) != NULL) {
		printf("%s", buffer);
	}
	fclose(tasksFile);

	// Print at bottom of screen
	printf("\033[999;1H[a] add task | [c] complete task | [d] delete task");
	fgets(input, sizeof(input), stdin);
	input[strcspn(input, "\n")] = 0;
	if (strcmp(input, "a") == 0) {
		add_task();
	}
	else if (strcmp(input, "c") == 0) {
		complete_task();
	}
	else if (strcmp(input, "d") == 0) {
		delete_task();
	}
	else {
		printf("unknown command %s", input);
		return 1;
	}



	return 0;
}

int main(int argc, char *argv[]) {	

	for (;;) {
		draw_home();
	}

	return 0;
}

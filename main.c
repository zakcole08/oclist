#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define MAX_TASKS 100
#define TASKS_FILENAME ".tasks.txt"

char *tasks[MAX_TASKS];
int tasksLen = sizeof(tasks) / sizeof(tasks[0]);

FILE *tasksFile;

typedef enum {
	LOW = 3,
	MEDIUM = 2,
	HIGH = 1
} Priority;

typedef enum {
	TODO = 0,
	DONE = 1
} Status;

const char *priority_str[] = {
	"INVALID",
	"HIGH",
	"MEDIUM",
	"LOW"
};

const char *status_str[] = {
	"TODO",
	"DONE"
};

struct Task {
	int num;
	char name[100];
	Priority priority;
	Status status;
};

int clear_screen() {
	printf("\e[1;1H\e[2J");
	return 0;
}

char getch() {
	struct termios oldt, newt;
	char c;
	// Save current terminal settings
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	// Disable buffering and echo
	newt.c_lflag &= ~(ICANON | ECHO);
	// Apply new settings
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	c = getchar();
	// Restore old terminal settings
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return c;
}

int display_tasks() {
	printf("Tasks\n");
	printf("========\n");
	tasksFile = fopen(TASKS_FILENAME, "r");
	if (tasksFile == NULL) {
		tasksFile = fopen(TASKS_FILENAME, "w");
		fprintf(tasksFile, "\0");
	}
	struct Task task;
	while (fscanf(tasksFile, "%d,\"%[^\"]\",%d,%d\n", &task.num, task.name, (int*)&task.priority, (int*)&task.status) == 4) {
		printf("%d. %-32s%-8s%-8s\n", task.num, task.name, priority_str[task.priority], status_str[task.status]);
	}
	fclose(tasksFile);
	return 0;
}


int add_task() {
	struct Task task;

	// Get number of tasks
	tasksFile = fopen(TASKS_FILENAME, "r");
	char c;
	task.num = 1;
	while ((c = fgetc(tasksFile)) != EOF)
	{
		if(c == '\n')
		{
			task.num++;
		}
	}
	fclose(tasksFile);

	// Set new task status to TODO
	task.status = TODO;

	clear_screen();
	display_tasks();
	// Print at bottom of the screen
	printf("\033[999;1HType your task: ");
	fgets(task.name, sizeof(task.name), stdin);
	task.name[strcspn(task.name, "\n")] = 0;

	clear_screen();
	display_tasks();
	printf("\033[999;1HSelect priority: [1] HIGH | [2] MEDIUM | [3] LOW");
	int priorityInput = getch();

	switch(priorityInput) {
		case '1':
			task.priority = HIGH;
			break;
		case '2':
			task.priority = MEDIUM;
			break;
		case '3':
			task.priority = LOW;
			break;
		default:
			task.priority = MEDIUM;
	}

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
	clear_screen();
	display_tasks();

	// Print at bottom of screen
	printf("\033[999;1H[a] add task | [c] complete task | [d] delete task");
	char input = getch();
	if (input == 'a') {
		add_task();
	}
	else if (input == 'c') {
		complete_task();
	}
	else if (input == 'd') {
		delete_task();
	}
	else {
		printf("unknown command %c", input);
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

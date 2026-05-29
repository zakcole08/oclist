#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define MAX_TASKS 100
#define TASKS_FILENAME ".tasks.txt"
#define TEMP_FILENAME ".tasks.temp"

#define BOT "\033[999;1H"
#define GRN "\033[32m"
#define RED "\033[31m"
#define ORG "\033[38;5;208m"
#define CYN "\033[36m"
#define MAG "\033[35m"
#define GRY "\033[100m"
#define RST "\033[0m"
#define CLR "\033[1;1H\033[2J"

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

// Set default tab to TODO tab
Status tabSel = TODO;

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
	printf(CLR);
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
	if (tabSel == TODO) {
		printf(GRY"| TODO  |"RST" DONE |\n");
		printf(GRY"'-------'"RST"------'\n");
	}
	else {
		printf("| TODO  "GRY"| DONE |\n"RST);
		printf("'-------"GRY"'------'\n"RST);	
	}
	printf("Tasks\n");
	printf("========\n");
	// Headers
	printf("   %-32s%-12s%-8s\n", "Name", "Priority", "Status");
	tasksFile = fopen(TASKS_FILENAME, "r");
	if (tasksFile == NULL) {
		tasksFile = fopen(TASKS_FILENAME, "w");
		fprintf(tasksFile, "\0");
	}
	struct Task task;
	while (fscanf(tasksFile, "%d,\"%[^\"]\",%d,%d\n", &task.num, task.name, (int*)&task.priority, (int*)&task.status) == 4) {
		if (tabSel == TODO && task.status != TODO) {
			continue;
		}
		if (tabSel == DONE && task.status != DONE) {
			continue;
		}
		switch (task.priority) {
			case HIGH:
				if (task.status == DONE) {
					printf("%d. %-32s"RED"%-12s"GRN"%-8s\n"RST, task.num, task.name, priority_str[task.priority], status_str[task.status]);
				}
				else {
					printf("%d. %-32s"RED"%-12s"RST"%-8s\n", task.num, task.name, priority_str[task.priority], status_str[task.status]);
				}
				break;
			case MEDIUM:
				if (task.status == DONE) {
					printf("%d. %-32s"ORG"%-12s"GRN"%-8s\n"RST, task.num, task.name, priority_str[task.priority], status_str[task.status]);
				}
				else {
					printf("%d. %-32s"ORG"%-12s"RST"%-8s\n", task.num, task.name, priority_str[task.priority], status_str[task.status]);
				}
				break;
			case LOW:
				if (task.status == DONE) {
					printf("%d. %-32s"CYN"%-12s"GRN"%-8s\n"RST, task.num, task.name, priority_str[task.priority], status_str[task.status]);
				}
				else {
					printf("%d. %-32s"CYN"%-12s"RST"%-8s\n", task.num, task.name, priority_str[task.priority], status_str[task.status]);
				}
				break;
			default:
				printf("%d. %-32s%-12s%-8s\n", task.num, task.name, priority_str[task.priority], status_str[task.status]);
		}
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
	clear_screen();
	display_tasks();

	printf("\033[999;1HNumber of completed task: ");
	// Convert char to int
	int selectedNum = getch() - '0';

	struct Task task;
	tasksFile = fopen(TASKS_FILENAME, "r");
	FILE *tempFile = fopen(TEMP_FILENAME, "w");
	if (tasksFile == NULL) {
		tasksFile = fopen(TASKS_FILENAME, "w");
		fprintf(tasksFile, "\0");
	}
	// Copy each line to a temp file
	while (fscanf(tasksFile, "%d,\"%[^\"]\",%d,%d\n", &task.num, task.name, (int*)&task.priority, (int*)&task.status) == 4) {
		// Mark the selected task as done
		if (task.num == selectedNum) {
			task.status = DONE;
		}
		fprintf(tempFile, "%d,\"%s\",%d,%d\n", task.num, task.name, task.priority, task.status);
	}
	fclose(tasksFile);
	fclose(tempFile);
	// Remove old tasks file and rename the temp file to replace it
	remove(TASKS_FILENAME);
  rename(TEMP_FILENAME, TASKS_FILENAME);
	return 0;
}

int delete_task() {
	clear_screen();
	display_tasks();

	printf("\033[999;1HNumber of completed task: ");
	// Convert char to int
	int selectedNum = getch() - '0';

	struct Task task;
	tasksFile = fopen(TASKS_FILENAME, "r");
	FILE *tempFile = fopen(TEMP_FILENAME, "w");
	if (tasksFile == NULL) {
		tasksFile = fopen(TASKS_FILENAME, "w");
		fprintf(tasksFile, "\0");
	}
	// Copy each line to a temp file
	while (fscanf(tasksFile, "%d,\"%[^\"]\",%d,%d\n", &task.num, task.name, (int*)&task.priority, (int*)&task.status) == 4) {
		// Don't copy the selected task
		if (task.num == selectedNum) {
			continue;
		}
		if (task.num > selectedNum) {
			task.num--;
		}
		fprintf(tempFile, "%d,\"%s\",%d,%d\n", task.num, task.name, task.priority, task.status);
	}
	fclose(tasksFile);
	fclose(tempFile);
	// Remove old tasks file and rename the temp file to replace it
	remove(TASKS_FILENAME);
  rename(TEMP_FILENAME, TASKS_FILENAME);
	return 0;
}

int edit_task() {
	clear_screen();
	display_tasks();

	printf("\033[999;1HNumber of task to edit: ");
	// Convert char to int
	int selectedNum = getch() - '0';

	clear_screen();
	display_tasks();

	printf("\033[999;1HNew task name: ");
	char newTaskName[100];
	fgets(newTaskName, sizeof(newTaskName), stdin);
	newTaskName[strcspn(newTaskName, "\n")] = 0;

	clear_screen();
	display_tasks();
	printf("\033[999;1HSelect priority: [1] HIGH | [2] MEDIUM | [3] LOW");
	int newPriorityInput = getch();


	struct Task task;
	tasksFile = fopen(TASKS_FILENAME, "r");
	FILE *tempFile = fopen(TEMP_FILENAME, "w");
	if (tasksFile == NULL) {
		tasksFile = fopen(TASKS_FILENAME, "w");
		fprintf(tasksFile, "\0");
	}
	// Copy each line to a temp file
	while (fscanf(tasksFile, "%d,\"%[^\"]\",%d,%d\n", &task.num, task.name, (int*)&task.priority, (int*)&task.status) == 4) {
		// Edit the task
		if (task.num == selectedNum) {
			strcpy(task.name , newTaskName);
			switch(newPriorityInput) {
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
		}
		fprintf(tempFile, "%d,\"%s\",%d,%d\n", task.num, task.name, task.priority, task.status);
	}
	fclose(tasksFile);
	fclose(tempFile);
	// Remove old tasks file and rename the temp file to replace it
	remove(TASKS_FILENAME);
  rename(TEMP_FILENAME, TASKS_FILENAME);
	
	return 0;
	
	return 0;
}

int draw_home() {
	clear_screen();
	display_tasks();

	// Print at bottom of screen
	printf("\033[999;1H[a] add task | [c] complete task | [d] delete task | [e] edit task | [t] toggle tab | [q] quit");
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
	else if (input == 'e') {
		edit_task();
	}
	else if (input == 't') {
		if (tabSel == TODO) {
			tabSel = DONE;
		}
		else {
			tabSel = TODO;
		}
	}
	else if (input == 'q') {
		printf("\n");
		return -1;
	}
	else {
		printf("unknown command %c", input);
		return 1;
	}

	return 0;
}

int main(int argc, char *argv[]) {	

	for (;;) {
		if (draw_home() == -1) {
			return 0;
		}
	}

	return 0;
}

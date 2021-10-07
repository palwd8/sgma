#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    int hour;
    int min;
    char *sub;
    char *link;
} Class;

const char FILE_NAME[] = "/usr/share/sgma/schedule.txt";
const int NUM_CLASSES = 9;
const int FIRST_CLASS = 9;

void notify(char *class_name)
{
	if (fork() == 0) {
		execlp("notify-send", "notify-send", "-a", "SGMA", class_name, "Starting in a few mins", NULL);
	}
}

void get_schedule(FILE *file, Class schedule[NUM_CLASSES], int current_day)
{
    char line[100];
    int hour;
    char dayfound = 0;
    while(fgets(line, sizeof(line), file)) {
        if (line[0] == '#') continue;
        if ((line[0] == '[' && atoi(&line[1]) == current_day)) {
            dayfound = 1;
            continue;
        }

        /* Ignore the rest of the file */
        if (dayfound && line[0] == '\n') {
            dayfound = 0;
            break;
        }

        if (dayfound) {
            hour = atoi(strtok(line, ":"));
            schedule[hour - FIRST_CLASS].hour = hour;
            schedule[hour - FIRST_CLASS].min = atoi(strtok(NULL, "\t"));
            schedule[hour - FIRST_CLASS].sub  = strdup(strtok(NULL, "\t")); /* We copy it first because strtok returns a pointer */
            schedule[hour - FIRST_CLASS].link = strdup(strtok(NULL, "\t"));
        }
    }
}

void join_meet(Class schedule[NUM_CLASSES], time_t now)
{
    struct tm *current_local;
    int cur_min;
    int cur_hour;
    int min_left;

    for (int i = 0; i < NUM_CLASSES; ++i) {

        now = time(NULL);
        current_local = localtime(&now);
        cur_min = current_local->tm_min;
        cur_hour = current_local->tm_hour;

        if (schedule[i].hour != -1) {
            if (cur_hour <= schedule[i].hour) {
                min_left = (schedule[i].hour - cur_hour) * 60 + schedule[i].min - cur_min - 4;
                if (min_left < 0) continue;
                sleep(min_left * 60);
                if (fork() == 0) {
                    notify(schedule[i].sub); 
                    execlp("xdg-open", "xdg-open", schedule[i].link, NULL);
                }
            }
        }
    }
}

int main()
{
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    int current_day = local->tm_wday;

    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        perror(FILE_NAME);
        exit(EXIT_FAILURE);
    }

    Class schedule[NUM_CLASSES];
    for (int i = 0; i < NUM_CLASSES; ++i)
        schedule[i].hour = -1;

    get_schedule(file, schedule, current_day);

    fclose(file);

    join_meet(schedule, now);

    return 0;
}

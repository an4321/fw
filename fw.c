/* See LICENSE file for copyright and license details.
 *
 * fw is a utility that monitors the specified files for updates
 * and executes a command when changes are detected.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

// structs
typedef struct {
    char **before;
    int beforeCount;
    char *after;
} ArgParserResult;

// function declarations
ArgParserResult parseargs(int argc, char *argv[]);

// variables
int clear = 0;

// function implementations
ArgParserResult parseargs(int argc, char *argv[]) {
    ArgParserResult result = {NULL, 0, NULL};

    int i;
    int beforeCount = 0;

    int separatorIndex = -1;
    for (i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--") == 0) {
            separatorIndex = i;
            break;
        }
    }

    if (separatorIndex != -1) {
        // allocate memory for the "before" args
        result.before = malloc((separatorIndex + 1) * sizeof(char *));
        for (i = 1; i < separatorIndex; i++) {
            if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--clear") == 0) {
                clear = 1;
            } else {
                result.before[beforeCount++] = argv[i];
            }
        }
        result.beforeCount = beforeCount;

        // allocate memory for the "after" args
        int afterLength = 0;
        for (i = separatorIndex + 1; i < argc; i++) {
            afterLength += strlen(argv[i]) + 1; // +1 for the space
        }
        result.after = malloc(afterLength);
        result.after[0] = '\0'; // initialize with an empty string
        for (i = separatorIndex + 1; i < argc; i++) {
            strcat(result.after, argv[i]);
            strcat(result.after, " ");
        }
        result.after[afterLength - 1] = '\0'; // remove the trailing space
    } else {
        // if "--" is not found, consider all args as "before" args
        result.before = malloc(argc * sizeof(char *));
        for (i = 0; i < argc; i++) {
            result.before[beforeCount++] = argv[i];
        }
        result.beforeCount = beforeCount;
    }

    return result;
}

void watch_files(char **files_to_watch, int num_files, char *command) {
    struct stat file_stat;
    time_t last_modification_time[num_files];

    // initialize last modification times
    for (int i = 0; i < num_files; i++) {
        stat(files_to_watch[i], &file_stat);
        last_modification_time[i] = file_stat.st_mtime;
    }

    while (1) {
        int modified = 0;
        for (int i = 0; i < num_files; i++) {
            stat(files_to_watch[i], &file_stat);

            // check for modification
            if (file_stat.st_mtime != last_modification_time[i]) {
                last_modification_time[i] = file_stat.st_mtime;
                modified = 1;
            }
        }
        if (modified) {
            if (clear) {
                system("clear");
            }
            system(command);
            printf("\n");
        }
        sleep(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        ArgParserResult result = parseargs(argc, argv);

        if (result.beforeCount > 0 && result.after != NULL) {
            watch_files(result.before, result.beforeCount, result.after);
        }

        free(result.before);
        free(result.after);
    } else {
        printf("fw - File Watch\n");
        printf("Usage: fw [-c] <watch>... -- <command>\n");
        printf("\n");
        printf("Options:\n");
        printf("\t-c, --clear\tclear before executing the command\n");
        printf("\n");
        printf("Arguments:\n");
        printf("\t<watch> \tfiles / directorys to monitor for updates\n");
        printf("\t<command> \tcommand to execute when updates are detected\n");
        printf("Example:\n");
        printf("\tfw -c main.c -- cc main.c -o main\n");
        printf("\n");
    }

    return 0;
}

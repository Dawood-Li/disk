#include "commands.h"

int main() {
    struct MyFileSystem * fs = filesystem_init();
    char command[256];
    char option[256];
    while (1) {
        pwd(fs); printf(" $ ");
        if (scanf(" %s", command) == EOF)           { return 0;              }
        else if (strcmp(command, "q"        ) == 0) { return 0;              }
        else if (strcmp(command, "exit"     ) == 0) { return 0;              }
        else if (strcmp(command, "clear"    ) == 0) { system("clear");       }
        else if (strcmp(command, "blockinfo") == 0) { block_info(fs);        }
        else if (strcmp(command, "pwd"      ) == 0) { pwd(fs); printf("\n"); }
        else if (strcmp(command, "ls"       ) == 0) { list_dir(fs);          }
        else if (strcmp(command, "cd") == 0) {
            scanf(" %s", option);
            change_dir(fs, option);
        } else if (strcmp(command, "mkdir") == 0) {
            scanf(" %s", option);
            mkdir(fs, option);
        } else if (strcmp(command, "rmdir") == 0) {
            scanf(" %s", option);
            rmdir(fs, option);
        } else if (strcmp(command, "create") == 0) {
            scanf(" %s", option);
            printf("input reserved size:");
            int reserved_size;
            scanf(" %d", &reserved_size);
            create_file(fs, option, reserved_size);
        } else if (strcmp(command, "delete") == 0) {
            scanf(" %s", option);
            rmfile(fs, option);
        } else if (strcmp(command, "fileinfo") == 0) {
            scanf(" %s", option);
            fileinfo(fs, option);
        } else {
            printf("%s: command not found.\n", command);
        }
    }
}

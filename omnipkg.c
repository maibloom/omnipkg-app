#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h> // For close()
#include "os_detect.h" // Include our new header

// Forward declarations
void run_put(int argc, char *argv[]); // Actual definition is expected in another .c file (e.g., omniput.c)
void handle_install(int argc, char *argv[]);
void handle_updateall();
void handle_backup(const char *filename);
void handle_restore(const char *filename);
void handle_define_os(); // New handler function

int execute_command(const char *command) {
    printf("Executing: %s\n", command);
    int status = system(command);

    if (status == -1) {
        perror("Failed to execute command (system call error)");
        return EXIT_FAILURE;
    } else if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        fprintf(stderr, "Command failed with exit status %d.\n", WEXITSTATUS(status));
        return EXIT_FAILURE;
    } else if (WIFSIGNALED(status)) {
        fprintf(stderr, "Command terminated by signal %d.\n", WTERMSIG(status));
        return EXIT_FAILURE;
    }
    printf("Command executed successfully.\n");
    return EXIT_SUCCESS;
}

void handle_install(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: omnipkg install <manager> <package1> [package2...]\n");
        fprintf(stderr, "Managers: flathub\n");
        return;
    }

    const char *manager = argv[0];
    char command[4096];
    size_t current_len = 0;

    if (strcmp(manager, "flathub") == 0) {
        current_len = snprintf(command, sizeof(command), "flatpak install -y flathub");
    } else {
        fprintf(stderr, "Unknown package manager for install: %s\n", manager);
        fprintf(stderr, "Available managers: flathub\n");
        return;
    }

    if (current_len >= sizeof(command)) {
        fprintf(stderr, "Initial command string too long.\n");
        return;
    }

    for (int i = 1; i < argc; i++) {
        size_t package_len = strlen(argv[i]);
        if (current_len + package_len + 2 > sizeof(command)) { // +1 for space, +1 for null terminator
            fprintf(stderr, "Command string exceeds buffer size while adding package: %s\n", argv[i]);
            return;
        }
        strcat(command, " ");
        strcat(command, argv[i]);
        current_len += package_len + 1;
    }

    execute_command(command);
}

void handle_updateall() {
    printf("\nUpdating all flathub packages...\n");
    if (execute_command("flatpak update -y") != EXIT_SUCCESS) {
        fprintf(stderr, "Flatpak update failed.\n");
    }
    printf("\nAll updates attempted.\n");
}

void handle_backup(const char *filename) {
    if (filename == NULL) {
        fprintf(stderr, "Backup filename not provided.\n");
        return;
    }

    char command[512];
    FILE *outfile = fopen(filename, "w");
    if (outfile == NULL) {
        perror("Failed to open backup file for writing");
        return;
    }
    printf("Backing up packages to %s\n", filename);

    printf("Backing up Flatpak applications...\n");
    snprintf(command, sizeof(command), "flatpak list --app --columns=application");
    FILE *pipe = popen(command, "r");
    if (pipe) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            fprintf(outfile, "flatpak:%s", buffer);
        }
        pclose(pipe);
    } else {
        fprintf(stderr, "Failed to execute flatpak list\n");
    }

    fclose(outfile);
    printf("Backup completed to %s\n", filename);
}

void handle_restore(const char *filename) {
    if (filename == NULL) {
        fprintf(stderr, "Restore filename not provided.\n");
        return;
    }

    FILE *infile = fopen(filename, "r");
    if (infile == NULL) {
        perror("Failed to open backup file for reading");
        return;
    }
    printf("Restoring packages from %s\n", filename);

    char line[512];
    char command[1024];

    while (fgets(line, sizeof(line), infile) != NULL) {
        line[strcspn(line, "\n")] = 0; // Remove newline

        if (strncmp(line, "flatpak:", 8) == 0) {
            char *app_id = line + 8;
            if (strlen(app_id) > 0) {
                snprintf(command, sizeof(command), "flatpak install -y flathub %s", app_id);
                execute_command(command);
            }
        } else {
            if (strlen(line) > 0) { // Only print if line is not empty and not recognized
                fprintf(stderr, "Skipping unrecognized line in backup file: %s\n", line);
            }
        }
    }
    fclose(infile);

    printf("\nPackage restoration process attempted.\n");
}

void handle_define_os() {
    const char* os_base = get_os_base_name();
    printf("%s\n", os_base);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        fprintf(stderr, "Available commands:\n");
        fprintf(stderr, "  put <action> <package1> [package2...]\n");
        fprintf(stderr, "      Actions: install, remove, update (handled by run_put)\n");
        fprintf(stderr, "  install <manager> <package1> [package2...]\n");
        fprintf(stderr, "      Managers: flathub\n");
        fprintf(stderr, "  updateall\n");
        fprintf(stderr, "      Updates packages from flathub\n");
        fprintf(stderr, "  backup <filename.txt>\n");
        fprintf(stderr, "      Backs up installed flathub packages\n");
        fprintf(stderr, "  restore <filename.txt>\n");
        fprintf(stderr, "      Restores packages from a backup file\n");
        fprintf(stderr, "  define os\n"); // New usage line
        fprintf(stderr, "      Identifies the base operating system\n");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "put") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Usage: %s put <action> <package1> [package2...]\n", argv[0]);
            fprintf(stderr, "Actions: install, remove, update\n");
            return EXIT_FAILURE;
        }
        run_put(argc - 2, &argv[2]);
    } else if (strcmp(argv[1], "install") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Usage: %s install <manager> <package1> [package2...]\n", argv[0]);
            fprintf(stderr, "Managers: flathub\n");
            return EXIT_FAILURE;
        }
        handle_install(argc - 2, &argv[2]);
    } else if (strcmp(argv[1], "updateall") == 0) {
        if (argc != 2) {
            fprintf(stderr, "Usage: %s updateall\n", argv[0]);
            return EXIT_FAILURE;
        }
        handle_updateall();
    } else if (strcmp(argv[1], "backup") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s backup <filename.txt>\n", argv[0]);
            return EXIT_FAILURE;
        }
        handle_backup(argv[2]);
    } else if (strcmp(argv[1], "restore") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s restore <filename.txt>\n", argv[0]);
            return EXIT_FAILURE;
        }
        handle_restore(argv[2]);
    } else if (strcmp(argv[1], "define") == 0) { // New command block
        if (argc != 3) {
            fprintf(stderr, "Usage: %s define <subcommand>\n", argv[0]);
            fprintf(stderr, "Available subcommands for define: os\n");
            return EXIT_FAILURE;
        }
        if (strcmp(argv[2], "os") == 0) {
            handle_define_os();
        } else {
            fprintf(stderr, "Unknown subcommand for define: %s\n", argv[2]);
            fprintf(stderr, "Available subcommands for define: os\n");
            return EXIT_FAILURE;
        }
    } else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        fprintf(stderr, "Run '%s' without arguments to see usage.\n", argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
#endif

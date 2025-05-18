#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <sys/stat.h> // For stat() in detect_distro_base

// Forward declarations
void run_put(int argc, char *argv[]); // Actual definition is expected in omniput.c
void detect_distro_base();

// Retained from original, as it's a general utility
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

void print_distro_token(const char* token_str) {
    if (token_str && *token_str) {
        char buffer[256];
        strncpy(buffer, token_str, sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';

        char* start = buffer;
        char* end = buffer + strlen(buffer) - 1;
        if (*start == '"' && end > start) { 
            start++;
            if (*end == '"') {
                *end = '\0';
            }
        }
        
        char *token = strtok(start, " ");
        while (token != NULL) {
            if (strlen(token) > 0) { 
                 printf("\"%s\" ", token);
            }
            token = strtok(NULL, " ");
        }
    }
}

void detect_distro_base() {
    FILE *fp;
    char line[256];
    char id[256] = "";
    char id_like[256] = "";
    int found_id_like = 0;
    int found_id = 0;
    int printed_something = 0;

    fp = fopen("/etc/os-release", "r");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "ID_LIKE=", 8) == 0) {
                strncpy(id_like, line + 8, sizeof(id_like) - 1);
                id_like[sizeof(id_like) - 1] = '\0';
                id_like[strcspn(id_like, "\n")] = 0; 
                found_id_like = 1;
            } else if (strncmp(line, "ID=", 3) == 0) {
                strncpy(id, line + 3, sizeof(id) - 1);
                id[sizeof(id) - 1] = '\0';
                id[strcspn(id, "\n")] = 0; 
                found_id = 1;
            }
        }
        fclose(fp);

        if (found_id_like && strlen(id_like) > 0) {
            print_distro_token(id_like);
            printed_something = 1;
        }
        
        if (!printed_something && found_id && strlen(id) > 0) {
            print_distro_token(id);
            printed_something = 1;
        }
    }

    struct stat st;
    if (stat("/etc/debian_version", &st) == 0) {
        if (!strstr(id_like, "debian") && !strstr(id, "debian")) { 
             printf("\"debian\" ");
             printed_something = 1;
        }
    }
    if (stat("/etc/arch-release", &st) == 0) {
        if (!strstr(id_like, "arch") && !strstr(id, "arch")) {
            printf("\"arch\" ");
            printed_something = 1;
        }
    }
    if (stat("/etc/fedora-release", &st) == 0) {
        if (!strstr(id_like, "fedora") && !strstr(id, "fedora")) {
            printf("\"fedora\" ");
            printed_something = 1;
        }
    }
    if (stat("/etc/redhat-release", &st) == 0 || stat("/etc/centos-release", &st) == 0) {
         if (!strstr(id_like, "rhel") && !strstr(id, "rhel") &&
             !strstr(id_like, "centos") && !strstr(id, "centos")) {
            printf("\"rhel\" "); 
            printed_something = 1;
        }
    }
    if (stat("/etc/SuSE-release", &st) == 0 || stat("/etc/suse-release", &st) == 0) {
        if (!strstr(id_like, "suse") && !strstr(id, "suse") &&
            !strstr(id_like, "opensuse") && !strstr(id, "opensuse")) {
            printf("\"suse\" ");
            printed_something = 1;
        }
    }
    if (stat("/etc/gentoo-release", &st) == 0) {
         if (!strstr(id_like, "gentoo") && !strstr(id, "gentoo")) {
            printf("\"gentoo\" ");
            printed_something = 1;
        }
    }
    
    if (printed_something) {
        printf("\n"); 
    } else {
        fprintf(stderr, "Could not determine distro base.\n");
    }
}


int main(int argc, char *argv[]) {
   if (argc < 2) {
       fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
       fprintf(stderr, "Available commands:\n");
       fprintf(stderr, "  put <action> [args...]\n");
       fprintf(stderr, "      Primary command for package operations (e.g., install, remove).\n");
       fprintf(stderr, "      Supported actions are determined by omniput.c.\n");
       fprintf(stderr, "      Example: %s put install <package1> [package2...]\n", argv[0]);
       fprintf(stderr, "  defdis\n");
       fprintf(stderr, "      Detects and prints the base distribution(s) (e.g., \"debian\" \"arch\").\n");
       return EXIT_FAILURE;
   }

   if (strcmp(argv[1], "put") == 0) {
       if (argc < 3) { 
           fprintf(stderr, "Usage: %s put <action> [args...]\n", argv[0]);
           fprintf(stderr, "Supported actions (e.g., install, remove) are determined by omniput.c.\n");
           return EXIT_FAILURE;
       }
       run_put(argc - 2, &argv[2]);
   } else if (strcmp(argv[1], "defdis") == 0) {
       if (argc != 2) {
           fprintf(stderr, "Usage: %s defdis\n", argv[0]);
           return EXIT_FAILURE;
       }
       detect_distro_base();
   } else {
       fprintf(stderr, "Unknown command: %s\n", argv[1]);
       fprintf(stderr, "Run '%s' without arguments to see usage.\n", argv[0]);
       return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}


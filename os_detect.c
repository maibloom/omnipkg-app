#include "os_detect.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For general utilities, though not strictly needed for this version
#include <sys/stat.h> // For checking file existence

// Helper function to check if a file exists
static int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

// Helper function to read the first relevant ID from /etc/os-release
// Looks for ID= or ID_LIKE=
// Returns a dynamically allocated string that the caller must free, or NULL.
// For simplicity in get_os_base_name, we'll use it to guide, but get_os_base_name will return static strings.
static char* parse_os_release_for_id(const char* key_prefix) {
    FILE *fp = fopen("/etc/os-release", "r");
    if (!fp) {
        return NULL;
    }

    char line[256];
    char *value = NULL;
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, key_prefix, strlen(key_prefix)) == 0) {
            char *val_start = line + strlen(key_prefix);
            // Remove quotes if present
            if (*val_start == '"') {
                val_start++;
                char *end_quote = strrchr(val_start, '"');
                if (end_quote) {
                    *end_quote = '\0';
                }
            } else {
                // Remove trailing newline
                val_start[strcspn(val_start, "\n")] = 0;
            }
            value = strdup(val_start); // Remember to free this
            break;
        }
    }
    fclose(fp);
    return value;
}


const char* get_os_base_name() {
    char *id = NULL;
    char *id_like = NULL;

    if (file_exists("/etc/os-release")) {
        id = parse_os_release_for_id("ID=");
        id_like = parse_os_release_for_id("ID_LIKE=");

        // Check ID_LIKE first, as it often gives the true "base"
        if (id_like) {
            // ID_LIKE can be a space-separated list, e.g., "debian ubuntu"
            if (strstr(id_like, "debian")) {
                free(id); free(id_like);
                return "debian";
            }
            if (strstr(id_like, "arch")) {
                free(id); free(id_like);
                return "arch";
            }
            if (strstr(id_like, "fedora")) {
                free(id); free(id_like);
                return "fedora";
            }
            if (strstr(id_like, "suse")) { // For openSUSE, SUSE Linux Enterprise
                free(id); free(id_like);
                return "suse";
            }
             if (strstr(id_like, "rhel") || strstr(id_like, "centos")) { // For RHEL, CentOS
                free(id); free(id_like);
                return "rhel";
            }
        }

        // If ID_LIKE didn't give a clear base, check ID
        if (id) {
            if (strcmp(id, "debian") == 0 || strcmp(id, "ubuntu") == 0 || strcmp(id, "raspbian") == 0 || strcmp(id, "linuxmint") == 0 || strcmp(id, "pop") == 0) {
                free(id); free(id_like);
                return "debian"; // Grouping Ubuntu, Mint, Pop as Debian-based
            }
            if (strcmp(id, "arch") == 0 || strcmp(id, "manjaro") == 0 || strcmp(id, "endeavouros") == 0) {
                free(id); free(id_like);
                return "arch"; // Grouping Manjaro as Arch-based
            }
            if (strcmp(id, "fedora") == 0) {
                free(id); free(id_like);
                return "fedora";
            }
            if (strcmp(id, "opensuse-tumbleweed") == 0 || strcmp(id, "opensuse-leap") == 0 || strcmp(id, "sles") == 0) {
                free(id); free(id_like);
                return "suse";
            }
            if (strcmp(id, "rhel") == 0 || strcmp(id, "centos") == 0 || strcmp(id, "almalinux") == 0 || strcmp(id, "rocky") == 0) {
                free(id); free(id_like);
                return "rhel";
            }
        }
        free(id); // Free if allocated and not used
        free(id_like); // Free if allocated and not used
    }

    // Fallback checks if /etc/os-release wasn't definitive or doesn't exist
    if (file_exists("/etc/arch-release")) {
        return "arch";
    }
    if (file_exists("/etc/debian_version")) {
        return "debian";
    }
    if (file_exists("/etc/fedora-release")) {
        return "fedora";
    }
    if (file_exists("/etc/redhat-release")) { // General Red Hat family
        return "rhel";
    }
    if (file_exists("/etc/SuSE-release") || file_exists("/etc/suse-release")) {
        return "suse";
    }

    return "unknown";
}

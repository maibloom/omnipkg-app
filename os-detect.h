#ifndef OS_DETECT_H
#define OS_DETECT_H

// Function to determine the base operating system
// Returns a string literal (e.g., "debian", "arch", "fedora", "suse", "rhel", "unknown")
// The caller should not free the returned string.
const char* get_os_base_name();

#endif // OS_DETECT_H

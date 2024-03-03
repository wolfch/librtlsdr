#include "ipcutil.h"
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdint.h>

#include <sys/mman.h>
#include <fcntl.h>
//#include <semaphore.h>

#include <errno.h>
#include <string.h>
#include <syslog.h>
#include <sys/shm.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <unistd.h>

#ifdef __linux__
#include <stdint.h>
#elif __APPLE__
#include <inttypes.h>
#endif

char misc_msg[1024];
sem_t* shm_mutex;
int shm_fd;

void log_lib_error(const char* format, ...) {
    va_list args;
    char suffix[32];
    sprintf(suffix, " - errno = %d", errno);
    va_start(args, format);
    memset(misc_msg, 0, sizeof(misc_msg));
    vsprintf(misc_msg, format, args);
    strcat(misc_msg, suffix);
    perror(misc_msg);
    va_end(args);
}

int msleep(int millis) {
    if (usleep(millis * 1000) < 0) {
        log_lib_error("usleep(%d)", millis);
        return -1;
    }
    return 0;
}

int strtos(const char* s, int16_t* value) {
    char* endptr;
    long lvalue = strtol(s, &endptr, 10); // Convert string to long

    if (*endptr != '\0' && *endptr != '\n') {
        fprintf(stderr, "Invalid input: %s\n", s);
        return 1;
    }

    if (lvalue < INT16_MIN || lvalue > INT16_MAX) {
        fprintf(stderr, "Value out of range: %" PRId16 "\n", (int16_t)lvalue);
        return 1;
    }

    *value = (int16_t)lvalue;
    return 0;
}

void strip(char *str) {
    size_t start = 0;
    size_t end = 0;
    size_t len = 0;

    if (str == NULL) {
        return;
    }

    len = strlen(str);

    // Strip leading whitespace
    start = 0;
    while (start < len && isspace((unsigned char)str[start])) {
        start++;
    }

    // Strip trailing whitespace
    end = len;
    while (end > start && isspace((unsigned char)str[end - 1])) {
        end--;
    }

    // Shift the non-whitespace characters to the beginning of the string
    memmove(str, str + start, end - start);
    str[end - start] = '\0';
}

int count_char(const char* str, char ch) {
    int count = 0;
    for (int i = 0; str[i] != '\0'; i++)
        if (str[i] == ch)
            count++;
    return count;
}

int
shm_init(void** ptr, int size, const char* shm_name) {
    char sem_name[64];
    strcat(strcat(sem_name, shm_name), "_sem");

    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        log_lib_error("shm_open");
        return errno;
    }
    ftruncate(shm_fd, size);

    *ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (*ptr == MAP_FAILED) {
        log_lib_error("mmap");
        return errno;
    }

    shm_mutex = sem_open(sem_name, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (shm_mutex == SEM_FAILED) {
        log_lib_error("sem_open");
        return errno;
    }
    return 0;
}

int
shm_cleanup(const char *shm_name, void* ptr, int size) {
    char sem_name[64];
    strcat(strcat(sem_name, shm_name), "_sem");

    sem_close(shm_mutex);
    sem_unlink(sem_name);
    munmap(ptr, size);
    shm_unlink(shm_name);
    return 0;
}

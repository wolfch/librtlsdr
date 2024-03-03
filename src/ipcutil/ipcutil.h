#ifndef _IPCUTIL_H
# define _IPCUTIL_H
#ifdef __linux__
#  include <stdint.h>
#elif __APPLE__
#  include <inttypes.h>
#endif
#include <semaphore.h>

#define str(x) ({ char _str[strlen(x) + 1]; strcpy(_str, (x)); _str; })

// signed 32 bit to unsiged 8 bit
#define S32TOU8(x) ((uint8_t)((x) & 0x000000ff))
// signed 32 bit to signed 16 bit
#define S32TO16(x) ((int16_t)((x) & 0x0000ffff))

extern char misc_msg[1024];

extern sem_t* shm_mutex;
extern int shm_fd;

int strtos(const char* s, int16_t* value);
void strip(char *str);
int count_char(const char* str, char ch);
void log_lib_error(const char* format, ...);
int shm_init(void** ptr, int size, const char* shm_name);
int shm_cleanup(const char *shm_name, void* ptr, int size);

#endif /*_IPCUTIL_H*/

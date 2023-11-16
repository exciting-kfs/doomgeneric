#ifndef _DOOMGENERIC_SYSCALL_KFS_H
#define _DOOMGENERIC_SYSCALL_KFS_H

#include <stdint.h>

#define SYS_attach_tty 10003
#define SYS_detach_tty 10002
#define SYS_get_key_state 10001
#define SYS_draw_buffer 10000

void draw_buffer(uint32_t *buf);

void attach_tty();

void detach_tty();

void get_key_state(uint32_t *buf);

#endif // _DOOMGENERIC_SYSCALL_KFS_H

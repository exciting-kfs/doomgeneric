#include <syscall.h>

#include "syscall_kfs.h"

void draw_buffer(uint32_t *buf) {
        syscall(SYS_draw_buffer, buf);
}

void attach_tty() {
        syscall(SYS_attach_tty);
}

void detach_tty() {
        syscall(SYS_detach_tty);
}

void get_key_state(uint32_t *buf) {
        syscall(SYS_get_key_state, buf);
}

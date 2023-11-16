#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "doomgeneric.h"
#include "doomkeys.h"
#include "syscall_kfs.h"

void DG_SleepMs(uint32_t ms) {
        long sec = ms / 1000;
        long nsec = (ms % 1000) * 1000 * 1000;

        struct timespec ts = {
            .tv_nsec = nsec,
            .tv_sec = sec,
        };

        nanosleep(&ts, NULL);
}

uint32_t DG_GetTicksMs() {
        struct timespec ts;

        clock_gettime(CLOCK_MONOTONIC, &ts);

        return (ts.tv_nsec / 1000 / 1000) + (ts.tv_sec * 1000);
}

#define WQ_BUFFER_SIZE 4096

typedef struct KeyEvent {
        unsigned char key;
        unsigned char is_pressed;
} KeyEvent;

typedef struct KeyQueue {
        KeyEvent data[WQ_BUFFER_SIZE];
        size_t head;
        size_t tail;
} KeyQueue;

int wq_is_empty(KeyQueue *self) {
        return self->head == self->tail;
}

static inline size_t wq_next_index(size_t idx) {
        return (idx + 1) % WQ_BUFFER_SIZE;
}

int wq_is_full(KeyQueue *self) {
        return wq_next_index(self->tail) == self->head;
}

void wq_push(KeyQueue *self, KeyEvent ev) {
        self->data[self->tail] = ev;
        if (wq_is_full(self)) {
                self->head = wq_next_index(self->head);
        }
        self->tail = wq_next_index(self->tail);
}

KeyEvent wq_pop(KeyQueue *self) {
        KeyEvent ev = self->data[self->head];

        self->head = wq_next_index(self->head);

        return ev;
}

void wq_init(KeyQueue *self) {
        self->head = 0;
        self->tail = 0;
}

uint32_t prev_key[8];
uint32_t curr_key[8];
KeyQueue key_queue;

const unsigned char keymap[][2] = {
    {0xc3, KEY_RIGHTARROW},
    {0xc2, KEY_LEFTARROW},

    {0xc0, KEY_UPARROW},
    {0x16, KEY_UPARROW},

    {0xc1, KEY_DOWNARROW},
    {0x12, KEY_DOWNARROW},

    {0x0, KEY_STRAFE_L},
    {0x3, KEY_STRAFE_R},
    {0x4, KEY_USE},
    {0x35, KEY_FIRE},
    {0xe1, KEY_ESCAPE},
    {0xe7, KEY_ENTER},
    {0xe6, KEY_TAB},
    {0x42, KEY_RSHIFT},
    {0x43, KEY_RSHIFT},

    {0x18, 'y'},
    {0x20, '0'},
    {0x21, '1'},
    {0x22, '2'},
    {0x23, '3'},
    {0x24, '4'},
    {0x25, '5'},
    {0x26, '6'},
    {0x27, '7'},
    {0x28, '8'},
    {0x29, '9'},

    {0, 0},
};

void DG_DrawFrame() {
        draw_buffer(DG_ScreenBuffer);

        memcpy(prev_key, curr_key, sizeof(prev_key));
        get_key_state(curr_key);

        for (size_t i = 0; keymap[i][0] || keymap[i][1]; ++i) {
                unsigned char kfs_key = keymap[i][0];
                unsigned char doom_key = keymap[i][1];

                size_t arr_idx = kfs_key / 32;
                size_t bit_idx = kfs_key % 32;

                if ((prev_key[arr_idx] & (1U << bit_idx)) !=
                    (curr_key[arr_idx] & (1U << bit_idx))) {
                        KeyEvent ev = {
                            .is_pressed = (curr_key[arr_idx] & (1U << bit_idx)) != 0,
                            .key = doom_key,
                        };
                        wq_push(&key_queue, ev);
                }
        }
}

int DG_GetKey(int *pressed, unsigned char *key) {
        if (!wq_is_empty(&key_queue)) {
                KeyEvent ev = wq_pop(&key_queue);

                *pressed = ev.is_pressed;
                *key = ev.key;

                return 1;
        }

        return 0;
}

void DG_SetWindowTitle(const char *title) {
}

void DG_Init() {
        detach_tty();
        atexit(attach_tty);
        get_key_state(curr_key);
        wq_init(&key_queue);
}

int main(int argc, char *argv[]) {
        doomgeneric_Create(argc, argv);

        for (;;)
                doomgeneric_Tick();

        return 0;
}

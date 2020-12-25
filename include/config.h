#ifndef CONFIG_H
#define CONFIG_H

////////////////////////////////
// default soy example config // 
////////////////////////////////

// number of times to allow a blacklisted word to be typed
#define STRIKE_NUM      3

// words to blacklist. Kill xorg if any of these words are typed. Note null-termination.
#define BLACKLIST       "haters\0", "taboo\0", "badword\0"

// words to whitelist. Autocomplete these words if N_MATCH_COMPLET number of characters have been typed
#define WHITELIST       "apt install\0", "merry christmas\0", "autocomplete me\0", "hello world\0"  
#define N_MATCH_COMPLET    2,                3,                    4,                  3

#endif

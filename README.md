# kmod-keyfun
A loadable linux kernel module for autocompleting and blacklisting certain words system wide. Tested on Linux 5.4.0 with a PS/2 laptop keyboard. For other setups (especially, USB keyboards), your mileage may vary. 

# whitelisted words
In this context, whitelisting words implies autocompletion IFF a certain number of initial characters of a whitelisted word is typed. For example: 

```
    whitelist:              gigantic 
    num_match_comp:         2

    person types letter:    g
    person types letter:    i
    person types letter:    g
    --> (3 matching letters, match at index 2) --> 
    keyboard types letter:  i
    keyboard types letter:  g
    keyboard types letter:  a
    ...
    keyboard types letter:  c
```

# blacklisting words
You can blacklist words. If a person types a blacklisted word, the kernel will kill Xorg (i.e. window manager will get restarted). If this incident happens `STRIKE_NUM` times, the kernel will commit suicide by triple faulting and your machine will reset.
`Warning:` This is as if killing power to your computer, so data corruption may occur xD. 

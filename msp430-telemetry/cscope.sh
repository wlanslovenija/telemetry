#!/bin/bash

(echo /usr/msp430/include/msp430g2553.h; find . /usr/msp430/include/ -type f | egrep '\.[ch]$' | grep -v '\/msp430[a-zA-Z0-9]*\.h') | cscope -Rb -i- 

#!/bin/sh

openocd -f board/stm32f4discovery.cfg -c "init; reset halt; flash write_image erase autogrow.bin 0x08000000; reset run; shutdown"

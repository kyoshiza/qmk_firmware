## Thumbshift69 keyboard

65% keyboard that has split spacebar layout  
How to write bootloader and fuse:  

    make thumbshift69:nicola:production
    avrdude -p m32u4 -P COM4 -c avrisp -U flash:w:thumbshift69_nicola_production.hex:i
    avrdude -p m32u4 -P COM4 -c avrisp -U hfuse:w:0xd9:m
    avrdude -p m32u4 -P COM4 -c avrisp -U lfuse:w:0x5e:m
    avrdude -p m32u4 -P COM4 -c avrisp -U efuse:w:0xc6:m

http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega32u4&LOW=5E&HIGH=D8&EXTENDED=C6&LOCKBIT=FF

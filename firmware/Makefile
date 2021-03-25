rstctlr.hex:	rstctlr.elf
		avr-objcopy -j .text -j .data -O ihex rstctlr.elf rstctlr.hex

rstctlr.elf:	rstctlr.o
		avr-gcc --lto -mmcu=attiny2313a -o rstctlr.elf rstctlr.o

rstctlr.o:	rstctlr.c
		#avr-gcc -Os -Wall -Werror -DF_CPU=12000000UL -mmcu=attiny2313a -c rstctlr.c -o rstctlr.o
		avr-gcc -Os -Wall -Werror -DF_CPU=1500000UL -mmcu=attiny2313a -c rstctlr.c -o rstctlr.o

upload:		rstctlr.hex
		avrdude -c usbtiny -p t2313 -U flash:w:rstctlr.hex -F

fuse:
		#avrdude -c usbtiny -p t2313 -U lfuse:w:0x64:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m -F
		avrdude -c usbtiny -p t2313 -U lfuse:w:0x60:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m -F

extclk:
		ft232r_prog --cbus4 Clk12

clean:
		rm -f rstctlr.hex rstctlr.elf rstctlr.o

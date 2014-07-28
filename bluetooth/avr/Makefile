# write to device: sudo avrdude -p m48 -P usb -c DRAGON_ISP -U flash:w:a.hex

all: trigger

trigger.out: trigger.c
	avr-gcc -mmcu=attiny2313 -Os trigger.c -o trigger.out

trigger: trigger.out
	avr-objcopy -j .text -j .data -O ihex trigger.out trigger.hex

write: all
	avrdude -p t2313 -P usb -c DRAGON_ISP -U flash:w:trigger.hex

clean:
	rm *.out

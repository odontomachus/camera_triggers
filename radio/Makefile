# write to device: sudo avrdude -p m48 -P usb -c DRAGON_ISP -U flash:w:a.hex

all: trigger remote

trigger.out: trigger.c trigger.h
	avr-gcc -mmcu=attiny85 -Os trigger.c -o trigger.out

remote.out: remote.c trigger.h
	avr-gcc -mmcu=attiny85 -Os remote.c -o remote.out

trigger: trigger.out
	avr-objcopy -j .text -j .data -O ihex trigger.out trigger.hex

remote: remote.out
	avr-objcopy -j .text -j .data -O ihex remote.out remote.hex

write_trigger: trigger
	avrdude -p t85 -P usb -c DRAGON_ISP -U flash:w:trigger.hex

write_remote: remote
	avrdude -p t85 -P /dev/ttyACM0 -c avrispv2 -U flash:w:remote.hex

clean:
	rm *.out

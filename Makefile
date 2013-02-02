CC      = gcc
PWD    := $(shell pwd)
CFLAGS  = -Wall -m32 -Wa,--32 -nostdlib -nostdinc -ffreestanding -I$(PWD)
LDFLAGS = --warn-common -melf_i386 --strip-all

OBJECTS = bootloader/multiboot.o \
      kernel/hardware/input_output/screen/vga.o \
	  kernel/startup.o

KERNEL   = roentgenium.elf
MULTIBOOT_IMAGE = roentgenium.iso

# Main target
all: $(KERNEL)

$(KERNEL): $(OBJECTS)
	$(LD) $(LDFLAGS) -T ./linker.ld -o $@ $^

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

%.o: %.S
	$(CC) -I$(PWD) -c $< $(CFLAGS) -o $@

cdrom: $(KERNEL)
	mv $(KERNEL) bootloader/grub/
	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot \
	-boot-load-size 4 -boot-info-table -o $(MULTIBOOT_IMAGE) bootloader/grub/

clean:
	$(RM) *.iso
	$(RM) bootloader/*.o
	$(RM) bootloader/grub/roentgenium.elf
	$(RM) kernel/*.o
	$(RM) kernel/hardware/input_output/screen/*.o
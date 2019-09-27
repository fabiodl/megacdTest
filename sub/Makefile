CC=/opt/gendev/bin/m68k-elf-gcc
CCFLAGS=  -m68000 -Wall -fno-builtin  -O3 -fuse-linker-plugin -fno-web -fno-gcse -fno-unit-at-a-time -fomit-frame-pointer -flto 
manual:	 *.c *.s
	$(CC) $(CCFLAGS) -o out/rom.out -T mcd.ld *.c *.s 
	/opt/gendev/bin/m68k-elf-objcopy -O binary out/rom.out out/rom.bin

clean:
	@rm out/*

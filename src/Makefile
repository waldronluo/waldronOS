ipl.bin : ipl.nas Makefile
	../z_tools/nask.exe ipl.nas ipl.bin ipl.lst
waldronos.img : ipl.bin Makefile
	../z_tools/edimg.exe imgin:../z_tools/fdimg0at.tek wbinimg src:ipl.bin len:512 from:0 to:0 imgout:waldronos.img
run: waldronos.img Makefile
	qemu-system-i386 -hda waldronos.img
clean : 
	rm -f ipl.bin ipl.lst waldronos.img
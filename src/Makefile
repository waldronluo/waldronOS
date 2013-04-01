TOOLPATH	= ../z_tools/
MAKE		= $(TOOLPATH)make.exe -r
NASK		= $(TOOLPATH)nask.exe
EDIMG		= $(TOOLPATH)edimg.exe
QEMU		= qemu-system-i386 -fda


default :run Makefile

ipl.bin : ipl.nas Makefile
	$(NASK) ipl.nas ipl.bin ipl.lst

waldronos.sys : waldronos.nas Makefile
		$(NASK) waldronos.nas waldronos.sys waldronos.lst
waldronos.img : ipl.bin waldronos.sys Makefile
	$(EDIMG) imgin:../z_tools/fdimg0at.tek wbinimg src:ipl.bin len:512 from:0 to:0 copy from:waldronos.sys to:@: imgout:waldronos.img


run: waldronos.img Makefile
	$(QEMU) waldronos.img


clean : 
	rm -f ipl.bin ipl.lst waldronos.img waldronos.sys waldronos.lst

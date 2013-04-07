OBJS_BOOTPACK = bootpack.obj naskfunc.obj mmm.obj

TOOLPATH = ../z_tools/
INCPATH  = ../z_tools/waldronos/
DIYPATH  = ../diy_tools/
MAKE     = make -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
RULEFILE = $(TOOLPATH)haribote/haribote.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
MAKEFONT = $(TOOLPATH)makefont.exe
COPY     = cp
DEL      = rm -rf

default : run
#	$(MAKE) img

hankaku.h: hankaku.txt Makefile
	python $(DIYPATH)fontmaker.py 

ipl10.bin : ipl10.nas Makefile
	$(NASK) ipl10.nas ipl10.bin ipl10.lst

asmhead.bin : asmhead.nas Makefile
	$(NASK) asmhead.nas asmhead.bin asmhead.lst


%.gas : %.c Makefile
	$(CC1) -o $*.gas $*.c 

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

bootpack.gas : bootpack.c hankaku.h Makefile 
	$(CC1) -o bootpack.gas hankaku.h bootpack.c

bootpack.nas : bootpack.gas Makefile
	$(GAS2NASK) bootpack.gas bootpack.nas

naskfunc.obj: naskfunc.nas Makefile
	$(NASK) naskfunc.nas naskfunc.obj naskfunc.lst

bootpack.obj : bootpack.nas Makefile
	$(NASK) bootpack.nas bootpack.obj bootpack.lst

bootpack.bim : $(OBJS_BOOTPACK)  Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map $(OBJS_BOOTPACK) 
# 3MB+64KB=3136KB

bootpack.hrb : bootpack.bim Makefile
	$(BIM2HRB) bootpack.bim bootpack.hrb 0

waldronos.sys : asmhead.bin bootpack.hrb Makefile
	cat  asmhead.bin bootpack.hrb > waldronos.sys

waldronos.img : ipl10.bin waldronos.sys Makefile
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:ipl10.bin len:512 from:0 to:0 \
		copy from:waldronos.sys to:@: \
		imgout:waldronos.img

img : 
	$(MAKE) waldronos.img

run :waldronos.img
	qemu-system-i386 -vga std -fda waldronos.img
install :
	$(MAKE) img
	$(IMGTOL) w a: waldronos.img

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.obj
	-$(DEL) bootpack.nas
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.hrb
	-$(DEL) waldronos.sys
	-$(DEL) waldronos.img
	-$(DEL) hankaku.h
gitclean: 
	git rm *.bin
	git rm *.lst
	git rm *.gas
	git rm *.obj
	git rm bootpack.nas
	git rm bootpack.map
	git rm bootpack.bim
	git rm bootpack.hrb
	git rm waldronos.sys
	git rm waldronos.img
	git rm hankaku.h
src_only :
	$(MAKE) clean
	-$(DEL) waldronos.img

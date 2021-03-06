OBJS_BOOTPACK = bootpack.obj graphic.obj naskfunc.obj dsctbl.obj \
				int.obj queue.obj keyboard.obj mouse.obj \
				memory.obj sheet.obj timer.obj mtask.obj \
				console.obj file.obj

TOOLPATH = ../z_tools/
INCPATH  = ../z_tools/waldronos/
DIYPATH  = ../diy_tools/
MAKE     = make 
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
QEMU	 = qemu-system-i386 -m 32 -localtime -vga std -fda
COPY     = cp
DEL      = rm -rf

default : 
	-$(MAKE) run -slient 
	$(MAKE) clean -s

helloa.bim : helloa.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:helloa.bim stack:1k map:helloa.map helloa.obj

helloa.hrb : helloa.bim Makefile
	$(BIM2HRB) helloa.bim helloa.hrb 0

bug1.bim : bug1.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bug1.bim map:bug1.map bug1.obj a_nask.obj

bug1.hrb : bug1.bim Makefile
	$(BIM2HRB) bug1.bim bug1.hrb 0

bug2.bim : bug2.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bug2.bim map:bug2.map bug2.obj a_nask.obj

bug2.hrb : bug2.bim Makefile
	$(BIM2HRB) bug2.bim bug2.hrb 0

winhelo.bim : winhelo.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:winhelo.bim stack:1k map:winhelo.map winhelo.obj a_nask.obj

winhelo.hrb : winhelo.bim Makefile
	$(BIM2HRB) winhelo.bim winhelo.hrb 40K

hello.bim : hello.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:hello.bim stack:1k map:hello.map hello.obj a_nask.obj

hello.hrb : hello.bim Makefile
	$(BIM2HRB) hello.bim hello.hrb 0

hankaku.c: hankaku.txt Makefile
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

bootpack.gas : bootpack.c hankaku.c Makefile 
	$(CC1) -o bootpack.gas hankaku.c bootpack.c

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

waldronos.img : ipl10.bin waldronos.sys winhelo.hrb bug1.hrb bug2.hrb hello.hrb helloa.hrb Makefile
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:ipl10.bin len:512 from:0 to:0 \
		copy from:waldronos.sys to:@: \
		copy from:Makefile to:@: \
		copy from:bootpack.c to:@: \
		copy from:hankaku.txt to:@: \
		copy from:note.md to:@: \
		copy from:bug1.hrb to:@: \
		copy from:bug2.hrb to:@: \
		copy from:hello.hrb to:@: \
		copy from:helloa.hrb to:@: \
		copy from:winhelo.hrb to:@: \
		imgout:waldronos.img

img : 
	$(MAKE) waldronos.img

run :waldronos.img
	 $(QEMU) waldronos.img
install :
	$(MAKE) img
	$(IMGTOL) w a: waldronos.img

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.obj
	-$(DEL) *.wal
	-$(DEL) *.bim
	-$(DEL) *.map
	-$(DEL) *.hrb
	-$(DEL) graphic.nas
	-$(DEL) int.nas
	-$(DEL) memory.nas
	-$(DEL) mouse.nas
	-$(DEL) sheet.nas
	-$(DEL) timer.nas
	-$(DEL) queue.nas
	-$(DEL) keyboard.nas
	-$(DEL) dsctbl.nas
	-$(DEL) bootpack.nas
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.hrb
	-$(DEL) waldronos.sys
	-$(DEL) waldronos.img
	-$(DEL) hankaku.h
gitclean: 
	-git rm *.bin
	-git rm *.lst
	-git rm *.gas
	-git rm *.obj
	-git rm bootpack.nas
	-git rm bootpack.map
	-git rm bootpack.bim
	-git rm bootpack.hrb
	-git rm waldronos.sys
	-git rm waldronos.img
	-git rm hankaku.c
src_only :
	$(MAKE) clean
	-$(DEL) waldronos.img

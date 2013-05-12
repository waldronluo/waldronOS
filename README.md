##waldronOS
***笔记君在此***
#**第一天**
关于*cpu*
只是处理电信号的集成电路
关于*汇编*
DB = Data type.
RESB = reserve byte.（从现在开始的地址空出十个字节来，就可以写成RESB 10）
#**第二天**
|||AX accumulator|CX counter|DX data|BX base|SP stack pointer|BP base pointer|SI source index|DI destination index|||
(32位)
EAX 低16位是AX
ECX 低16位是CX
EDX 类推
EBX
ESP
EBP
ESI
EDI
关于*汇编*
mov [678],123 ;678位置上（8位）变成123（01111011）
mov byte [678], 123 ;678位置变成123,679变成0（16位）
关于*org 0x7c00*
0xf0000地址储存bios
bios默认调用0x7c00的操作系统
0x7c00~0x7dff;启动区内容装载地址

#**第三天**
因为BX只有16位，只能处理64K的地址。所以内存最大只有64K。EBX有32位，可以处理4G的地址。所以现在的机子可以用4G的内存。
第三天到加入c的时候代码有比较大的飞跃，因此可以直接从第三天最后开始做最后的。
---------------------------------------------------------------------------------
系统启动的流程：
最初检查VBE是否存在。若存在，则可跳转高分辨率模式。否则只能进入低分辨率模式（320×200）。
然后检查VBE的版本。需要2.0以上（CMP,0x0200）才能使用高分辨率。
然后检查指定的画面模式能否使用。
若是能用，则使用啦。
VBE mode有以下几种，其中0x101,0x103,0x105,0x107分辨是(640×480×8 800×600×8 1024×768×8 1280×1024×8）。使用时要给BX赋值，为VBEMODE+0x4000
接下来是键盘的BIOS。
然后是PIC的初始化，用来保CPU，禁止中断。

---------------------------------------------------------------------------------
#**第五天**
感觉上字体库编译器是坏的，所以打算用python再写一个。扔到diytool下面。
写好之后生成一个hankaku.h这样的东西来调用。因此不用extern char hankaku[4096]这样的东西了。
但是最后发现。。。是因为extern没有引用进来发生的错误。不想搞了，就这样吧。
啊最后还是没做完，先去睡了。明天加油啦啦～

#**第六天**
我去终于到第六天，前面熬过的都是什么呀。
可以接收键盘中断了呀～

#**第八天**
可以接收鼠标中断并且读出获得的数据了。我想很快就能移动鼠标了啦啦啦。
但是鼠标本身写的函数有原罪，因此应该会在原来的基础上改改。
现在的架构跟书上的类似但不是完全（虽然少得可怜）不同啦。

#**第九天**
可以使用鼠标鸟。啦啦。
开始搞内存撒。
这个星期六结束之前（也就是现在的明天）争取搞完第十天的内容。

#**第十天**
Makefile今天一直无法更新，后来又重新pull了一次才终于又能上传了。有大神告诉我到底发生啥事了咩？
还有qemu默认的内存大小是4MB吗？为啥？我为啥每次跑内存检查都只能跑出4MB。。。要死的节奏啊。

naskfunc.nas里面的memtest\_sub函数写错了导致的问题。现在的问题是：QEMU的默认内存有388M这么多？？？
先记个笔记吧。这里使用的内存管理方式是非常简单而且原始的方式。就是找到第一块可用空间就给了。这样不利于内存空洞啥的。反正最后应用程序一多就会出问题。不过现在嘛先不去改它了，迅速走完进度条才是关键。

泪目。。。sheets重搞吧。。。
git reset --hard <commit ID>是获得之前版本的呀。
git push origin master -f 是强制推送

终于做完鸟。面试去。

#**第十一天**
以后一个星期做五天的。
面试就被面出是个渣渣啊。弱爆了！
还是好好背单词去吧唉。

#**第十二天**
感觉写起来好pintos的赶脚。11点半就困了啊。睡了睡了。

#**第十三天**
一開始就讓人去改queue，只是把char改成int嘛。。。還要改名字成32後綴。。。懶得理作者了。。。
還有還有，每年一次的調整程序啥的。。。你到底是在哪裏加入的啊？。。。
不過這東西開啓時間最多都不會超過1天。。還是不加了吧。。。

#**第十四天**
在屏幕分辨率那裏遇到問題。
然後去問了google老師之後還是沒有解決任何問題，最後在作者的官方網站上看到linux版的操作。。觀感是作者的某個朋友。。。[這裏是傳送們](http://bd.tank.jp/misc/haribote_on_linux.html)
就是應該給多幾個qemu的參數。具體如下：
從	- QEMU = qemu-system-i386 -fda
變成+ QEMU = qemu-system-i386 -m 32 -localtime -vga std 

話說最後做出的waldronOS.sys比作者的大了2kkkk.....

#**第十五天**
多任務初步完成。巨簡單哪。。。

#**第十六天**
很疑惑作者宣稱的速度。。。假如task\_a的優先級給高了。反而不是件好事（會更慢）。目測原因是每次人輸入電腦的操作是非常有限的。因此就在不斷地sleep。而sleep的時候，又不斷地調整任務的順序，因此會降低速度。當優先級爲1/10的時候，因爲有許多操作都是在獲得的時間內完成的，切換任務不需要這麼多操作，自然變快了。...不過話說，還有一節就做完了！加油加油加油！！！

華麗麗的下半場。做完了。。。
（若是界面或者哪裏出錯了。用while(1) io\_hlt();來調試，使得系統靜止，然後看看輸出（真是不錯的方法。）


-----------------------------------------------------------
現在先整理一下，系統最底層的應當是中斷，視圖，以及彙編跟機器的一大堆交流。視圖決定了以什麼樣的方式顯示，中斷則可以接受外部數據以及定時器。而定時器又是進程管理的基礎。
-----------------------------------------------------------

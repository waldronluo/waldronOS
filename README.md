##waldronOS
***笔记君在此***

#**第三天**
第三天到加入c的时候代码有比较大的飞跃，因此可以直接从第三天最后开始做最后的。

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

---
title: "第二回 簡易アセンブラとディスアセンブラを作ろう"
layout: page
---

第二回はQEMUを使ってARMのバイナリを動かしたり、バイナリを生成したりディスアセンブルしたりするコードを書きます。

OSの無い環境でプログラムを動かす事で、C言語の土台の理解を深める事を目的とします。

- QEMUのベアボーン環境を使えるようになる
- readelf, objcopy, objdumpなどのコマンドに慣れ親しむ
- 簡単なARMのアセンブリを理解する
- 簡単なアセンブラとディスアセンブラを作る

# 何故アセンブラと逆アセンブラを作るのか？

C言語を勉強しよう、という時に、なんだか関係ない別の言語に見えるアセンブラ言語を学ぼうというのは「なんでよ！？」と思うかもしれません。

そこでここではまず、今回QEMUを使ってバイナリを動かしたり簡易アセンブラや逆アセンブラを作るモチベーションを説明したいと思います。

### C言語を理解するにはアセンブリを知っている方がいい

C言語はアセンブリ言語を知らなくても理解出来ます。
ですが、C言語はアセンブリ言語との親和性が高く、アセンブリ言語の理解があるとずっと出来る事も増えます。

また、初めて低レベルな事をやる時にはobjdumpやobjcopyなど多くのコマンドが一気に出てきて、
実際は大した事無い事でもなんだかすごく圧倒されてしまいがちです。
一度どこかで簡単な題材に対してobjdumpなどを使った経験を持っておくと、
そうした時にもひるまずに済みます。

### バイナリの処理の勉強になる

C言語に一番向いた処理、というのはバイナリを扱う処理だと思います。
逆アセンブラやアセンブラなどはバイナリ処理の代表的な構造を持っているので、
バイナリ処理の練習に適しています。

バイナリ処理はだいたい似たような構造となるので、どれか一つでやった事があるかどうかの差は大きい。
そういう訳でバイナリ処理の題材として手頃なのでやってみよう、というのがあります。

### 低レベルなプログラムは楽しい！

C言語の楽しさといえば低レベルプログラミングでしょう。
低レベルプログラムは、C言語の楽しさの割と中心的な所だと思います。（著者の個人的見解です）

特に規模の小さい低レベルプログラムが楽しい。
規模の小さい低レベルプログラミングは隅から隅まで理解出来ます。
これはプログラムにおいて低レベルプログラムだけでしか味わえない楽しさです。

一方で最近は低レベルでも皆大きなものを作りたがりがちで、
本などを買って来て読んでも、あまりこの楽しさは味わえません。
やはり簡単な例でバイナリを隅から隅まで理解する、というのは、
C言語を学ぶなら一回くらいやっておいてもいいでしょう。

### 何故ARMなのか？

最初x86でこの回を作る気だったのですが、x86のアセンブリ言語は歴史的事情からいろいろ複雑な仕組みになっている為、説明がややこしくなります。

一方ARMはいろいろな事が単純になっている為、低レベルの事を勉強するには手頃なターゲットとなっています。
また、C言語を書く必要があるシチュエーションではARMをターゲットにする方がむしろ多いと思うので、実用性という観点からもARMを覚えておくのは悪くない、と思いARMをターゲットとしました。



# 01 QEMUの環境を作ろう

以下Windows 10のWSLを前提にします。昨今ではたいていの環境で似たり寄ったりのパッケージシステムがあるでしょう。

### QEMUとARM用gccをインストール

今回はARM用のQEMU環境であるqemu-system-armと、ARM用のクロスコンパイルのためのパッケージであるgcc-arm-embeddedをインストールします。

```
sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa
sudo apt update
sudo apt install gcc-arm-embedded

sudo apt install qemu-system-arm
```

### バイナリエディタをインストール

今回はバイナリの中身を調査する為、何らかのバイナリエディタを入れる事がオススメです。
大したサイズじゃないのでodコマンドなどで頑張ってもいいんですが、
今回はバイナリエディタをインストールする事にします。

なんでもいいんですが、私は最近はBz Editorに落ち着きました。

https://github.com/devil-tamachan/binaryeditorbz


### hello_arm.sを動かしてみよう

sources/arm_asm/01_qemu_setup/hello_arm.s

に、UARTにhelloと表示するアセンブリが書いてあります。これを動かしてみましょう。

```
$ arm-none-eabi-as hello_arm.s -o hello_arm.o
$ arm-none-eabi-ld hello_arm.o -Ttext 0x00010000 -o hello_arm.elf
$ arm-none-eabi-objcopy hello_arm.elf -O binary hello_arm.bin
$ qemu-system-arm -M versatilepb -m 128M -nographic -kernel hello_arm.bin -serial mon:stdio
```

終了はC-a xです。

# 02 ARM アセンブリ入門

ldr, str, add, cmp, bne, mov, .ascizなどを説明。
print_loop.sとhello_arm.sの解説。

# 03 アセンブリからC関数を呼ぶ

最低限のセットアップだけで関数を呼ぶ。リンカの使い方とかも。

# 04 リロケーションとリンカスクリプト

hello_arm.o, hello_arm.elf, hello_arm.binのバイナリを比較して、リロケーションの話を簡単に。
逆アセンブルもこの辺か。

# 05 簡易アセンブラを作ろう

リロケーション無しで簡単なアセンブリを作る。0x00010000決め打ち。



----

以下メモ


```
arm-none-eabi-objdump -S hello_arm.o
```

versatilepbはARM926EJ-S。
https://developer.arm.com/docs/ddi0198/latest/preface

ただデータシートの4章くらいが今回程度の知識なら妥当な気がする。
https://developer.arm.com/docs/ddi0027/latest/arm7di-data-sheet


C function call
https://developer.arm.com/products/architecture/cpu-architecture/a-profile/docs/ihi0042/e/procedure-call-standard-for-the-arm-architecture


versatilepb memorymap
https://github.com/hackndev/qemu/blob/master/hw/versatilepb.c
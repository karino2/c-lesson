---
title: "bare metalでのgdb入門"
layout: page
---

[第二回](arm_asm.md)のARMのアセンブリを学習する時には、gdbを使うと理解の助けになります。

ですが、既存のgdbの解説はC言語やデバッグシンボルの存在を前提にした物が主流ですが、
今回はバイナリ自身を見ていくのが中心なので、
少し標準的な使い方と違う所があります。

gdbでのリモートデバッグを普段から使っている人ならbare metal関連でググって出てくる知識を足すだけで一通り用が足せると思いますが、gdbをあまり使った事が無い人にとってはどの知識とどの知識を組み合わせたらいいのか、なかなか分かりにくいと思います。

そこで、ここにbare metalでのバイナリを中心に、gdbを使った人が無い人を対象に簡単に使い方を解説するページを作る事にしました。

内容的には比較的独立しているので、ページを分けておきます。

## hello_arm.binをgdbでデバッグ実行する

gdbを使ってQEMU内のプロセスをデバッグ実行する場合は、リモートデバッグという形になります。
QEMUの中でデバッグモニタと呼ばれるプログラムを動かして、
それとQEMUの外のホスト（つまりQEMUを実行しているWindowsマシンとかMacマシンの事）のgdbプロセスとTCP/IPで通信しながらデバッグする事になります。

### QEMU側のコマンド

普段実行しているQEMUコマンドに-sと-Sのオプションを付けます。例えば以下のようになります。

```
qemu-system-arm -M versatilepb -m 128M -nographic -kernel hello_arm.bin -serial mon:stdio -s -S
```

最後に-sと-Sがついていますね。
こうするとhello_arm.binをロードしてデバッグモニタ付きでデバッグプロセスが立ち上がります。

-sと-Sの意味について知りたい方は以下のサイトなどが参考になりますが、ゆとりは気にせずコピペで行きましょう。

[https://elinux.org/Virtual_Development_Board](https://elinux.org/Virtual_Development_Board)

### ホスト側のコマンド

さて、QEMUを動かしているのとは別のターミナルで

```
arm-none-eabi-gdb
(gdb) target remote :1234
```

とやると、QEMU内のデバッグモニタと

```
(gdb) set $pc=0x10000
(gdb) info all-registers
(gdb) si
(gdb) disassemble 0x10000, 0x10020
(gdb) print /x $r0
(gdb) p/x $r0
```

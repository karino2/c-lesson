---
title: "第二回 簡易アセンブラとディスアセンブラを作ろう"
layout: page
---

第二回はQEMUを使ってARMのバイナリを動かしたり、バイナリを生成したりディスアセンブルしたりするコードを書きます。

----

以下メモ

```
sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa
sudo apt update
sudo apt install gcc-arm-embedded

sudo apt install qemu-system-arm
```

バイナリエディタ

今回はBz Editorを使った。

https://github.com/devil-tamachan/binaryeditorbz


```
arm-none-eabi-objdump -S hello_arm.o
```

versatilepbはARM926EJ-S。
https://developer.arm.com/docs/ddi0198/latest/preface

ただデータシートの4章くらいが今回程度の知識なら妥当な気がする。
https://developer.arm.com/docs/ddi0027/latest/arm7di-data-sheet
---
title: "Tensorflow入門ページ"
layout: page
---

既存のTensorflow入門が気に食わないので自分で動画の説明を作ろうと思い、そのための補助教材置き場です。

[内輪でやっている詳解ディープラーニングの勉強会](https://karino2.github.io/deeplearning-tensorflow-keras-study/)

で話した内容からブランチして手直ししていくつもりです。



# 事前準備

- githubアカウント作成
- GCPアカウン作成とチュートリアル
  - [GCE tutorial](http://cs231n.github.io/gce-tutorial/)を先頭から、 「Connect to Your Virtual Instance and Download the Assignment」のsshにつなげる所までやってくること（ダウンロードする所から先はやらなくてOK.ただJupyter Notebookとつなげる所はやっておくとついていくのは楽かも）
- 実装予定の節の周辺を読んでくる。実装予定は
   - 3.3.2
   - 3.4.3.1
   - 3.4.3.2
   - 3.5.3、3.6.3、3.7.3は、おそらく時間的にやれない気がするが、念の為軽く見ておくくらいはしていただけると幸い
- Python知らない人は後述の「Pythonの最低限の知識」くらいはやってあると望ましい（予習が間に合わなかったらサポートしますが）


## Pythonの最低限の知識

あまり専門的な知識を前提とする気はないので、関数とクラスが書けてリスト内包表記がわかるくらいの人なら、別段予習は必要ありませんが、苦手と思っている人は以下くらいをいやっておくと万全。

- [CS231nのPython Numpyチュートリアル](http://cs231n.github.io/python-numpy-tutorial/)
  - これ見るくらいで十分です。
- [Pythonチュートリアル](https://docs.python.jp/3/tutorial/index.html)
  - 上記のチュートリアルでは分からない、という人はこちらを先にやるのがオススメ。
  - CS231nのチュートリアルを見て、分からない所をこちらで調べる、という感じに使えば十分
  - CS231nの方がついていくのが辛くてこちらのPythonチュートリアルの方がやりやすい、という人は以下の感じで進める
    - 最低1から4まで
    - 出来たらさらに
      - 6
      - 9.1から9.3まで
      - 12くらいを読んでおくと万全
- [Python Cookbook](http://shop.oreilly.com/product/0636920027072.do)
  - こちらはさらに学びたい人用(事前準備というレベルでは不要です)
  - 辞書的に使う。目次に軽く目を通しておいて、調べたい事が出てきたら、調べるついでに付近を読む、という感じに使う
  - （この勉強会という枠を超えて）データ分析のPythonとしては、Pythonチュートリアルとcookbookだけで十分


# 基本コンセプト

- プログラミング初心者を想定して、自分が考えるベストのやり方を話す
  - 好きにして良い事も自分が良いと思う一つだけを話す
  - 複数選択肢がある時は自分の一番のオススメを一つだけ話す
  - 他の方法が好きな人はそれを使っても構いません
- 何故そうなのか、という事について、話すと長くなる事は説明しない
  - 初心者の人は、理由はおいといてとりあえず自分のオススメのやり方から始めてもらう
  - ある程度慣れてきたら違うやり方でやってもらって構わない
- ゼロから始められるように、全作業を自分で行う
- データ分析を個人でやる事を想定してそれ以外の話はなるべくしない
  - チーム開発は別に学んでください
  - ただし他の分析屋との協業くらいは意識した内容にはします
- その場では理解出来ないだろうけど必要な事も話しておく
  - 実際に作業をしていく過程やトラブルにあった時に「あぁ、こういう事か」と分かる素地を作る

# 話す事

- Docker
- Notebook
- gitとgithub

# 環境設定の基本

- 作業をスクリプトに残す必要性
   - Jupyter Notebook
   - Dockerfile
   - gist
   - 作業メモ
- 今回最終的に作る構成

## インスタンスに残さない

インスタンスはいつでも潰せるように。

- git
- S3やDropBox等
- gist

# GCEとgcloudコマンド

- ポートフォワード
- gist
- screen

## 最低限のコマンド


ポートフォワードは後述

# Linux入門

Unixは大雑把には、ファイルとプロセスを合わせた物と言える。

## プロセス

- CPU入門
   - 車のエンジンとの比較
   - プログラムカウンタとゲート
- init
- ローダー(ld.so)とLD_LIBRARY_PATH
- forkとexec
   - 環境変数

## ファイル

- rootfs
- mount pointとマウント

## ディストリジューションとパッケージ

- DebianとUbuntu、RedHatとCentOS
- aptとyum


# Docker入門

## Dockerとは何か？

- 軽い仮想環境（のようなもの）
- 環境の再現性
- 軽い
- 本来の用途
   - データ分析向けじゃない

## イメージとwritable

- union mountによるレイヤー構造
   - append onlyと削除
   - 共有
- 最後にwritableなレイヤー
- イメージとは何か？

## コンテナ

- namespaceとcgroupによる分離
- プロセスのサブツリー

## Dockerfileとdocker build

- レイヤーを作る為のスクリプト
- 書く時の作業手順
- 何をDockerfileに書くべきか
- ハッシュとレイヤー
- apt-getとpip
   - ubuntuとCentOSとLinux
- docker run -tiによるデバッグ

## dockerサーバーとdockerコマンド

- docker build
- docker run
   - オプションit、alloc pseudo tty and keep stdin open
- docker ps
- docker rmi
- docker images


## 分析時のベストプラクティス

- 何をイメージにし、何をmountするべきか
- 何をホストに置くべきか
   - wget、ack、git、screen
   - gistでのセットアップ



# Jupyter Notebook入門


## Hello Notebook

まずは触ってみる。

- ポートフォワードとは
   - IPアドレスとポート
- gcloudとポートフォワード、docker run。
- ホストでscreenを動かし、その中でdockerを動かし、その中でnotebookを動かす

## 利点

- 作業経過がスクリプトに残る
- セルによるゆるいモジュール化

## 簡単な解説

- カーネルとPythonプロセス
   - セルマジックとカーネルのリスタート
- セル
  - マークダウン
  - コード
- SIGHUPとscreen

## 良く使うscreenのショートカット等

- C-a d
- C-a n
- C-a c
- screen -r
- screen -ls

# データ分析の為のgit入門

gitはまず日常の事だけ（難しいから）

最初の設定はemailとアカウント（とエディタ）

## 分散ソース管理

分散で有る事の意義（はあまりデータ分析では関係ない）

- remoteとoriginとローカル

## ドキュメントとしてのソース管理

- 最後に残るのはレポジトリ
  - wikiやチケットは失われる
- READMEとcommit log


## よく使うコマンド

- git init
- git add
- git commit
- git push
- git clone
- git remote


# プログラミングの原則

- コードを見ながら書く
- まず動かし、頻繁に動かす
   - コンピュータからフィードバックをもらう重要性
   - Notebook
      - まずデータをちょっと見る
   - ユニットテスト
   - 動くコードから変える
- 関数の作り方
  - YAGNI
  - 3回出会ったら考える
  - インタラクティブに使いやすい事
  - ネストとフラットさ
     - 実行のしやすさ
  - ライフサイクルを意識した開発
  - 必要になったらやる事の大切さ
  - 長さの目安（10行以下くらい）
- グローバル変数と引数とクラス
  - グローバル変数のメリット
- 一回はフラットに実行する（なるべく）
  - コーディングのほとんどの時間はデバッグ
  - for文
  - 関数
- （なるべく）どの場所もその部分だけをShift-Enterで実行出来るように。

## コードを見ながら書く

- ゼロからイチにする所は難しい
- なるべく全行程の入ってるコード
- なるべく短いもの
- なるべく似ているもの（ただし勉強の為にはあえて違う物も）
- コードの書き溜めの重要性
  - Getting Startedや入門書の写経

## 推奨するフォルダ構成
- data
- Dockerfile
- ソース
- ipynb
- ライフサイクルを意識する事！


# Tensorflow入門

- CPUとGPUの違い
   - マニュアルとベルトコンベア
- 自動微分
- Computation Graphは設計図
- Pythonにおける足し算

## さらに勉強する為に
- [本家の Getting Started](https://www.tensorflow.org/get_started/get_started) 最初はこれ
- [CS 20SI](https://web.stanford.edu/class/cs20si/syllabus.html) 次はこれ。前半のスライドは入門に良い。

---




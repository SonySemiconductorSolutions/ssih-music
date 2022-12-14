# import-sfz.py

`import-sfz.py` は、SFZで記述された音源をSound Signal Processing Library for Spresense(ssprocLib)で扱えるSFZに変換します。

## 書式

```
python import-sfz.py [--out=<output_dir>] [<files...>]
```

## 説明

ssprocLibが提供しているSFZSinkモジュールには、いくつかの制約があります。
* 音源サンプルファイルの制約
    * ファイル形式はWAVファイルであること。
    * サンプリング周波数は48kHz、ビット幅は16bit、チャンネル数は2であること。
* 音源再生時の制約
    * ピッチシフト再生に対応していないため `pitch_keycenter` を扱えない。

したがって以下に該当するSFZ音源は、SFZSinkモジュールで正しく再生することができません。
* 音源サンプルファイルのファイル形式がOgg VorbisやFLACである。
* 音源サンプルファイルのサンプリング周波数が44.1kHzである。
* 音源サンプルファイルのビット幅が24bitである。
* 音源サンプルファイルのチャンネル数が1である。
* `pitch_keycenter` によるピッチシフト再生を期待している。

`import-sfz.py` はこれらの問題を解決するツールです。
* 音源サンプルファイルのファイルフォーマットを48kHz, 16bit, 2chのWAVファイルに変換する。
* 音源サンプルファイルにピッチシフト処理を行う。ファイル名は "{変換前ピッチ}{変換後ピッチ}_{元ファイル名}" とする。
* `lokey` と `hikey` に異なる値が設定されている `<region>` は、 `lokey`, `hikey`, `pitch_keycenter` が同値になる複数個の `<region>` に展開する。

## 変換例

変換前のSFZ:
```変換前.sfz
// ノート番号59の音
<region> sample=B3.flac lokey=59 hikey=59 pitch_keycenter=59

// ノート番号60と61の音
<region> sample=C4.flac lokey=60 hikey=61 pitch_keycenter=60
```

変換後のSFZ:
```変換後.sfz
// ノート番号59の音
<region> sample=059059_B3.wav lokey=59 hikey=59 pitch_keycenter=59

// ノート番号60と61の音
<region> sample=060060_C4.wav lokey=60 hikey=60 pitch_keycenter=60
<region> sample=060061_C4.wav lokey=61 hikey=61 pitch_keycenter=61
```

## 事前準備

`import-sfz.py` は以下のコマンドを使用します。インストールしてPATHが通った状態にしておいてください。
* `python`
* `ffmpeg`

## 使用例

例として VSCO 2 CE を変換してみましょう。
"output" フォルダの中に、SFZSinkで使用できるSFZ音源を出力させてみます。

1. 15GB程度のストレージ容量を確保する。
2. [VSCO 2 CE](https://vis.versilstudios.com/vsco-community.html)からSFZ音源ファイル "VSCO2-CE-1.1.0.zip" をダウンロードする (ダウンロードサイズ2GB以上)。
3. ZIPファイルを展開する (展開後サイズ3GB以上)。
```
unzip VSCO2-CE-1.1.0.zip
```
4. `import-sfz.py` で変換する (変換後サイズ7GB程度)。
```
python import-sfz.py --out=output VSCO2-CE-1.1.0/*.sfz
```

# Copyright

Copyright 2022 Sony Semiconductor Solutions Corporation

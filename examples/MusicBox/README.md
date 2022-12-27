# MusicBox

MIDIファイルを再生するサンプルです。

## 構成ハードウェア

* [Spresense メインボード](https://developer.sony.com/ja/develop/spresense/specifications)
* [Spresense 拡張ボード](https://developer.sony.com/ja/develop/spresense/specifications)
* microSDHCカード
    * 楽譜ファイル(MIDIファイルなど)
    * SFZ音源ファイル
* スピーカー

## 構成ソフトウェア

* Spresense Reference Board
* [Sound Signal Processing Library for Spresense](https://github.com/SonySemiconductorSolutions/ssih-music/)

## ソースコード (抜粋)

```MusicBox.ino
#include <ScoreSrc.h>
#include <SFZSink.h>

// SFZ音源ファイルを指定します。
SFZSink sink("SawLpf.sfz");

// 演奏する楽譜ファイルを指定します。
// フォルダを指定すると、そのフォルダの中にある楽譜ファイルを演奏します。
ScoreSrc inst("SCORE", true, sink);

void setup() {
    inst.begin();
}

void loop() {
    inst.update();
}
```

## 楽譜ファイルを用意する

サンプルスケッチMusicBoxはMIDIファイルの再生に対応しています。
再生したいMIDIファイルをSDカードにコピーして、そのファイル名をスケッチの `ScoreSrc` に指定します。

```MusicBox.ino
// 演奏する楽譜ファイルを指定します。
// フォルダを指定すると、そのフォルダの中にある楽譜ファイルを演奏します。
ScoreSrc inst("score.mid", true, sink);
```

MIDIファイルは販売・配布されているものを入手して使用することができます。
その場合は**利用規約、著作権、ライセンスをよく確認し順守して使用してください。**

MIDIファイル配布サイトの例:
* [ぷりんと楽譜](https://www.print-gakufu.com/)
* [Wikimedia Commons](https://commons.wikimedia.org/wiki/)
    * [Category:MIDI files](https://commons.wikimedia.org/wiki/Category:MIDI_files)
        * 多数のMIDIファイルが一覧されますが、すべてのMIDIファイルが表示されているわけではありません。例えば[Flohwalzer(ノミのワルツ、猫踏んじゃった)](https://commons.wikimedia.org/wiki/Category:Flohwalzer)のMIDIファイルはこのページには列挙されていません。
* [フリーBGM Music with myuu](https://www.ne.jp/asahi/music/myuu/)

## 音源ファイルを用意する

サンプルスケッチMusicBoxはSFZ音源ファイルを使って楽譜を再生します。
演奏したいSFZ音源ファイルをSDカードにコピーして、そのファイル名をスケッチの `SFZSink` に指定します。

```MusicBox.ino
// SFZ音源ファイルを指定します。
SFZSink sink("SawLpf.sfz");
```

SFZ音源ファイルは[サンプル音源](https://github.com/SonySemiconductorSolutions/ssih-music/releases/latest/download/assets.zip)(SawLpf.sfz)のほか、販売・配布されているものを入手して使用することができます。
販売・配布されているSFZ音源ファイルは、そのままでは本ライブラリで使用できない場合があるため、その場合はインポートツール[import-sfz.py](../../tools/README.md)で変換すると使用できるようになる場合があります。

このようなSFZ音源ファイルを使用する場合は**利用規約、著作権、ライセンスをよく確認し順守して使用してください。**
**インポートツールを使用した場合は音源ファイルの加工を伴うため、加工に関する条項にも注意してください。**
**また、1つのSFZ音源ファイルの中であっても、SFZファイルやWAVファイルに個別のライセンスが設定されている場合もあります。**

SFZ音源ファイル配布サイトの例:
* [Versilian Studios LLC.](https://vis.versilstudios.com/)
    * [VSCO Community](https://vis.versilstudios.com/vsco-community.html)
* [Virtual Playing Orchestra](http://virtualplaying.com/virtual-playing-orchestra/)
* [The FreePats project](https://freepats.zenvoid.org/)
* [SFZ Instruments](https://sfzinstruments.github.io/)

## 演奏方法

Spresenseの電源をオンにすると、指定した楽譜が指定した音源で再生されます。

## 高度な設定

一般的にMIDIファイル(スタンダードMIDIファイル)は、複数の演奏パートの演奏情報を含んでいます。

しかし、本ライブラリには同時に鳴らせる音の数(和音数)に限りがあるため、すべてのパートの演奏ができないことがあります。
その場合は `setParam` を使って、再生対象の演奏パートを絞り込むことで改善する場合があります。

### 再生するチャンネルを絞り込む

`ChannelFilter`モジュールを使うことで、再生対象の演奏パートをチャンネル番号を使って絞り込むことができます。
チャンネル番号は演奏パートを区別するために使われる1～16の数値です。
再生対象の演奏パートにどのチャンネル番号が割り当てられているかは、DAWソフトでMIDIファイルを開くと確認することができます。

チャンネル2を再生させたい場合は、以下のようにしてチャンネル2だけが再生されるようにフィルタリングします。

```MusicBox.ino
#include <ScoreSrc.h>
#include <ChannelFilter.h>                  // 追加
#include <SFZSink.h>

SFZSink sink("SawLpf.sfz");
ChannelFilter filter(sink);                 // 追加
ScoreSrc inst("score.mid", true, filter);   // 変更

void setup() {
    inst.begin();
    inst.setParam(ChannelFilter::PARAMID_CHANNEL_MASK, 0x00000000); // 再生対象チャンネルをクリアする
    inst.setParam(ChannelFilter::PARAMID_ENABLE_CHANNEL, 2);        // チャンネル2を再生対象に追加する
}

void loop() {
    inst.update();
}
```

### 再生するトラックを絞り込む

再生するMIDIファイルがフォーマット1のときに限り、再生対象のトラックを指定することで演奏パートを絞り込むことができます。
MIDIファイルがフォーマット1かどうか、再生対象の演奏パートがどの何番目のトラックに割り当てられているかは、DAWソフトでMIDIファイルを開くと確認することができます。

第3トラックを再生させたい場合は、以下のようにして第1トラックと第3トラックが再生されるようにフィルタリングします。
再生したいのは第3トラックだけですが、フォーマット1のMIDIファイルでは第1トラックにテンポ情報があるため、第1トラックも再生対象に含める必要があります。

```MusicBox.ino
#include <ScoreSrc.h>
#include <SFZSink.h>

SFZSink sink("SawLpf.sfz");
ScoreSrc inst("score.mid", true, sink);

void setup() {
    inst.setParam(ScoreFilter::PARAMID_TRACK_MASK, 0x00000000); // 再生対象トラックをクリアする
    inst.setParam(ScoreFilter::PARAMID_ENABLE_TRACK, 0);        // 第1トラック(コンダクタートラック)を再生対象に追加する
    inst.setParam(ScoreFilter::PARAMID_ENABLE_TRACK, 2);        // 第3トラック(再生したいトラック)を再生対象に追加する
    inst.begin();
}

void loop() {
    inst.update();
}
```

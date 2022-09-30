# Sound Signal Processing Library for Spresense (20220826版)

このライブラリは[Spresense](https://www.sony-semicon.co.jp/products/smart-sensing/spresense/)で簡単に楽器を開発するためのものです。
電子楽器の演奏データを定めるMIDI規格をベースとしたAPIと、Spresenseのオーディオ入力・オーディオ出力を簡単に扱える機能ブロックを提供します。

このライブラリを試すには音源データが必要です。
音源データは[こちら](https://github.com/SonySemiconductorSolutions/ssih-music/releases/latest/download/assets.zip)からダウンロードしてください。

## チュートリアルの内容

* 開発環境のセットアップ
* ハードウェアのセットアップ
* ソフトウェアのインストール
* サンプル楽器
* 電子楽器の予備知識
* 楽器のソフトウェア構成
* SDSink を使って音を出す
* Src モジュールを使って複雑な入力を扱う
* SFZSink を使って音源の再生方法を制御する

## 開発環境のセットアップ

ソフトウェアの開発にはArduino IDEで行います。
次の手順に従ってArduino IDE、Spresense Arduino board package、Sound Signal Processing Library for Spresenseをインストールします。

1. [Spresense Arduino スタートガイド](https://developer.sony.com/develop/spresense/docs/arduino_set_up_ja.html)を参考に開発環境を構築する
    * 1.2. Arduino IDE のインストール
    * 1.4. Spresense Arduino board package のインストール
        * `追加のボードマネージャのURL`には、下記2つのURLを設定してください。
            ```
            https://github.com/sonydevworld/spresense-arduino-compatible/releases/download/generic/package_spresense_index.json
            https://github.com/TomonobuHayakawa/spresense-arduino-compatible/releases/download/common/package_spresense_instrument_index.json
            ```
            ![追加のボードマネージャーのURLの画像](/docs/BoardManagerA.png)
        * ボードマネージャから、下記2つのパッケージをインストールしてください。
            * `Spresense Reference Board` バージョン2.2.1
            * `Spresense Instrument Board` バージョン2.2.1
                * ![ボードパッケージインストールの画像](/docs/BoardManagerB.png)
    * 2.2. Spresense ブートローダーのインストール
2. Spresense Instrument Packageがインストールされていることを確認する
    * Arduino IDEを開き、 `ツール` > `ボード` > `Spresense Instrument Boards` > `Spresense Instrument` を選択できることを確認します。
    * メニューに現れない場合は、開発環境のセットアップ手順を最初からやり直してください。
3. Sound Signal Processing Library for SpresenseをArduino IDEにインストールする
    1. [リリースページ](https://github.com/SonySemiconductorSolutions/ssih-music/releases/)から Source code (zip) をダウンロードする
    2. Arduino IDEを開き、 `スケッチ` > `ライブラリをインクルード` > `.ZIP形式ライブラリをインストール` をクリック
    3. ダイアログが開いたら、1でダウンロードした ssih-music.zip を選択して `開く` をクリック

## ハードウェアのセットアップ

本ライブラリは、Spresenseを使った楽器のサンプルとして、ボタンを使った楽器とマイクを使った楽器を用意しています。
サンプルを動かすために必要なハードウェアと、その接続手順を以下に示します。

* 使用するハードウェア
    * [Spresense メインボード](https://developer.sony.com/ja/develop/spresense/specifications)
    * [Spresense 拡張ボード](https://developer.sony.com/ja/develop/spresense/specifications)
    * [Mic&LCD KIT for SPRESENSE](https://github.com/autolab-fujiwaratakayoshi/MIC-LCD_kit_for_SPRESENSE)
    * microSDカード
        * FAT32でフォーマットしたmicroSDHCカードを使用します。初めて使う時には[SDメモリカードフォーマッター](https://www.sdcard.org/ja/downloads-2/formatter-2/)でフォーマットすることを推奨します。
    * マイク (ミニプラグ(3.5mm)で接続できるもの)
        * Mic&LCD KIT for SPRESENSEに付属しているものが使えます。
    * スピーカー (ミニプラグ(3.5mm)で接続できるもの)
        * Spresense 拡張ボードのヘッドホンジャックに接続してください。
    * micro-B USBケーブル
* ハードウェアの接続
    1. Spresense メインボードとSpresense 拡張ボードを接続する
    2. Spresense 拡張ボードに、Mic&LCD KIT for SPRESENSEのマイク基板とスイッチ4個付きLCD基板を接続する
    3. マイクをマイク基板の `MJA` 端子に接続する
    4. スピーカーをSpresense 拡張ボードの `Headphone Connector` に接続する
        * ![接続後の画像](/docs/UNION.png)

## ソフトウェアのインストール

ハードウェアが接続できたら、音源ファイルと楽器ソフトウェアを書き込みます。
次の手順に従って、マイクで拾った声と同じ高さの音を出す SimpleHorn を書き込んでみましょう。

1. 音源ファイルをmicroSDカードに書き込む
    1. microSDカードをPCに挿入する
    2. [リリースページ](https://github.com/SonySemiconductorSolutions/ssih-music/releases/)から[assets.zip](https://github.com/SonySemiconductorSolutions/ssih-music/releases/latest/download/assets.zip)をダウンロードして、展開したファイルをmicroSDカードにコピーする
    3. microSDカードをSpresense 拡張ボードのmicroSDHCスロットに挿入する
2. 共通スケッチを書き込む
    1. Arduino IDEを開き、 `ファイル` > `スケッチ例` > `Sound Signal Processing Library for Spresense` > `YuruHorn_SubCore1` を選択する
    2. `ツール` > `ボード` > `Spresense Instrument Board` > `Spresense Instrument` を選択する
    3. `ツール` > `Core` > `SubCore 1` を選択する
    4. `ツール` > `シリアルポート` から、Spresenseが接続されているCOMポートを選択する
    5. `スケッチ` > `マイコンボードに書き込む` を選択する
    6. 「ボードへの書き込みが完了しました」と表示されたら終了
        * 書き込みに失敗した場合は「Spresense ブートローダーのインストール」からやり直してください。
3. DSPバイナリをSDカードに書き込む
    1. Arduino IDEを開き、 `ファイル` > `スケッチ例` > `Audio` > `dsp_installer` > `src_installer` を選択する
    2. `ツール` > `ボード` > `Spresense Instrument Board` > `Spresense Instrument` を選択する
    3. `ツール` > `Core` > `MainCore` を選択する
    4. `ツール` > `シリアルポート` から、Spresenseが接続されているCOMポートを選択する
    5. `スケッチ` > `マイコンボードに書き込む` を選択する
    6. 「ボードへの書き込みが完了しました」と表示されるまで待つ
        * 書き込みに失敗した場合は「Spresense ブートローダーのインストール」からやり直してください。
    7. `ツール` > `シリアルモニタ` でシリアルモニタを起動
    8. 「Select where to install SRC?」と質問されるので、SDカードにインストールするために「1」と入力して `送信` する
    9. 「Finished」と表示されたら終了
4. サンプルスケッチを書き込む
    1. Arduino IDEを開き、 `ファイル` > `スケッチ例` > `Sound Signal Processing Library for Spresense` > `SimpleHorn` を選択する
    2. `ツール` > `ボード` > `Spresense Instrument Board` > `Spresense Instrument` を選択する
    3. `ツール` > `Core` > `MainCore` を選択する
    4. `ツール` > `シリアルポート` から、Spresenseが接続されているCOMポートを選択する
    5. `スケッチ` > `マイコンボードに書き込む` を選択する
    6. 「ボードへの書き込みが完了しました」と表示されたら終了
        * 書き込みに失敗した場合は「Spresense ブートローダーのインストール」からやり直してください。
5. 演奏する
    * マイクに向かって鼻歌を歌うと、声の高さに応じた楽器の音が奏でられます。

## サンプル楽器

本ライブラリでは SimpleHorn 以外にもいくつかのサンプルを用意しています。

* ボタンで演奏する楽器
    * ButtonDrum
        * [Mic&LCD KIT for SPRESENSE](https://github.com/autolab-fujiwaratakayoshi/MIC-LCD_kit_for_SPRESENSE)のスイッチ4個付きLCD基板の、ボタン4～6を使用する
            * ![LCD KITの基板画像](/docs/Sample_LCD.png)
        * ボタンを押すと、それに対応した音が出る
            * ボタン4: 音源1再生
            * ボタン5: 音源2再生
            * ボタン6: 音源切り替え
            * ボタン7: (割り当てなし)
    * OneKeySynth
        * [Mic&LCD KIT for SPRESENSE](https://github.com/autolab-fujiwaratakayoshi/MIC-LCD_kit_for_SPRESENSE)のスイッチ4個付きLCD基板の、ボタン4～5を使用する
        * ボタンを押すと、楽譜の音符を読み進めて音を出す
            * ボタン4: 音源再生
            * ボタン5: 音源再生
            * ボタン6: (割り当てなし)
            * ボタン7: (割り当てなし)
* マイクで演奏する楽器
    * SimpleHorn
        * [Mic&LCD KIT for SPRESENSE](https://github.com/autolab-fujiwaratakayoshi/MIC-LCD_kit_for_SPRESENSE)のマイク基板を利用する
        * マイクで拾った音高と同じ高さの音が出る
    * YuruHorn
        * [Mic&LCD KIT for SPRESENSE](https://github.com/autolab-fujiwaratakayoshi/MIC-LCD_kit_for_SPRESENSE)のマイク基板を利用する
        * SimpleHorn と同じくマイクで拾った音高と同じ高さの音が出る楽器で、[SFZ Format](http://sfzformat.com/legacy/)による音源再生制御ができる
            * 例: ループ再生による音源ファイルの長さを超える音の再生ができる

## 電子楽器の予備知識

デジタルシンセサイザーに代表される多くの電子楽器は、[MIDI規格](https://midi.org/)で演奏データをやり取りしています。
演奏データはMIDIメッセージと呼ばれ、代表的なMIDIメッセージに *ノート・オン* と *ノート・オフ* があります。

ノート・オンは「音符の音を鳴らす」、ノート・オフは「音符の音を止める」MIDIメッセージで、パラメータとしてノート番号とベロシティを持ちます。

ノート番号は音の種類を表す数値で0～127の値を取ります。音階のある楽器ではノート番号と音の高さの関係は規格として定められており、ピアノの真ん中のC(ド, 261.6Hz)を60として、0(27.5Hz)～127(12543.9Hz)までの音を扱うことができます。
音階のない楽器ではMIDI規格としての決まりごとはなく、音の種類(例えばバスドラム／スネアドラム／シンバル)を区別する数値として用いられます。

ベロシティは音の強さを表す値で、同じく0～127の値を取ります。

![音階とノート番号の関係](/docs/NoteNumber.png)

## 楽器のソフトウェア構成

本ライブラリで作る楽器は、ボタンやマイクなどのセンサー入力を受けて演奏データを送る Src(ソース; Source) と、演奏データを受けて実際に音を出す Sink(シンク) という2種類のモジュールから構成します。
Src と Sink の間には、必要に応じて演奏データを加工するFilterを任意の数だけ置くこともできます。

![楽器のソフトウェア構成](/docs/DataFlow-framework.png)

本ライブラリは、用途に応じていくつかの実装済みモジュールを提供しています。

* 実装済み Src モジュール:
    * YuruhornSrc: マイクに入力された音声を解析し、同じ高さの音の演奏データを発行する
    * ScoreSrc: SDカード上の楽譜ファイルを元に演奏データを発行する
* 実装済み Sink モジュール:
    * SDSink: ユーザーが設定した音源テーブルにしたがって、SDカード上の音源ファイルを再生する
    * SFZSink: SDカード上の音源定義ファイル(SFZファイル)にしたがって、音源ファイルを再生する
        * 再生条件や再生制御を指定することで、より高度な演奏をサポートします。詳細は後述の「SFZSink を使って音源の再生方法を制御する」を参照してください。
* 実装済み Filter モジュール:
    * OctaveShift: 音の高さをオクターブ単位で上下させる
    * OneKeySynthesizerFilter: SDカード上の楽譜ファイルで音の高さを上書きする

本ライブラリのすべてのモジュールはすべて下記の関数を備えています。
これらの関数で演奏データをやり取りします。

| 関数              | 機能                                                           |
| ---               | ---                                                            |
| `begin`関数       | 楽器を動かす準備をします。                                     |
| `update`関数      | 定期的に呼び出すことで、楽器を動かします。                     |
| `sendNoteOn`関数  | ノート・オンを発行します。指定したノート番号の音を再生します。 |
| `sendNoteOff`関数 | ノート・オフを発行します。音の再生を停止します。               |

これらのうち`sendNoteOn`関数と`sendNoteOff`関数には、下記のパラメータを指定します。

| 引数              | 説明           | 設定できる範囲 | 制約                                  |
| ---               | ---            | ---            | ---                                   |
| 第1引数: note     | ノート番号     | 0～127         |                                       |
| 第2引数: velocity | ベロシティ     | 0～127         | 未対応。1以上の値を設定してください。 |
| 第3引数: channel  | チャンネル番号 | 0～15          | 未対応。0を指定してください。         |

## SDSink を使って音を出す

サンプルスケッチ ButtonDrum を参考に、音の出し方を確認しましょう。

![ButtonDrum のモジュール構成](/docs/DataFlow-ButtonDrum.png)

1. SDSink に音源ファイルを登録する
    * サンプル
        ```ButtonDrum.ino
        // 1. ノート番号と音源ファイルの対応表を定義する
        const SDSink::Item table[12] = {
            {60, "SawLpf/60_C4.wav"},
            {61, "SawLpf/61_C#4.wav"},
            {62, "SawLpf/62_D4.wav"},
            {63, "SawLpf/63_D#4.wav"},
            {64, "SawLpf/64_E4.wav"},
            {65, "SawLpf/65_F4.wav"},
            {66, "SawLpf/66_F#4.wav"},
            {67, "SawLpf/67_G4.wav"},
            {68, "SawLpf/68_G#4.wav"},
            {69, "SawLpf/69_A4.wav"},
            {70, "SawLpf/70_A#4.wav"},
            {71, "SawLpf/71_B4.wav"}
        };
        
        // 2. SDSink に対応表を登録する
        SDSink inst(table, 12);
        ```
    * 音源ファイルはSDカード上に置きます
    * この例では、ノート番号60に SawLpf/60_C4.wav 、ノート番号62に SawLpf/62_D4.wav を対応させた、合計12要素の対応表を定義している
2. SDSink を使って音を出す
	* `setup`関数で SDSink の`begin`関数を実行する
    * `loop`関数で SDSink の`update`関数を実行する
    * ボタンが押されたら`sendNoteOn`関数を実行し、離されたら`sendNoteOff`関数を実行する
    * サンプル
        * `setup`関数
            ```ButtonDrum.ino
            void setup() {
                inst.begin();
            }
            ```
		* `loop`関数
            ```ButtonDrum.ino
            int selector = 0;
            int note = 60 + (selector * 2);

            void loop() {
                int button4_input = digitalRead(PIN_D04);
                if (button4_input != button4) {
                    if (button4_input == LOW) {
                        // ボタンが押されたら、鳴らす音を選んで、再生する
                        note = 60 + (selector * 2);
                        inst.sendNoteOn(note, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
                    } else {
                        // ボタンが離されたら、鳴らしている音を止める
                        inst.sendNoteOff(note, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
                    }
                    button4 = button4_input;
                }
                inst.update();
            }
            ```

## Src モジュールを使って複雑な入力を扱う

サンプルスケッチ SimpleHorn を参考に、マイクで拾った音高を使って演奏させてみましょう。

![SimpleHorn のモジュール構成](/docs/DataFlow-SimpleHorn.png)

ButtonDrum はボタン入力で演奏する楽器だったので簡単なスケッチを書くだけで楽器を作ることができました。
しかし、音声入力で演奏する楽器はどのように作ればいいのでしょうか。
一般的に音声を解析するには大きなプログラムを書く必要がありますが、本ライブラリでは音高を演奏データに変換するモジュール YuruhornSrc を提供しているので、これを使ってみましょう。

1. YuruhornSrc と SDSink を接続する
    * サンプル
        ```SimpleHorn.ino
        // 1. ノート番号と音源ファイルの対応表を定義する
        const SDSink::Item table[25] = {
            {48, "SawLpf/48_C3.wav"}, {49, "SawLpf/49_C#3.wav"},
            {50, "SawLpf/50_D3.wav"}, {51, "SawLpf/51_D#3.wav"},
            {52, "SawLpf/52_E3.wav"},
            {53, "SawLpf/53_F3.wav"}, {54, "SawLpf/54_F#3.wav"},
            {55, "SawLpf/55_G3.wav"}, {56, "SawLpf/56_G#3.wav"},
            {57, "SawLpf/57_A3.wav"}, {58, "SawLpf/58_A#3.wav"},
            {59, "SawLpf/59_B3.wav"},
            {60, "SawLpf/60_C4.wav"}, {61, "SawLpf/61_C#4.wav"},
            {62, "SawLpf/62_D4.wav"}, {63, "SawLpf/63_D#4.wav"},
            {64, "SawLpf/64_E4.wav"},
            {65, "SawLpf/65_F4.wav"}, {66, "SawLpf/66_F#4.wav"},
            {67, "SawLpf/67_G4.wav"}, {68, "SawLpf/68_G#4.wav"},
            {69, "SawLpf/69_A4.wav"}, {70, "SawLpf/70_A#4.wav"},
            {71, "SawLpf/71_B4.wav"},
            {72, "SawLpf/72_C5.wav"}
        };

        // 2. SDSink に対応表を登録する
        SDSink sink(table, 25);

        // 3. YuruhornSrc の次のモジュールとして SDSink を登録する
        YuruhornSrc inst(sink);
        ```
2. YuruhornSrc を動かす
	* `setup`関数で YuruhornSrc の`begin`関数を実行する
    * `loop`関数で YuruhornSrc の`update`関数を実行する
    * サンプル
        * `setup`関数
            ```SimpleHorn.ino
            void setup() {
                inst.begin();
            }
            ```
		* `loop`関数
            ```SimpleHorn.ino
            void loop() {
                inst.update();
            }
            ```

これだけのコードで、入力音声を YuruhornSrc が演奏データに変換し、その演奏データを SDSink が音を奏でます。

SDSink を使った楽器は音源ファイルをそのまま再生するので、音源ファイルに無音部分があると楽器の音が途切れてしまいます。
これを防ぐためには、SFZファイルと SFZSink を使ってループポイントを設定することで解決できます。
SFZSinkについては「SFZSink を使って音源の再生方法を制御する」を参照してください。

## SFZSink を使って音源の再生方法を制御する

ButtonDrum と SimpleHorn は、スケッチの中に音源ファイルの情報を書いていました。
YuruHorn は SDSink の代わりにSFZSinkを使って音源を再生しています。
SFZSink はSDカード上に置かれたSFZファイルからノート番号と音源ファイルの対応表を読み取るため、SDSink のように対応表をスケッチに定義する必要がありません。

![YuruHorn のモジュール構成](/docs/DataFlow-YuruHorn.png)

SFZファイルは音源ファイルに対する情報を付加することができるため、音源ファイルの再生方法を細かく設定することができます。
一例として、有効範囲の設定、ループ再生方法の指定、ループポイントの設定といったことができます。
ここでは、SFZファイルの簡単な書き方を確認しましょう。

1. 音源ファイルとノート番号の対応を記述する
	* サンプル
    	```SawLpf.sfz
        <region> key=48 sample=SawLpf/48_C3.wav
        <region> key=49 sample=SawLpf/49_C#3.wav
        <region> key=50 sample=SawLpf/50_D3.wav
        ```
        | キーワード                         | 分類     | 説明                                             |
        | ---                                | ---      | ---                                              |
        | `<region>`                         | ヘッダー | 1つの音を定義します。対応表の1要素にあたります。 |
        | `key`={ノート番号、またはノート名} | Opcode   | 音に対応するノート番号を指定します。             |
        | `sample`={ファイル名}              | Opcode   | 音に対応する音源ファイル名を指定します。         |
    * この例では、ノート番号48に SawLpf/48_C3.wav 、ノート番号50に SawLpf/50_D3.wav が対応しています。
2. 音源ファイルの再生方法を指定する
    * サンプル
        ```SawLpf.sfz
        <region> key=48 sample=SawLpf/48_C3.wav  loop_mode=loop_continuous loop_start=48000 loop_end=143994
        <region> key=49 sample=SawLpf/49_C#3.wav loop_mode=loop_continuous loop_start=48127 loop_end=144036
        <region> key=50 sample=SawLpf/50_D3.wav  loop_mode=loop_continuous loop_start=47938 loop_end=144137
        ```
        | キーワード                        | 分類     | 説明                                                                       |
        | ---                               | ---      | ---                                                                        |
        | `offset`={オーディオサンプル}     | Opcode   | 有効範囲の始点を指定します。                                               |
        | `end`={オーディオサンプル}        | Opcode   | 有効範囲の終点を指定します。                                               |
        | `loop_start`={オーディオサンプル} | Opcode   | ループポイントの始点を指定します。                                         |
        | `loop_end`={オーディオサンプル}   | Opcode   | ループポイントの終点を指定します。                                         |
        | `loop_mode=no_loop`               | Opcode   | ループ再生を無効にします。`loop_mode`の初期値です。                        |
        | `loop_mode=one_shot`              | Opcode   | ワンショット再生します。再生中は再生指示・停止指示を受け付けません。       |
        | `loop_mode=loop_continuous`       | Opcode   | ループ再生を有効にします。`loop_start`, `loop_end`の間をループ再生します。 |
    * `offset`, `end`を指定しない場合、有効範囲は音源ファイル全体となります。
    * `loop_start`, `loop_end` を指定しない場合、ループポイントは音源ファイル全体となります。
    * `end`, `loop_end`に指定したオーディオサンプルは、再生対象に含まれます。例えば`offset=100 end=100`と指定した場合、1オーディオサンプルが再生されます。
    * 上記のほかのOpcode(パラメータ)にも一部対応しています。SFZについて詳しく知りたい方は [SFZ Format](https://sfzformat.com/) を参照してください。
3. SFZSink で音を出す
    * サンプル
        ```YuruHorn.ino
        SFZSink sink("yuruhorn.sfz");   // SDSink の対応表の代わりに SFZSink にはファイル名を指定する
        OctaveShift filter(sink);       // OctaveShift の次のモジュールとして SFZSink を登録する
        YuruhornSrc inst(filter);       // YuruhornSrc の次のモジュールとして OctaveShift を登録する

        void setup() {
            inst.begin();
        }

        void loop() {
            inst.update();
        }
        ```

これで、音源ファイルをより適切に再生できる楽器になりました。

## License

本ソフトウェアは Apache License 2.0 または GNU Lesser General Public License v2.1 or later の下に提供します。
詳しくは LICENSE を参照してください。

## Copyright

Copyright 2022 Sony Semiconductor Solutions Corporation

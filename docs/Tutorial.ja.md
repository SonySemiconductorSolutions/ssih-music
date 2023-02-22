# Sound Signal Processing Library for Spresense (2022-11-25版)

Sound Signal Processing Library for Spresense(ssprocLibと呼びます)は、[Spresense](https://www.sony-semicon.co.jp/products/smart-sensing/spresense/)で簡単に楽器を開発するためのライブラリです。
電子楽器の演奏データを定めるMIDI規格をベースとしたAPIと、Spresenseのオーディオ入力・オーディオ出力を簡単に扱えるソフトウェアモジュールを提供します。

ssprocLibを試すには音源データが必要です。
音源データは[こちら](https://github.com/SonySemiconductorSolutions/ssih-music/releases/latest/download/assets.zip)からダウンロードしてください。

## チュートリアルの内容

* 開発環境のセットアップ
* ハードウェアのセットアップ
* 楽器ソフトウェアのインストール
* サンプル楽器ソフトウェア
* 電子楽器の予備知識
* 楽器のソフトウェアモジュール
* SDSink を使って音を出す
* Src モジュールを使って複雑な入力を扱う
* SFZSink を使って音源の再生方法を制御する

## 開発環境のセットアップ

楽器ソフトウェアはArduino IDEで開発します。
次の手順に従ってArduino IDE、Spresense Arduino board package、Sound Signal Processing Library for Spresenseをインストールします。

1. [Spresense Arduino スタートガイド](https://developer.sony.com/develop/spresense/docs/arduino_set_up_ja.html)を参考にして、Arduino IDEとSpresense Arduino board packageをインストールする。
2. Sound Signal Processing Library for SpresenseをArduino IDEにインストールする。
    1. [リリースページ](https://github.com/SonySemiconductorSolutions/ssih-music/releases/)から Source code (zip) をダウンロードする。
    2. Arduino IDEを開き、 "スケッチ" > "ライブラリをインクルード" > ".ZIP形式ライブラリをインストール" をクリックする。
    3. ダイアログが開いたら、上の手順でダウンロードした ssih-music.zip を選択して "開く" をクリックする。

## ハードウェアのセットアップ

ssprocLibはSpresenseを使った楽器ソフトウェアのサンプルとして、ボタンで演奏する楽器とマイクで演奏する楽器を提供します。
サンプルを実行するために必要なハードウェアと、その接続手順を以下に示します。

* 使用するハードウェア
    * [Spresense メインボード](https://developer.sony.com/ja/develop/spresense/specifications)
    * [Spresense 拡張ボード](https://developer.sony.com/ja/develop/spresense/specifications)
    * [Mic&LCD KIT for SPRESENSE](https://github.com/autolab-fujiwaratakayoshi/MIC-LCD_kit_for_SPRESENSE)
        * マイク 4本
        * マイク基板
        * スイッチ4個付きLCD基板
    * microSDカード
        * FAT32でフォーマットしたmicroSDHCカードを使用します。初めて使う時には[SDメモリカードフォーマッター](https://www.sdcard.org/ja/downloads-2/formatter-2/)でフォーマットすることを推奨します。
    * マイク (ミニプラグ(3.5mm)で接続する)
        * マイクはMic&LCD KIT for SPRESENSEに付属しています。
    * スピーカー (ミニプラグ(3.5mm)で接続する)
        * Spresense 拡張ボードのHeadphone Connectorに接続します。
    * USBケーブル (micro-B)
* ハードウェアの接続
    1. Spresense メインボードとSpresense 拡張ボードを接続する。
    2. Spresense 拡張ボードに、Mic&LCD KIT for SPRESENSEのマイク基板とスイッチ4個付きLCD基板を接続する。
    3. マイクをマイク基板のMJA端子に接続する。
    4. スピーカーをSpresense 拡張ボードのHeadphone Connectorに接続する。
        * ![接続後の画像](/docs/UNION.png)

## 楽器ソフトウェアのインストール

ハードウェアを接続したら、音源ファイルと楽器ソフトウェアを書き込みます。
次の手順に従って、声と同じ高さの音を出す SimpleHorn を書き込んでみましょう。

1. 音源ファイルをmicroSDカードに書き込む。
    1. microSDカードをPCに挿入する。
    2. [リリースページ](https://github.com/SonySemiconductorSolutions/ssih-music/releases/)から[assets.zip](https://github.com/SonySemiconductorSolutions/ssih-music/releases/latest/download/assets.zip)をダウンロードして、展開したファイルをmicroSDカードにコピーする。
    3. microSDカードをSpresense 拡張ボードのmicroSDHCスロットに挿入する。
2. 共通スケッチを書き込む。
    1. Arduino IDEを開き、 "ファイル" > "スケッチ例" > "Sound Signal Processing Library for Spresense" > "YuruHorn_SubCore1" をクリックする。
    2. "ツール" > "ボード" > "Spresense Board" > "Spresense" をクリックする。
    3. "ツール" > "Core" > "SubCore 1" をクリックする。
    4. "ツール" > "シリアルポート" の中から、Spresenseが接続されているCOMポートをクリックする。
    5. "スケッチ" > "マイコンボードに書き込む" をクリックする。
    6. 「ボードへの書き込みが完了しました」と表示されるまで待つ。
        * 書き込みに失敗した場合は、[Spresense Arduino スタートガイド](https://developer.sony.com/develop/spresense/docs/arduino_set_up_ja.html)の「Spresense ブートローダーのインストール」からやり直してください。
3. DSPバイナリをSDカードに書き込む。
    1. Arduino IDEを開き、 "ファイル" > "スケッチ例" > "Audio" > "dsp_installer" > "src_installer" をクリックする。
    2. "ツール" > "ボード" > "Spresense Board" > "Spresense" をクリックする。
    3. "ツール" > "Core" > "MainCore" を選択する。
    4. "ツール" > "シリアルポート" の中から、Spresenseが接続されているCOMポートをクリックする。
    5. "スケッチ" > "マイコンボードに書き込む" をクリックする。
    6. 「ボードへの書き込みが完了しました」と表示されるまで待つ。
        * 書き込みに失敗した場合は、[Spresense Arduino スタートガイド](https://developer.sony.com/develop/spresense/docs/arduino_set_up_ja.html)の「Spresense ブートローダーのインストール」からやり直してください。
    7. "ツール" > "シリアルモニタ" をクリックして、シリアルモニタを起動する。
    8. 「Select where to install SRC?」と質問されるので、SDカードにインストールするために「1」と入力して "送信" をクリックする。
    9. 「Finished」と表示されたら終了です。
4. 楽器ソフトウェアを書き込む。
    1. Arduino IDEを開き、 "ファイル" > "スケッチ例" > "Sound Signal Processing Library for Spresense" > "SimpleHorn" をクリックする。
    2. "ツール" > "ボード" > "Spresense Board" > "Spresense" をクリックする。
    3. "ツール" > "Core" > "MainCore" をクリックする。
    4. "ツール" > "シリアルポート" の中から、Spresenseが接続されているCOMポートをクリックする。
    5. "スケッチ" > "マイコンボードに書き込む" をクリックする。
    6. 「ボードへの書き込みが完了しました」と表示されるまで待つ。
        * 書き込みに失敗した場合は、[Spresense Arduino スタートガイド](https://developer.sony.com/develop/spresense/docs/arduino_set_up_ja.html)の「Spresense ブートローダーのインストール」からやり直してください。
5. SimpleHorn を演奏する。
    * マイクに向かって鼻歌を歌うと、声の高さに応じた楽器の音が奏でられます。

## サンプル楽器ソフトウェア

ssprocLibはいくつかの楽器ソフトウェアのサンプルを用意しています。

* ボタンで演奏する楽器
    * [ButtonDrum](/examples/ButtonDrum/README.md)
        * ボタンを押すと対応する音が出る楽器です。LCD基板のボタン4～6を使用します。
            * ![LCD KITの基板画像](/docs/Sample_LCD.png)
            * ボタンの動作
                * ボタン4: 音源1を再生する
                * ボタン5: 音源2を再生する
                * ボタン6: 音源を切り替える
                * ボタン7: 使用しません
    * [OneKeySynth](/examples/OneKeySynth/README.md)
        * ボタンを押すと楽譜ファイルの通りに演奏する楽器です。LCD基板のボタン4～5を使用します。
            * ボタンの動作
                * ボタン4: 楽譜ファイルに従って音源を再生する
                * ボタン5: ボタン4と同じ
                * ボタン6: 使用しません
                * ボタン7: 使用しません
* マイクで演奏する楽器
    * [SimpleHorn](/examples/SimpleHorn/README.md)
        * 演奏者の鼻歌と同じ高さの音を出す楽器です。マイクとマイク基板を利用します。
    * [YuruHorn](/examples/YuruHorn/README.md)
        * 演奏者の鼻歌と同じ高さの音を出す楽器です。マイクとマイク基板を利用します。
        * [SFZ Format](http://sfzformat.com/legacy/)による音源ファイルの再生制御をすることできます。
        * 例えば、ループ再生をすることによって、音源ファイルよりも長い時間の再生ができます。

## 電子楽器の予備知識

電子楽器は[MIDI規格](https://midi.org/)で定められた演奏データをやり取りしています。
演奏データはMIDIメッセージと呼ばれます。

代表的なMIDIメッセージに *ノート・オン* と *ノート・オフ* があります。
ノート・オンは「音符の音を鳴らす」MIDIメッセージで、ノート・オフは「音符の音を止める」MIDIメッセージです。
それぞれのMIDIメッセージは、ノート番号とベロシティというパラメータを持ちます。

ノート番号は0～127の数値で、音の種類を表します。
音階のある楽器において、ノート番号は音の高さを表します。
一番低い音を0(27.5Hz)、一番高い音を127(12543.9Hz)で表します。
例えば、ピアノの真ん中のC(261.6Hz)のノート番号は60です。
音階のない楽器において、ノート番号は音の種類(バスドラム、スネアドラム、シンバルなど)を表します。

ベロシティは音の強さを表す値で0～127の値を取ります。

![音階とノート番号の関係](/docs/NoteNumber.png)

## 楽器のソフトウェアモジュール

ssprocLibにおける楽器は、ボタンやマイクなどのセンサー入力を受けて演奏データを送信する Source(Srcと略します) と、演奏データを受信して音を出す Sink という2種類のモジュールから構成されます。
Src と Sink の間には、必要に応じて演奏データを加工する Filter を任意の数だけ置くこともできます。

![楽器のソフトウェアモジュール](/docs/DataFlow-framework.png)

ssprocLibはいくつかの実装済みモジュールを提供しています。

* 実装済み Src モジュール:
    * `YuruhornSrc`: マイクに入力された音声を解析して、同じ高さの音を出す演奏データを送信する。
    * `ScoreSrc`: SDカード上の楽譜ファイルを元に演奏データを送信する。
* 実装済み Sink モジュール:
    * `SDSink`: ユーザーが設定した音源テーブルにしたがって、音源ファイルを再生する。
    * `SFZSink`: 音源定義ファイル(SFZファイル)にしたがって、音源ファイルを再生する。
        * 再生条件や再生制御を指定することで、より高度な演奏をサポートします。詳細は後述の「SFZSink を使って音源の再生方法を制御する」を参照してください。
* 実装済み Filter モジュール:
    * `OctaveShift`: 演奏データのノート番号を、オクターブ単位で上下させる。
    * `OneKeySynthesizerFilter`: 演奏データのノート番号を、楽譜ファイルのノート番号に置き換える。

Src モジュールと Sink モジュールは下記の関数を備えています。
これらの関数で演奏データをやり取りしています。

| 関数               | 機能                                                                                        |
| ---                | ---                                                                                         |
| `begin` 関数       | この関数は楽器を演奏する準備をします。Arduinoの `setup` 関数から実行してください。            |
| `update` 関数      | この関数を定期的に呼び出すことで楽器を演奏します。Ardiunoの `loop` 関数から実行してください。 |
| `sendNoteOn` 関数  | 音の再生を開始するために、ノート・オンを送信します。                                        |
| `sendNoteOff` 関数 | 音の再生を停止するために、ノート・オフを送信します。                                        |

`sendNoteOn`関数と`sendNoteOff`関数には下記の引数を指定します。

| 引数       | 説明           | 設定できる範囲 |
| ---        | ---            | ---            |
| `note`     | ノート番号     | 0～127         |
| `velocity` | ベロシティ     | 0～127         |
| `channel`  | チャンネル番号 | 1～16          |

## SDSink を使って音を出す

サンプルスケッチ ButtonDrum を参考に、音の出し方を確認しましょう。

![ButtonDrum のモジュール構成](/docs/DataFlow-ButtonDrum.png)

1. `SDSink` に音源ファイルを登録する。
    * SDカード上に音源ファイルを置く。
    * ノート番号と音源ファイルの対応表を作り、SDSinkに登録する。
    * サンプルスケッチ
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
    * この例ではノート番号60に "SawLpf/60_C4.wav" 、ノート番号62に "SawLpf/62_D4.wav" を対応させた、合計12要素の対応表を定義しています。
2. サンプル楽器 ButtonDrum を実行する。
    * `setup` 関数で `begin` 関数を実行する。
    * `loop` 関数で `update` 関数を実行する。
    * ボタンが押されたら `sendNoteOn` 関数を実行し、離されたら `sendNoteOff` 関数を実行する処理を `loop` 関数に書く。
    * サンプルスケッチ
        ```ButtonDrum.ino
        void setup() {
            inst.begin();
        }
        
        Button button4(PIN_D04);
        Button button5(PIN_D05);
        Button button6(PIN_D06);
        
        int note1 = INVALID_NOTE_NUMBER;
        int note2 = INVALID_NOTE_NUMBER;
        
        int selector = 0;
        
        void loop() {
            // ボタン4
            if (button4.hasChanged()) {
                if (button4.isPressed()) {
                    note1 = 60 + (selector * 2);
                    inst.sendNoteOn(note1, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
                } else {
                    inst.sendNoteOff(note1, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
                }
            }
        
            // ボタン5 (省略)
        
            // ボタン6 (省略)
        
            inst.update();
        }
        ```

## Src モジュールを使って複雑な入力を扱う

サンプルスケッチ SimpleHorn を参考に、 Src モジュールの使い方を確認しましょう。

![SimpleHorn のモジュール構成](/docs/DataFlow-SimpleHorn.png)

ButtonDrum はボタン入力で演奏する楽器だったので、簡単なスケッチを書くだけで楽器を作ることができました。
しかし、音声入力で演奏する楽器はどのように作ればいいのでしょうか。
ssprocLibは音高を演奏データに変換するモジュール `YuruhornSrc` を提供しているので、これを使ってみましょう。

1. `YuruhornSrc` と `SDSink` を接続する。
    * サンプルスケッチ
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
2. サンプル楽器 SimpleHorn を実行する。
    * `setup` 関数で `YuruhornSrc` の `begin` 関数を実行する。
    * `loop` 関数で `YuruhornSrc` の `update` 関数を実行する。
    * サンプルスケッチ
        ```SimpleHorn.ino
        void setup() {
            inst.begin();
        }
        
        void loop() {
            inst.update();
        }
        ```

`YuruhornSrc` が入力音声を演奏データに変換し、その演奏データを元に `SDSink` が音を奏でます。
一般的に音声を解析するには大きなプログラムを書く必要がありますが、ssprocLibではこれだけのプログラムで音声を扱うことができます。

`SDSink` を使った楽器は音源ファイルをそのまま再生するので、音源ファイルに無音部分があると楽器の音が途切れてしまいます。
これを防ぐためには、SFZファイルと `SFZSink` を使ってループポイントを設定することで解決できます。
`SFZSink` については「SFZSink を使って音源の再生方法を制御する」を参照してください。

## SFZSink を使って音源の再生方法を制御する

SFZファイルを扱うサンプルスケッチ YuruHorn を見てみましょう。
YuruHorn は SimpleHorn と同様に音声入力で演奏する楽器ですが、 `SDSink` ではなく `SFZSink` を使います。

![YuruHorn のモジュール構成](/docs/DataFlow-YuruHorn.png)

`SFZSink` はSFZファイルにしたがって音源を再生する Sink モジュールです。
SFZファイルはノート番号と音源ファイルの対応関係と、音源ファイルの付加情報(有効範囲や再生方法など)を記述したテキストファイルです。
SFZファイルの簡単な書き方を確認しましょう。

1. ノート番号と音源ファイルの対応を記述する。
    * 対応表を `<region>`, `key`, `sample` のキーワードを使って記述する。
    * サンプル (抜粋)
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
    * この例ではノート番号48に SawLpf/48_C3.wav 、ノート番号50に SawLpf/50_D3.wav が対応しています。
2. 音源ファイルの再生方法を記述する。
    * 音源ファイルをループ再生させるために `loop_mode=loop_continuous`, `loop_start`, `loop_end` キーワードを指定する。
    * サンプル (抜粋)
        ```SawLpf.sfz
        <region> key=48 sample=SawLpf/48_C3.wav  loop_mode=loop_continuous loop_start=48000 loop_end=143994
        <region> key=49 sample=SawLpf/49_C#3.wav loop_mode=loop_continuous loop_start=48127 loop_end=144036
        <region> key=50 sample=SawLpf/50_D3.wav  loop_mode=loop_continuous loop_start=47938 loop_end=144137
        ```
        | キーワード                        | 分類     | 説明                                                                                |
        | ---                               | ---      | ---                                                                                 |
        | `offset`={オーディオサンプル}     | Opcode   | 音源ファイルの有効範囲の始点を指定します。                                          |
        | `end`={オーディオサンプル}        | Opcode   | 音源ファイルの有効範囲の終点を指定します。                                          |
        | `loop_start`={オーディオサンプル} | Opcode   | ループポイントの始点を指定します。                                                  |
        | `loop_end`={オーディオサンプル}   | Opcode   | ループポイントの終点を指定します。                                                  |
        | `loop_mode=no_loop`               | Opcode   | ループ再生をしません。これは `loop_mode` の初期値です。                             |
        | `loop_mode=one_shot`              | Opcode   | ワンショット再生します。再生中は再生指示・停止指示を受け付けません。                |
        | `loop_mode=loop_continuous`       | Opcode   | ループ再生を有効にします。`loop_start` から `loop_end` までが繰り返し再生されます。 |
    * `offset` と `end` を指定しない場合、有効範囲は音源ファイル全体となります。
    * `loop_start` と `loop_end` を指定しない場合、ループポイントは音源ファイル全体となります。
    * `end` と `loop_end` に指定したオーディオサンプルは再生対象に含まれます。例えば `offset=100 end=100` と指定した場合は、1オーディオサンプルが再生されます。
    * 上記のほかのOpcode(パラメータ)にも一部対応しています。SFZについて詳しく知りたい方は [SFZ Format](https://sfzformat.com/) を参照してください。
3. サンプル楽器 YuruHorn を実行する。
    * サンプルスケッチ
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

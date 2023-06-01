# Sound Signal Processing Library for Spresense

## Overview

The Sound Signal Processing Library for Spresense (called ssprocLib) is a library for easy instrument development in [Spresense](https://developer.sony.com/develop/spresense/).

See the [tutorial](/docs/Tutorial.en.md) for how to use this library.

## Version info

| バージョン | リリース日  | 主な新機能と変更点                                               |
| ---        | ---         | ---                                                              |
| v0.6.0     | 2022-07-20  | 初版                                                             |
| v0.7.0     | 2022-08-26  | (機能追加) スケッチでメモリプールの初期化が不要になりました。    |
|            |             | (不具合修正) https://github.com/SonySemiconductorSolutions/ssih-music/issues/3: 音声データの供給がされないと異常停止する問題を修正しました。    |
|            |             | (不具合修正) https://github.com/SonySemiconductorSolutions/ssih-music/issues/4: 一部のSFZファイルのロードができない問題を修正しました。         |
|            |             | (サンプル) サンプルスケッチにYuruFlaCastanetを追加しました。     |
| v0.8.0     | 2022-09-30  | (機能追加) MIDIファイルの一時停止に対応しました。                |
|            |             | (サンプル) サンプルスケッチにYuruFlaShoeを追加しました。         |
| v0.9.0     | 2022-10-28  | (機能追加) 4和音出力に対応しました。                             |
|            |             | (機能追加) プレイリストファイル(M3U)の再生に対応しました。       |
|            |             | (機能追加) Spresense Reference Board開発環境に対応しました。     |
|            |             | (機能追加) SFZ Formatのdefault_pathパラメータに対応しました。    |
|            |             | (不具合修正) MIDIファイル再生で、音符や休符の再生時間が短くなる問題を修正しました。                                                             |
| v0.10.0    | 2022-11-25  | (不具合修正) MIDIファイルを再生したときに、パート間で音ずれを起こす問題を修正しました。                                                         |
|            |             | (サンプル) サンプルスケッチYuruFlaCastanetを新しい加速度センサBMI270に対応しました。                                                            |
|            |             | (ツール) SFZ音源を本ライブラリで使用できるように変換する import-sfz.py を追加しました。                                                         |
|            |             | (ドキュメント) 英語版チュートリアルを追加しました。                                                                                             |
| v1.0.0     | 2022-12-23  | (機能追加) MIDIファイルの全トラックの再生に対応しました。最大で32トラックまでの再生に対応しますが、同時に出せる音は4つまでです。                |
|            |             | (互換性向上) チャンネル値の値範囲を1～16にしました。                                                                                            |
| v1.1.0     | 2023-02-22  | (機能追加) MIDIファイル再生で、MIDIメッセージによる再生制御 (曲選択、頭出し、再生、停止)に対応しました。                                        |
|            |             | (機能追加) 演奏時のノイズを低減しました。                                                                                                       |
|            |             | (サンプル) サンプルスケッチに使用手順のドキュメントを追加しました。                                                                             |
| v1.2.0     | 2023-03-31  | (機能追加) MIDIのProgram ChangeメッセージとControl Changeメッセージに対応しました。                                                             |
|            |             | (ドキュメント) API仕様のコメント(日本語)を追加しました。                                                                                        |
|            |             | (ドキュメント) サンプルスケッチでスタンダードMIDIファイルを使う場合の注意事項を記載しました。examples/MusicBox/README.mdを参照してください。    |
| v1.3.0     | 2023-05-26  | (ツール) 楽譜ファイルを扱うモジュールのユニットテストを追加しました。これに伴ってテスト用スタブの機能拡張と不具合修正を行いました。             |
|            |             | (その他) Spresense Reference Board バージョン 3.0.0 での動作確認を行いました。                                                                  |

## ライブラリ概要

このライブラリは[Spresense](https://www.sony-semicon.co.jp/products/smart-sensing/spresense/)で簡単に楽器を開発するためのライブラリです。
電子楽器の演奏データを定めるMIDI規格をベースとしたAPIと、Spresenseのオーディオ入力・オーディオ出力を簡単に扱えるソフトウェアモジュールを提供します。

## チュートリアル

使い方は[チュートリアル](/docs/Tutorial.ja.md)を参照してください。

## ライセンス

本ソフトウェアは Apache License 2.0 または GNU Lesser General Public License v2.1 or later の下に提供します。
詳しくは [LICENSE](/LICENSE) を参照してください。

## Copyright

Copyright 2022 Sony Semiconductor Solutions Corporation

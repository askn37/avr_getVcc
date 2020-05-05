# avr_getVcc

AVR用 VCC電圧計測と 内蔵温度感知器 の計測

## Arduino IDE への導入

1. .ZIPアーカイブをダウンロードする。
[Click here](https://github.com/askn37/avr_getVcc/archive/master.zip)

2. ライブラリマネージャで読み込む

    スケッチ -> ライブラリをインクルード -> .ZIP形式のライブラリをインストール...

## 使い方

```c
#include <Arduino.h>
#include "avr_getVcc.h"

void setup (void) {
  while (!Serial);
  Serial.begin(9600);
}

void loop (void) {
  float vcc = getVcc();
  float tmp = getCoreTemperature();

  Serial.print(vcc);
  Serial.write(',');
  Serial.print(tmp - 273.15);
  Serial.println();
  delay(1000);
}
```

## リファレンス

### float getVcc (void)

VCC電圧を float 値で返す。
単位は V（ボルト）である。
精度は環境温度にも左右され ±0.15V 程度の誤差がある。

この関数を使うと一時的にAD変換基準電圧が VCC（VDD）に変更され、実行完了までには 10ms 以上の時間がかかる。
終了後AD変換基準電圧は元に戻されるがこの間の割込は禁止されていない。
また割込中や yield() 内から実行することは出来ない。

この関数を実行中・実行後に analogRead() を使用する場合は、同様に 10ms 以上の猶予を取らなければ期待した値は得られないだろう。

### float getCoreTemperature (void)

AVR内蔵温度感知器で取得したコア温度を float値で返す。
単位は K（ケルビン）であるため摂氏（セルシウス度）に直すには 273.15 を引く。

megaAVR-0 シリーズでは工場出荷時校正値が適用されるので、概ね当てになる値（環境温度より数度高い値）が得られる。

その他の AVRでは校正しない限り期待した値は得られない。また内蔵温度感知器を備えていないAVRコアではそもそも何も取得できない。

この関数を使うと一時的にAD変換基準電圧が 1.1Vに変更され、実行完了までには 10ms 以上の時間がかかる。
終了後AD変換基準電圧は元に戻されるがこの間の割込は禁止されていない。
また割込中や yield() 内から実行することは出来ない。

この関数を実行中・実行後に analogRead() を使用する場合は、同様に 10ms 以上の猶予を取らなければ期待した値は得られないだろう。

---

## 既知の不具合／制約／課題

- 主要な AVR 以外はテストされていない。megaAVR でも動作する。ESP32 は動作しない。
- 古い Arduino IDE には対応しない。1.8.12で動作確認。少なくとも C++11 は使えなければならない。
- 英文マニュアルが未整備である。

## 改版履歴

- 0.1
  - 初版

## 使用許諾

MIT

## 著作表示

朝日薫 / askn
(SenseWay Inc.)
Twitter: [@askn37](https://twitter.com/askn37)
GitHub: [https://github.com/askn37](https://github.com/askn37)

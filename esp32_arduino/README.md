# esp32_arduino（C++版）

ESP32（Arduinoフレームワーク）によるUIデバイス実装

---

# 概要

* LCD + エンコーダ + ボタンでUI操作
* HTTP経由でServerへリクエスト送信

---

# 構成

* main.cpp：状態管理
* menu：メニュー定義
* actions：API実行
* display：描画処理

---

# セットアップ

```bash
./01_make_container.sh
./05_build_flash_monitor.sh
```

---

# 備考

* Docker環境で再現可能
* Server APIに依存



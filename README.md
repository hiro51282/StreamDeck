# ESP32 StreamDeck（Docker再現プロジェクト）

物理ボタンでPCを操作する小型StreamDeck

ESP32を使ったローカル操作デバイスを構築したい人向けのプロジェクトです。

* ESP32でStreamDeckを作る
* 開発環境をDockerだけで再現する

ためのプロジェクトです。

---

# 目的

## メイン

* ESP32 + LCD + ボタン + ノブでStreamDeckを作る

## サブ

* Dockerのみで開発環境を完全再現
* IDE非依存
* 環境差異ゼロ

---

# 構成

* ESP32（UIデバイス）
* Flaskサーバ（処理実行）
* HTTPで連携

ESP32は「操作」
Serverは「実行」

---

# ディレクトリ

## esp32/

* PlatformIO + Docker環境
* main.cpp：エントリ
* actions / menu / display

## server/

* Flask API
* configはGit除外
* Python + Flask（venv前提）

---

# 使い方（最短）

```bash
./01_make_container.sh   # 初回のみ
./05_build_flash_monitor.sh
```

---

# Server起動

```bash
sdserver
```

（alias推奨）

---

# セキュリティ

* token一致でのみ実行
* LAN内限定
* 機密情報はGit除外

---

# 実装機能

* WOL
* Shutdown（confirmあり）
* CPU / Temp / Memory
* Ping
* Uptime

---

# 設計思想

* ESP32：UIに専念
* Server：処理を持つ
* 疎結合（HTTP）

---

# 状態

- ブレッドボード上で実機動作確認済み

---

# リリース

## v1.0.0

* StreamDeck基本機能完成
* メニューUI / confirm / スクロール対応
* Docker環境構築

初期完成版

# StreamDeck System（ESP32 + Server）

物理ボタンでPCを操作するローカルStreamDeckシステム

本プロジェクトは「仕様」と「実装」を分離し、 複数言語・複数構成で同一機能を再現可能な構成を持つ。

* UIデバイス（ESP32）
* 実行サーバ（HTTP API）

各実装は疎結合で設計されており、差し替え可能。

---

# アーキテクチャ

[ESP32] --HTTP--> [Server]

* ESP32：入力/UI担当
* Server：処理実行担当

---

# 実装バリエーション

## ESP32

* esp32_arduino/（C++）
* esp32_tinygo/（TinyGo）※予定

## Server

* server_py/（Flask）
* server_go/（Go）※予定

---

# API仕様

| Endpoint  | 内容        |
| --------- | --------- |
| /shutdown | PCシャットダウン |
| /status   | CPU使用率    |
| /temp     | 温度        |
| /memory   | メモリ使用率    |
| /ping     | 生存確認      |
| /uptime   | 起動時間      |

---

# ハード構成

* ESP32
* LCD1602（I2C）
* ロータリーエンコーダ（KY-040）
* ボタン

※ GPIO配線は実装依存

---

# セットアップ

## Server

### 起動前設定（共通）

* 設定ファイルを用意（Git管理外）

  * `server_py/config.py`（例: `config.py.example` をコピー）
* 必須設定

  * `TOKEN`：認証トークン（ESP32側と一致させる）
* ネットワーク

  * 同一LAN内で利用
  * ポート `5000` を使用（必要に応じて開放）

---

### 初回（追加時）

```bash
# Python版
cd server_py
pip install -r requirements.txt
python server.py
```

※ 将来的にDockerへ移行予定

### 切り替え（実装変更時）

```bash
# Python版
cd server_py
python server.py

# Go版（予定）
cd server_go
go run main.go
```

※ API仕様は共通のため、どちらでも動作可能

---

## ESP32

### 起動前設定（共通）

* 設定ファイルを用意（Git管理外）

  * `esp32/src/wifi_config.h`（例: `wifi_config.h.example` をコピー）
  * `esp32/src/wol_config.h`（例: `wol_config.h.example` をコピー）
* 必須設定

  * `WIFI_SSID` / `WIFI_PASS`
  * `SERVER_IP`（ServerのIPアドレス）
  * `TOKEN`（Server側と一致させる）

※ `TOKEN` と `SERVER_IP` はServer設定と一致させること

---

各実装ディレクトリを参照（Docker対応）

---

# 特徴

* 実装差し替え可能（C++ / TinyGo / Python / Go）
* Dockerによる環境再現
* 疎結合設計（HTTP）
* UIと処理の責務分離

---

# セキュリティ

* token一致でのみ実行
* LAN内限定
* 機密情報はGit除外

---

# 状態

* ブレッドボード上で実機動作確認済み

---

# リリース

## v1.0.0

* StreamDeck基本機能完成
* メニューUI / confirm / スクロール対応
* Docker環境構築

初期完成版

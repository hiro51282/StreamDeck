# StreamDeck System（ESP32 + Server）

物理ボタンでPCを操作するローカルStreamDeckシステム

本プロジェクトは「仕様」と「実装」を分離し、構成要素を疎結合にすることで再現性と拡張性を確保する。

* UIデバイス（ESP32）
* 実行サーバ（HTTP API）

---

# アーキテクチャ

[ESP32] --HTTP--> [Server]

* ESP32：入力/UI担当
* Server：処理実行担当

---

# 実装

## ESP32

* esp32_arduino/（C++）

## Server

* server_go/（Go）

※ ServerはGo実装に統一

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

## ケース設計

* 設計方針
* レイアウト
* 使用部品
* 配線構成
* ケース加工

## 構成

* ESP32
* LCD1602（I2C）
* ロータリーエンコーダ（KY-040）
* ボタン

※ GPIO配線は実装依存

---

# セットアップ

## Server（Go）

### 起動前設定

* 設定ファイルを用意（Git管理外）

  * `server_go/config.json`（例: `config.json.example` をコピー）
* 必須設定

  * `TOKEN`：認証トークン（ESP32側と一致させる）
* ネットワーク

  * 同一LAN内で利用
  * ポート `5000` を使用

---

### 開発実行

```bash
cd server_go
go build -o streamdeck-server
./streamdeck-server
```

---

### 本番運用

* systemd により自動起動
* バイナリは `/usr/local/bin/streamdeck-server` に配置

---

## ESP32

### 起動前設定

* 設定ファイルを用意（Git管理外）

  * `esp32/src/wifi_config.h`
  * `esp32/src/wol_config.h`
* 必須設定

  * `WIFI_SSID` / `WIFI_PASS`
  * `SERVER_IP`
  * `TOKEN`

※ Server設定と一致させること

---

# 特徴

* 疎結合設計（HTTP）
* UIと処理の責務分離
* Goによる軽量サーバ

※ Dockerは開発・検証用途でのみ使用

---

# セキュリティ

* token一致でのみ実行
* LAN内限定
* 機密情報はGit除外

---

# 状態

* Goサーバ + ESP32 による実機動作確認済み
* シャットダウン機能動作確認済み

---

# リリース

## v1.1.0

* Go版へ完全移行
* systemdによる常駐化
* シャットダウン機能安定化

---

# TODO

* プロセス一覧取得
* プロセスキル機能
* 動的メニュー対応

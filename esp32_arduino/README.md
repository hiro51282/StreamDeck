# esp32_arduino（C++版）

ESP32（Arduinoフレームワーク）によるUIデバイス実装（薄いクライアント）

---

# 概要

* LCD + エンコーダ + ボタンでUI操作
* HTTP経由でGoサーバへリクエスト送信
* **状態管理はサーバ側に集約**（ESP32は表示と入力のみ）

---

# 設計方針（重要）

* ESP32はステートレス（メニュー構造・遷移ロジックを持たない）
* すべての状態はサーバの `/menu` が返す State に従う
* 操作は `/action` に集約（up / down / select / back）
* サーバ疎通確認は `/ping` のみで判定

---

# アーキテクチャ

```text
ESP32（UI） ──HTTP──> Go Server（状態管理・処理）
```

---

# エンドポイント

## 表示取得

```http
GET /menu?token=<token>
```

* 現在のStateを取得（menu / cursor / items / result など）

## 操作送信

```http
POST /action?token=<token>
Content-Type: application/json

{ "type": "up" | "down" | "select" | "back" }
```

## 疎通確認

```http
GET /ping?token=<token>
```

---

# 初期化フロー（重要）

```text
起動
↓
WiFi接続（成功までリトライ）
↓
/ping でサーバ疎通確認
↓
OK？
 ├ YES → 通常モードへ
 └ NO
     ↓
     WOL送信
     ↓
     待機（数秒）
     ↓
     /ping 再試行（複数回）
     ↓
     失敗 → エラー表示
```

* ユーザ確認（Y/N）は行わない
* 自動復旧を優先

---

# 再接続フロー（重要）

通信エラー時のみ発火（initとは別）

```text
/menu or /action 失敗
↓
/ping
↓
NG
↓
（直前がshutdownでない場合のみ）
    WOL + リトライ
```

※ shutdown直後はWOLしない（直前操作で抑制）

---

# 通常動作フロー

```text
GET /menu
↓
表示（LCD）
↓
入力（エンコーダ / ボタン）
↓
POST /action
↓
GET /menu
↓
表示更新
```

---

# ディレクトリ構成（例）

* main.cpp：初期化・ループ・再接続制御
* http_client：API通信
* input：エンコーダ / ボタン
* display：LCD描画

※ メニュー定義は持たない（サーバ依存）

---

# 表示（例）

```text
[Connecting WiFi...]
[Checking Server...]
[Waking PC...]
[Retry 2/5...]
[ERROR: SERVER NOT FOUND]
```

---

# セットアップ

```bash
./01_make_container.sh
./05_build_flash_monitor.sh
```

---

# 注意事項

* メニュー構造をESP32側に持たない
* NextMenu / Actionなどを解釈しない
* 状態キャッシュを持たない

---

# 状態

* サーバAPI連携前提
* 初期化・再接続設計反映済み
* 動的メニュー対応前提

---

# TODO

* 描画最適化（差分更新）
* 入力デバイスの安定化
* エラーハンドリング強化

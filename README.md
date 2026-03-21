# ESP32 StreamDeck 開発環境（Docker Only）

このリポジトリは **ESP32開発環境をDockerのみで再現するプロジェクト**です。

目的は次の通りです。

* IDE依存をなくす
* 外部サービス障害で開発が止まらない
* いつでも同じ環境を再現できる
* コマンドを覚えなくても開発できる

VSCode DevContainerなどには依存しません。

開発環境は

```
Docker
PlatformIO CLI
ESP32
```

のみで構成されています。

---

# 必要なもの

ホストPCに以下が必要です。

* Docker
* ESP32

以上。

VSCodeなどのIDEは必須ではありません。

---

# ディレクトリ構成

```
StreamDeck
├ src
│  └ main.cpp
├ platformio.ini
├ Dockerfile
├ .dockerignore
│
├ 01_make_container.sh
├ 02_enter_container.sh
├ 03_flash.sh
├ 04_monitor.sh
└ 05_build_flash_monitor.sh
```

---

# 初回セットアップ

最初にDockerイメージを作成します。

```
./01_make_container.sh
```

この処理は

```
docker build -t esp32-dev .
```

を実行しています。

Dockerイメージはローカルに保存されるため

**一度作れば再作成は不要です。**

---

# 開発方法

## コンテナに入る

```
./02_enter_container.sh
```

成功すると

```
root@xxxx:/workspace#
```

が表示されます。

この状態で以下のコマンドが使えます。

ビルド

```
pio run
```

ESP32書き込み

```
pio run -t upload
```

シリアルモニタ

```
pio device monitor -b 115200
```

---

# 書き込みだけ実行

```
./03_flash.sh
```

ESP32へ直接書き込みを行います。

---

# シリアルモニタ

```
./04_monitor.sh
```

ESP32のシリアルログを表示します。

---

# 一発実行（おすすめ）

以下を実行すると

```
./05_build_flash_monitor.sh
```

次の処理をすべて自動で行います。

```
ビルド
↓
ESP32書き込み
↓
シリアルモニタ
```

開発時はこのスクリプトだけで十分です。

---

# 開発サイクル

普段の作業

```
./05_build_flash_monitor.sh
```

以上。

---

# 再開方法

別の日に開発を再開する場合。

```
cd StreamDeck
```

```
./05_build_flash_monitor.sh
```

Dockerイメージが残っている限り

**必ず同じ開発環境が起動します。**

---

# この構成の特徴

このプロジェクトは以下の問題を回避するために設計されています。

* IDE依存
* DevContainer障害
* 外部CDN障害

そのため

```
Docker + PlatformIO CLI
```

のみで構成されています。

これにより

* 再現性
* 安定性
* シンプルさ

を優先したESP32開発環境になっています。

---

# プロジェクト目的

このリポジトリは

```
ESP32
LCD1602
ボタン
ロータリーエンコーダ
```

を使った

**StreamDeck風ハードウェアUI**

を開発するためのベース環境です。

---

# 補足（ホスト側サーバ開発）

ESP32とは別に、PC側でHTTPサーバ（Flask）を動かす場合があります。

## ディレクトリ（リポジトリ外 or 別ディレクトリ）

```
StreamDeck/server
```

※ ESP32環境とは分離する（Dockerと混ぜない）

---

## 起動手順（毎回）

```bash
cd /path/to/StreamDeck/server
source venv/bin/activate
python server.py
```

---

## 初回のみ（venv作成）

```bash
python3 -m venv venv
source venv/bin/activate
pip install flask
```

---

## ワンコマンド起動（推奨）

```bash
alias sdserver="cd /path/to/StreamDeck/server && source venv/bin/activate && python server.py"
```

```bash
sdserver
```

---

## 注意

* ESP32は必ずこのリポジトリ内でDocker実行
* serverはホストで実行（Dockerに入れない）
* Pythonはvenv前提（Ubuntu仕様）

---

# 開発時の原則

* ESP32（Docker）とServer（ホスト）は分離する
* それぞれ独立して動作確認する
* 問題発生時は片方ずつ検証する

---

# サーバ運用（重要）

Flaskは開発用サーバのため

**起動・停止・自動起動の仕組みを用意する必要があります。**

---

## ■ 手動起動

```bash
sdserver
```

---

## ■ 手動停止

```bash
Ctrl + C
```

---

## ■ 自動起動（systemd）

サービスとして登録すると

* PC起動時に自動で立ち上がる
* 常時待ち受けになる

### ① サービスファイル作成

```bash
sudo nano /etc/systemd/system/streamdeck.service
```

```ini
[Unit]
Description=StreamDeck Flask Server
After=network.target

[Service]
User=<your-user>  # ← 実行ユーザー名に変更（例: ubuntu）
WorkingDirectory=/path/to/StreamDeck/server  # ← 自分の環境に合わせて変更
ExecStart=/path/to/StreamDeck/server/venv/bin/python server.py  # ← 自分の環境に合わせて変更
Restart=always

[Install]
WantedBy=multi-user.target
```

---

### ② 有効化

```bash
sudo systemctl daemon-reexec
sudo systemctl daemon-reload
sudo systemctl enable streamdeck
```

---

### ③ 起動

```bash
sudo systemctl start streamdeck
```

---

### ④ 状態確認

```bash
sudo systemctl status streamdeck
```

---

### ⑤ 停止

```bash
sudo systemctl stop streamdeck
```

---

### ⑥ 再起動

```bash
sudo systemctl restart streamdeck
```

---

## ■ ログ確認

```bash
journalctl -u streamdeck -f
```

---

# 運用方針

* 開発中 → 手動起動（sdserver）
* 常用段階 → systemdで常駐化

---

# セキュリティ設定（重要）

## ■ API仕様（簡易）

```
GET /shutdown?token=xxxx
```

* token一致 → 実行
* 不一致 → 403

※ LAN内利用を前提とし、外部公開はしないこと 

※ ポート5000はLAN内のみで使用すること（外部公開禁止）

---

本プロジェクトでは以下の情報をGitに含めません。

* WiFi SSID / PASSWORD
* サーバIP
* トークン

これらはローカルファイルで管理します。

---

## ■ ESP32側

```
wifi_config.h
```

例（Gitには含めない）

```
#define WIFI_SSID "your-ssid"
#define WIFI_PASS "your-password"
#define SERVER_IP "192.168.x.x"
#define TOKEN "your-token"
```

---

## ■ server側

```
config.py
```

```
TOKEN = "your-token"
```

---

## ■ .gitignore

※ 以下を必ず除外

```
wifi_config.h
config.py
.env
```

---

## ■ テンプレファイル（推奨）

```
wifi_config.h.example
config.py.example
```

コピーして使用

```
cp wifi_config.h.example wifi_config.h
cp config.py.example config.py
```

---

# sudo設定（重要）

シャットダウン機能を動かすため、以下を設定する必要があります。

```bash
sudo visudo
```

```
<your-user> ALL=(ALL) NOPASSWD: /usr/sbin/shutdown
```

※ 環境によりパスが異なるため確認

```bash
which shutdown
```

---

# 設計ポイント

* サーバは「常に待機する前提」で作る
* ESP32は「叩くだけ」にする
* サーバ側で機能を増減できる設計にする

これにより

* 機能追加が楽になる
* ESP32の書き換え頻度が減る
* システム全体が安定する

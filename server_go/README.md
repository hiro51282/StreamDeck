# server_go（Go版）

StreamDeck用HTTP APIサーバ（Go実装）

---

# 概要

* ESP32からのリクエストを受けて処理を実行
* Go標準ライブラリによる軽量HTTPサーバ

本サーバは2つのモードで利用される：

* ホスト実行：実際にPC操作（shutdownなど）を行う
* Docker実行：開発・動作確認用（危険操作は無効）

---

# エンドポイント

## UI制御（ESP32用）

* GET /menu

  * 現在のUI状態（State）を取得

* POST /action

  * ユーザー操作を送信
  * body: { "type": "up" | "down" | "select" | "back" }

## 内部サービス（直接利用は非推奨）

* /shutdown
* /status
* /temp
* /memory
* /ping
* /uptime

※ 上記はメニュー経由で呼び出される内部サービスとして扱う

---

# セットアップ

## ビルド（共通）

```bash
go build -o streamdeck-server
```

---

## 本番配置（推奨）

```bash
sudo mv streamdeck-server /usr/local/bin/streamdeck-server
```

※ パスを通すことで systemd や手動実行が安定する

---

## ホスト実行（本番・実機操作）

```bash
go run .
```

※ 開発時は go run . を使用（ビルド不要・即時反映）
※ shutdownなどのOS操作はこのモードでのみ有効bash
go build -o streamdeck-server
./streamdeck-server

````

※ shutdownなどのOS操作はこのモードでのみ有効

---

## Docker実行（開発・検証）

```bash
docker compose up -d --build
````

※ shutdownは無効（SKIPPED）

### 開発時の反映（DevEx）

コード変更後はコンテナを再起動することで反映される：

```bash
docker restart streamdeck_server_go
```

---

# サーバへの入り方（確認用）

### ホストで動かしている場合

```bash
cd server_go
go build -o streamdeck-server
./streamdeck-server
```

別ターミナルで確認：

```bash
whoami
```

---

### Dockerで動かしている場合

```bash
docker exec -it streamdeck_server_go /bin/sh
whoami
```

---

# モード切替（推奨）

開発/本番の混在を防ぐため、単一スクリプトで切替する。

## mode.sh

```bash
./mode.sh
```

## 動作

* Dockerが起動中 → 停止してホスト起動（PROD）
* Dockerが停止中 → Docker起動（DEV）

## 前提

* docker-compose.ymlでcontainer_nameを指定すること

例：

```yaml
container_name: streamdeck_server_go
```

※ これが無いとモード判定が崩れる

---

# ⚠️ 重要（初回セットアップ）

## 環境変数

シャットダウンは明示的に許可しない限り実行されない。

```bash
export ALLOW_SHUTDOWN=true
```

※ 未設定の場合はFAILになる（安全装置）

---

# ⚠️ 重要（初回セットアップ）

本サーバは「PCシャットダウン」を実行するため、事前に権限設定が必要。

## 初回のみ実行

### shutdown_setup.sh（例）

```bash
#!/bin/bash
set -e

# 実行用スクリプト配置
sudo tee /usr/local/bin/streamdeck_shutdown.sh > /dev/null << 'EOF'
#!/bin/bash
/sbin/shutdown -h now
EOF

sudo chmod +x /usr/local/bin/streamdeck_shutdown.sh

# sudo権限付与（パスワード不要・限定）
SUDO_FILE="/etc/sudoers.d/streamdeck"
echo "ubuntu ALL=(ALL) NOPASSWD: /usr/local/bin/streamdeck_shutdown.sh" | sudo tee $SUDO_FILE
sudo chmod 440 $SUDO_FILE

echo "[OK] setup complete"
```

---

### shutdown_remove.sh（例）

```bash
#!/bin/bash
set -e

sudo rm -f /usr/local/bin/streamdeck_shutdown.sh
sudo rm -f /etc/sudoers.d/streamdeck

echo "[OK] removed"
```

---

```bash
sudo ./shutdown_setup.sh
```

これにより：

* shutdown実行用スクリプト配置
* sudoパスワードなし実行の許可（限定）

## 削除（元に戻す）

```bash
sudo ./shutdown_remove.sh
```

## 注意点

* 設定はホストOSに影響する
* 信頼できる環境でのみ実行すること
* Docker実行時はshutdownは無効（SKIPPED）

---

# 自動起動（本番運用）

PC起動時にサーバを自動起動するには systemd に登録する。

## 手順

WorkingDirectoryは自分の環境に合わせて変更してください

```bash
sudo nano /etc/systemd/system/streamdeck.service
```

```ini
[Unit]
Description=StreamDeck Server
After=network.target

[Service]
User=ubuntu
WorkingDirectory=/home/path/to/work/StreamDeck/server_go
ExecStart=/usr/local/bin/streamdeck-server
Restart=always

[Install]
WantedBy=multi-user.target
```

```bash
sudo systemctl daemon-reexec
sudo systemctl daemon-reload
sudo systemctl enable streamdeck
```

## 起動 / 停止

```bash
sudo systemctl start streamdeck
sudo systemctl stop streamdeck
```

---

# 開発モード（ネイティブGo）

systemdで動作しているバイナリを停止し、`go run`で開発・デバッグを行う。

## 開発開始

```bash
sudo systemctl stop streamdeck
cd server_go
go run .
```

* printログがそのままコンソールに出力される
* ビルド不要で即時反映
* panicやエラーを直接確認可能

## 本番へ戻す

```bash
go build -o streamdeck-server
sudo mv streamdeck-server /usr/local/bin/streamdeck-server
sudo systemctl start streamdeck
```

---

```bash
sudo systemctl start streamdeck
sudo systemctl stop streamdeck
```

## 状態確認

```bash
systemctl status streamdeck
```

## 注意

* Dockerの restart: always と併用すると競合する
* 本番モードでのみ使用すること

---

# 備考

## 設計メモ（重要）

* サーバはState（現在のメニュー状態）を保持する
* ESP32は完全ステートレス
* ServiceFunc / routes は「内部サービス定義」であり、直接HTTPとして使う前提ではない
* 実際の操作は /action → 内部サービス呼び出し で実行される

---

# 備考

* tokenによる簡易認証
* LAN内利用想定

---

# 動作確認（テスト項目）

以下の4パターンを確認すること：

## 開発モード（Docker）

* shutdownが実行されない（SKIPPEDになる）
* temp / status / memory が正常取得できる

## 本番モード（ホスト）

* shutdownでPCが実際に落ちる
* temp / status / memory が正常取得できる

---

## 再起動時の挙動

Dockerの restart 設定に依存する：

* restart: always → 再起動後、自動でDocker（開発モード）になる

* restartなし → 再起動後は何も起動しない（手動でmode.sh実行）

※ systemdを有効にしている場合は「本番モードで自動起動」になる ※ Dockerのrestart設定とsystemdは併用しないこと

---

# モードの優先順位（重要）

以下の優先順位で動作する：

1. systemd（有効な場合） → 本番モードで起動
2. Docker（restart: always の場合） → 開発モードで起動
3. 手動実行（mode.sh）

---

# 状態

* /ping 実装済み
* /memory 実装済み
* /status 実装済み
* /uptime 実装済み
* /temp 実装済み
* /shutdown（DockerではSKIPPED / ホストで有効 ※未検証）
* Go版単体で動作確認済み
* Python版と機能同等（shutdown除く）

#

---

# テストコマンド

## API直呼び

```bash
curl localhost:5000/status?token=<your token>
curl localhost:5000/temp?token=<your token>
curl localhost:5000/memory?token=<your token>
curl localhost:5000/ping?token=<your token>
curl localhost:5000/uptime?token=<your token>
```

---

## 画面系

### 表示

```bash
curl localhost:5000/menu?token=<your token>
```

### 操作

```bash
curl -X POST "http://localhost:5000/action?token=<your token>" -H "Content-Type: application/json" -d '{"type":"select"}'
curl -X POST "http://localhost:5000/action?token=<your token>" -H "Content-Type: application/json" -d '{"type":"up"}'
curl -X POST "http://localhost:5000/action?token=<your token>" -H "Content-Type: application/json" -d '{"type":"down"}'
curl -X POST "http://localhost:5000/action?token=<your token>" -H "Content-Type: application/json" -d '{"type":"back"}'
```

---

## プロセスキル用テスト

```bash
bash -c "exec -a mytestproc killtarget > /dev/null"
```

※ CPUを消費するテストプロセス
※ kill機能の動作確認に使用

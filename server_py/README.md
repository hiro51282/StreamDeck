# server_py（Python / Flask版）

StreamDeck用HTTP APIサーバ（Python実装）

---

# 概要

* ESP32からのリクエストを受けて処理を実行
* Flaskベースの軽量API

---

# エンドポイント

* /shutdown
* /status
* /temp
* /memory
* /ping
* /uptime

---

# セットアップ

```bash
# venv前提
pip install -r requirements.txt
python server.py
```

---

# 備考

* tokenによる簡易認証
* LAN内利用想定

---

# TODO

* Docker対応（venv廃止）
* Go版との比較

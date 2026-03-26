#!/bin/bash

set -e

echo "[INFO] Removing StreamDeck shutdown..."

# === sudoers削除 ===
if [ -f /etc/sudoers.d/streamdeck ]; then
  sudo rm /etc/sudoers.d/streamdeck
  echo "[INFO] sudoers entry removed."
else
  echo "[INFO] sudoers entry not found."
fi

# === スクリプト削除 ===
if [ -f /usr/local/bin/streamdeck_shutdown.sh ]; then
  sudo rm /usr/local/bin/streamdeck_shutdown.sh
  echo "[INFO] shutdown script removed."
else
  echo "[INFO] shutdown script not found."
fi

echo "[DONE] Removal complete."
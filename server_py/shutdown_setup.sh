#!/bin/bash

set -e

echo "[INFO] Setting up StreamDeck shutdown..."

# === スクリプト配置 ===
sudo tee /usr/local/bin/streamdeck_shutdown.sh > /dev/null << 'EOF'
#!/bin/bash
/usr/sbin/shutdown now
EOF

sudo chmod +x /usr/local/bin/streamdeck_shutdown.sh

echo "[INFO] Shutdown script installed."

# === sudoers登録 ===
if [ -f /etc/sudoers.d/streamdeck ]; then
  echo "[INFO] sudoers already exists. Skipping."
else
  echo "ubuntu ALL=(ALL) NOPASSWD: /usr/local/bin/streamdeck_shutdown.sh" | sudo tee /etc/sudoers.d/streamdeck > /dev/null
  sudo chmod 440 /etc/sudoers.d/streamdeck
  echo "[INFO] sudoers entry added."
fi

echo "[DONE] Setup complete."
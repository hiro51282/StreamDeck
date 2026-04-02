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
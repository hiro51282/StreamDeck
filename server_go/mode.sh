#!/bin/bash

set -e

CONTAINER_NAME="streamdeck_server_go"
APP_DIR="$(cd "$(dirname "$0")" && pwd)"

echo "[INFO] Checking current mode..."

if docker ps --format '{{.Names}}' | grep -q "^${CONTAINER_NAME}$"; then
    echo "[MODE] DEV → PROD"

    docker compose down

    echo "[INFO] Starting server (host)..."
    cd "$APP_DIR"
    go build -o server
    ./server

else
    echo "[MODE] PROD → DEV"

    docker compose up -d --build
fi
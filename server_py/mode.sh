#!/bin/bash

set -e

CONTAINER_NAME="streamdeck_server"

echo "[INFO] Checking current mode..."

if docker ps --format '{{.Names}}' | grep -q "^${CONTAINER_NAME}$"; then
    echo "[MODE] DEV → PROD"

    docker-compose down

    echo "[INFO] Starting server (host)..."
    python3 server.py

else
    echo "[MODE] PROD → DEV"

    docker-compose up -d --build
fi
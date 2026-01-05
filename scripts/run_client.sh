#!/bin/bash
# Script chay Client trong WSL

echo "========================================"
echo "     ENGLISH LEARNING - CLIENT         "
echo "========================================"

cd /mnt/d/EnglishLearning

# Compile neu can
if [ ! -f "./client" ]; then
    echo "[INFO] Compiling client..."
    make clean && make
fi

# Kiem tra tham so
if [ -z "$1" ]; then
    echo ""
    echo "Usage: $0 <server_ip> [port]"
    echo ""
    echo "Examples:"
    echo "  $0 127.0.0.1          # Ket noi localhost"
    echo "  $0 192.168.1.100      # Ket noi may khac"
    echo "  $0 192.168.1.100 9999 # Chi dinh port"
    echo ""
    read -p "Nhap IP Server: " SERVER_IP
    if [ -z "$SERVER_IP" ]; then
        SERVER_IP="127.0.0.1"
    fi
else
    SERVER_IP=$1
fi

PORT=${2:-8888}

echo ""
echo "[INFO] Connecting to $SERVER_IP:$PORT..."
echo ""

./client $SERVER_IP $PORT

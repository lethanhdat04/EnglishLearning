#!/bin/bash
# Script chay Server trong WSL

echo "========================================"
echo "     ENGLISH LEARNING - SERVER         "
echo "========================================"

cd /mnt/d/EnglishLearning

# Compile neu can
if [ ! -f "./server" ]; then
    echo "[INFO] Compiling server..."
    make clean && make
fi

# Lay IP
WSL_IP=$(hostname -I | awk '{print $1}')
echo ""
echo "[INFO] WSL IP: $WSL_IP"
echo "[INFO] Port: 8888"
echo ""
echo "========================================"
echo " QUAN TRONG: Tren Windows (PowerShell Admin), chay:"
echo ""
echo " cd D:\\EnglishLearning\\scripts"
echo " .\\setup_server_wsl.ps1"
echo ""
echo "========================================"
echo ""
echo "[INFO] Starting server..."
echo ""

./server

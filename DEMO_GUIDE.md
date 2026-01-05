# Hướng Dẫn Demo Client-Server Trên 2 Máy Khác Nhau (WSL)

## Mục Lục
1. [Yêu Cầu](#yêu-cầu)
2. [Tình Huống 1: Cùng 1 Máy](#tình-huống-1-cùng-1-máy)
3. [Tình Huống 2: 2 Máy Khác Nhau](#tình-huống-2-2-máy-khác-nhau)
4. [Khắc Phục Lỗi](#khắc-phục-lỗi)

---

## Yêu Cầu

- Windows 10/11 với WSL2 đã cài đặt
- Ubuntu trong WSL với g++ compiler
- 2 máy cùng mạng LAN (cho tình huống 2)

---

## Tình Huống 1: Cùng 1 Máy

Đây là trường hợp đơn giản nhất - chạy cả server và client trên cùng một máy.

### Bước 1: Mở Terminal WSL thứ nhất - Chạy Server
```bash
cd /mnt/d/EnglishLearning
make clean && make
./server
```

### Bước 2: Mở Terminal WSL thứ hai - Chạy Client
```bash
cd /mnt/d/EnglishLearning
./client 127.0.0.1 8888
```

### Bước 3: (Tùy chọn) Mở thêm Terminal - Client thứ 2
```bash
cd /mnt/d/EnglishLearning
./client 127.0.0.1 8888
```

---

## Tình Huống 2: 2 Máy Khác Nhau

```
┌─────────────────────────────────────────────────────────────────┐
│                         MẠNG LAN                                 │
│                                                                  │
│  ┌──────────────────────┐      ┌──────────────────────┐         │
│  │   MÁY A (Server)     │      │   MÁY B (Client)     │         │
│  │   Windows + WSL      │      │   Windows + WSL      │         │
│  │                      │      │                      │         │
│  │  Windows IP:         │      │  Windows IP:         │         │
│  │  192.168.1.100       │      │  192.168.1.101       │         │
│  │         │            │      │         │            │         │
│  │    Port Forward      │      │         │            │         │
│  │    8888 → WSL        │      │         │            │         │
│  │         │            │      │         │            │         │
│  │  ┌──────▼─────┐      │      │  ┌──────┴─────┐      │         │
│  │  │    WSL     │◄─────┼──────┼──│    WSL     │      │         │
│  │  │  ./server  │      │ TCP  │  │  ./client  │      │         │
│  │  └────────────┘      │      │  └────────────┘      │         │
│  └──────────────────────┘      └──────────────────────┘         │
└─────────────────────────────────────────────────────────────────┘
```

### Trên MÁY A (Server)

#### Bước 1: Lấy IP Windows của máy A
Mở **CMD** hoặc **PowerShell**:
```powershell
ipconfig
```
Tìm **IPv4 Address**, ví dụ: `192.168.1.100`

#### Bước 2: Setup Port Forwarding
Mở **PowerShell với quyền Administrator** (Right-click → Run as Administrator):

**Cách 1: Chạy script tự động**
```powershell
cd D:\EnglishLearning\scripts
.\setup_server_wsl.ps1
```

**Cách 2: Chạy thủ công**
```powershell
# Lấy IP của WSL
wsl hostname -I
# Giả sử kết quả là: 172.25.123.45

# Tạo port forwarding
netsh interface portproxy add v4tov4 listenport=8888 listenaddress=0.0.0.0 connectport=8888 connectaddress=172.25.123.45

# Mở firewall
netsh advfirewall firewall add rule name="English Learning Server" dir=in action=allow protocol=TCP localport=8888
```

#### Bước 3: Chạy Server trong WSL
```bash
cd /mnt/d/EnglishLearning
./server
```

### Trên MÁY B (Client)

#### Bước 1: Copy project (nếu chưa có)
- Copy qua USB, network share, hoặc git clone

#### Bước 2: Compile trong WSL
```bash
cd /mnt/d/EnglishLearning
make clean && make
```

#### Bước 3: Kết nối đến Server
```bash
# Thay 192.168.1.100 bằng IP Windows của máy A
./client 192.168.1.100 8888
```

---

## Các Lệnh Hữu Ích

### Kiểm tra IP WSL
```bash
# Trong WSL
hostname -I
```

### Kiểm tra IP Windows
```powershell
# Trong PowerShell/CMD
ipconfig | findstr IPv4
```

### Kiểm tra Port Forwarding
```powershell
# Trong PowerShell Administrator
netsh interface portproxy show all
```

### Xóa Port Forwarding
```powershell
netsh interface portproxy delete v4tov4 listenport=8888 listenaddress=0.0.0.0
```

### Kiểm tra Firewall Rule
```powershell
netsh advfirewall firewall show rule name="English Learning Server"
```

### Xóa Firewall Rule
```powershell
netsh advfirewall firewall delete rule name="English Learning Server"
```

### Test kết nối từ máy Client
```bash
# Ping máy server
ping 192.168.1.100

# Kiểm tra port (cài netcat nếu chưa có: sudo apt install netcat-openbsd)
nc -zv 192.168.1.100 8888
```

---

## Khắc Phục Lỗi

### Lỗi: "Cannot connect to server"

1. **Kiểm tra server đang chạy**
   ```bash
   # Trên máy server
   ps aux | grep server
   ```

2. **Kiểm tra port forwarding**
   ```powershell
   netsh interface portproxy show all
   ```

3. **Kiểm tra firewall**
   - Tạm thời tắt Windows Firewall để test
   - Hoặc kiểm tra rule đã tạo

4. **Ping kiểm tra mạng**
   ```bash
   ping <IP_máy_server>
   ```

### Lỗi: WSL IP thay đổi sau khi restart

WSL2 có thể thay đổi IP mỗi lần khởi động. Chạy lại script setup:
```powershell
cd D:\EnglishLearning\scripts
.\setup_server_wsl.ps1
```

### Lỗi: Connection refused

1. Server chưa chạy hoặc đã crash
2. Port forwarding chưa được thiết lập
3. Firewall chặn kết nối

### Lỗi: Connection timed out

1. 2 máy không cùng mạng LAN
2. Router/Switch chặn traffic
3. IP server không đúng

---

## Demo Với GUI Client

```bash
# Trên máy có GTK3 cài đặt
./gui_app 192.168.1.100 8888
```

Nếu chưa có GTK3:
```bash
sudo apt update
sudo apt install libgtk-3-dev
make gui
```

---

## Tài Khoản Demo

| Email | Password | Role |
|-------|----------|------|
| student@test.com | 123456 | Student |
| teacher@test.com | 123456 | Teacher |
| admin@test.com | 123456 | Admin |

---

## Quick Start Commands

### Máy Server (MÁY A)
```powershell
# PowerShell Administrator
cd D:\EnglishLearning\scripts
.\setup_server_wsl.ps1
```

```bash
# WSL Terminal
cd /mnt/d/EnglishLearning && ./server
```

### Máy Client (MÁY B)
```bash
# WSL Terminal - thay IP tương ứng
cd /mnt/d/EnglishLearning
./client 192.168.1.100 8888
```

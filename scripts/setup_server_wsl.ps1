# PowerShell Script - Chay voi quyen Administrator
# Setup Port Forwarding tu Windows sang WSL cho Server

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  SETUP SERVER - PORT FORWARDING WSL   " -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

# Lay IP cua WSL
$wslIP = (wsl hostname -I).Trim().Split(" ")[0]
Write-Host "`n[INFO] WSL IP: $wslIP" -ForegroundColor Yellow

# Lay IP Windows
$windowsIP = (Get-NetIPAddress -AddressFamily IPv4 | Where-Object { $_.InterfaceAlias -notmatch "Loopback" -and $_.IPAddress -notmatch "^169" } | Select-Object -First 1).IPAddress
Write-Host "[INFO] Windows IP: $windowsIP" -ForegroundColor Yellow

# Xoa rule cu neu co
Write-Host "`n[INFO] Removing old port proxy rules..." -ForegroundColor Gray
netsh interface portproxy delete v4tov4 listenport=8888 listenaddress=0.0.0.0 2>$null

# Tao port forwarding moi
Write-Host "[INFO] Creating port forwarding: 0.0.0.0:8888 -> ${wslIP}:8888" -ForegroundColor Green
netsh interface portproxy add v4tov4 listenport=8888 listenaddress=0.0.0.0 connectport=8888 connectaddress=$wslIP

# Hien thi ket qua
Write-Host "`n[INFO] Current port proxy rules:" -ForegroundColor Cyan
netsh interface portproxy show all

# Tao firewall rule
Write-Host "`n[INFO] Adding firewall rule..." -ForegroundColor Green
$ruleName = "English Learning Server WSL"
$existingRule = Get-NetFirewallRule -DisplayName $ruleName -ErrorAction SilentlyContinue
if ($existingRule) {
    Remove-NetFirewallRule -DisplayName $ruleName
}
New-NetFirewallRule -DisplayName $ruleName -Direction Inbound -Protocol TCP -LocalPort 8888 -Action Allow | Out-Null

Write-Host "`n========================================" -ForegroundColor Green
Write-Host "  SETUP HOAN TAT!                      " -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host "`nClient tren may khac ket noi bang:" -ForegroundColor White
Write-Host "  ./client $windowsIP 8888" -ForegroundColor Yellow
Write-Host "  ./gui_app $windowsIP 8888" -ForegroundColor Yellow
Write-Host "`nBay gio hay chay server trong WSL:" -ForegroundColor White
Write-Host "  cd /mnt/d/EnglishLearning && ./server" -ForegroundColor Yellow
Write-Host ""

# PowerShell script to run the Memory Allocator GUI
Write-Host "Starting Memory Allocator GUI..." -ForegroundColor Green
Start-Process -FilePath "build\gui.exe"
Write-Host "GUI application started!" -ForegroundColor Green
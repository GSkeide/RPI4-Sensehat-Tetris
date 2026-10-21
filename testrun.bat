@echo off
rem Run from the folder this .bat file is in, so it finds stetris.c next to it
cd /d "%~dp0"

rem Open a PowerShell window that uploads, compiles and runs, then stays open
start "" powershell -NoExit -Command "Write-Host 'Uploading test.c to the Pi...'; scp test.c sushi-pi@sushi-pi.local:~/; if ($LASTEXITCODE -ne 0) { Write-Host 'Upload failed.' } else { Write-Host 'Compiling and running on the Pi...'; ssh -t sushi-pi@sushi-pi.local 'gcc test.c -o test && ./test' }"

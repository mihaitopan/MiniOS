@echo off
".\utils\makeFloppy.py" ".\boot\mbr.asm" ".\boot\ssl.asm"

REM ".\utils\Bochs-2.6.8\bochsdbg.exe" -q -f floppy.bxrc
".\utils\Bochs-2.6.8\bochsdbg.exe" -q -f bochsrc.bxrc

python ".\utils\decryptLog.py"

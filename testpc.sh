./pc emulation emu.json > log.txt 2> log.error.txt &
tail -F log.error.txt

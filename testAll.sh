echo > tests/ExitCodes;
for x in tests/*\.asm; do ./assemble $x ${x::-4}.obj; echo $x $? >> tests/ExitCodes; done

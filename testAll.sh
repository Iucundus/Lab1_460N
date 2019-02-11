for x in tests/*\.asm; do ./assemble $x ${x::-4}.obj; done

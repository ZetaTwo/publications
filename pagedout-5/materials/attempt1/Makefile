patched: target payload2.bin
	objcopy --dump-section .init=init.bin target
	objcopy --update-section .init=payload2.bin \
	 		--rename-section .init=.x \
			--add-section=.init=init.bin \
			--set-section-flags .init=alloc,code \
			--change-section-vma .init=$(shell objdump -j .init -h target | awk '/.init/ { print "0x" x $$4 }') target $@

target: target.c
	gcc -o $@ $<

#payload.o: payload.c
#	gcc -c -o $@ $<

#payload.bin: payload.o
#	objcopy --dump-section .text=$@ $^


payload2.o: payload2.asm
	nasm -felf64 -o $@ $^ 

payload2.bin: payload2.o
	objcopy --dump-section .init=$@ $^


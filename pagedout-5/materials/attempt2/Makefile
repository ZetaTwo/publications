patched: target
	objcopy --change-section-vma .section2=$(shell objdump -j .section1 -h target | awk '/.section1/ { print "0x" x $$4 }') target $@

target: target.c
	gcc -o $@ $<

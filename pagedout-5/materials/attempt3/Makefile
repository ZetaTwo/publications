
patched: base
	./patch.sh $^ $@

base: base.c
	gcc -fno-asynchronous-unwind-tables -o base base.c

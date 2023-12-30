.PHONY: build clean

build: driver-copy driver-paste

clean:
	rm -rf *.o driver-copy driver-paste

clipboard.o: clipboard.mm
	clang -c $<

driver-%: driver-%.c clipboard.o
	clang -framework Foundation -framework AppKit -o $@ $^

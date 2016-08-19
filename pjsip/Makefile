all: pjsipua

pjsipua: main.c
	$(CC) -o $@ $< `pkg-config --cflags --libs libpjproject`

.PHONY: clean
clean:
	rm -f pjsipua

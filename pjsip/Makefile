all: pjsipua options

pjsipua: main.c
	$(CC) -o $@ $< `pkg-config --cflags --libs libpjproject`

options: options.c
	$(CC) -o $@ $< `pkg-config --cflags --libs libpjproject`

.PHONY: clean
clean:
	rm -f pjsipua options

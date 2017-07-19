CFLAGS += `pkg-config --cflags --libs sofia-sip-ua`

all: sofua options

sofua: sofua.c
	$(CC) -o $@ $< $(CFLAGS)

options: options.c
	$(CC) -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f sofua options

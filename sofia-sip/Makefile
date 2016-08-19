CFLAGS += `pkg-config --cflags --libs sofia-sip-ua`

all: sofua

sofua: sofua.c
	$(CC) -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f sofua

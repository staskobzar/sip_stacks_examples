#
# Make eXosip UA
#
LDFLAGS += -leXosip2 -losip2 -losipparser2 -lpthread
CFLAGS += -O2
sipua: sipua.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf sipua

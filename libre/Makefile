#INCLUDES += `pkg-config --cflags-only-I libre`
#LFLAGS += `pkg-config --libs-only-L libre`
#LIBS += `pkg-config --libs-only-l libre`

# Makefile like in redemo package

LIBRE_MK  := $(shell [ -f ../re/mk/re.mk ] && \
	echo "../re/mk/re.mk")
ifeq ($(LIBRE_MK),)
LIBRE_MK  := $(shell [ -f /usr/share/re/re.mk ] && \
	echo "/usr/share/re/re.mk")
endif
ifeq ($(LIBRE_MK),)
LIBRE_MK  := $(shell [ -f /usr/local/share/re/re.mk ] && \
	echo "/usr/local/share/re/re.mk")
endif

include $(LIBRE_MK)

CFLAGS	+= -I$(LIBRE_INC)

all: reua options

reua: reua.o
	$(LD) $(LFLAGS) $< -L$(LIBRE_SO) -lre $(LIBS) -o $@

reua.o: reua.c
	$(CC) $(CFLAGS) -c $< -o $@ $(DFLAGS)

options: options.o
	$(LD) $(LFLAGS) $< -L$(LIBRE_SO) -lre $(LIBS) -o $@

options.o: options.c
	$(CC) $(CFLAGS) -c $< -o $@ $(DFLAGS)

.PHONY: clean
clean:
	rm -f reua options *.o *.d



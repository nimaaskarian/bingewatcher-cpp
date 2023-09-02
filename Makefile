include config.mk
all: bw

install-options:
	@echo bw install options:
	@echo "DESTDIR  = $(DESTDIR)"
	@echo "PREFIX   = $(PREFIX)"

options:
	@echo bw compile options:
	@echo "CFLAGS    = $(CFLAGS)"

bw: main.cpp lib/*.cpp options
	$(CC) main.cpp lib/*.cpp -o bw $(CFLAGS)

install: bw install-options
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f bw $(DESTDIR)$(PREFIX)/bin

uninstall: 
	rm -f $(DESTDIR)$(PREFIX)/bin/bw

clean:
	rm bw

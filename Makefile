prefix=/usr
    
all:
	gcc -g src/*.c -o src/exiftool
	
install: exiftool
	install -m 0755 exiftool $(prefix)/bin
	
uninstall: all
	rm $(prefix)/bin/exiftool

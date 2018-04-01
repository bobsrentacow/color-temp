pwd = $(shell pwd)

test: main.c libcolor-temp.so
	gcc -L$(pwd) -Wall -o test main.c -lcolor-temp
libcolor-temp.so: color_temp.o
	gcc -shared -Wall -o libcolor-temp.so color_temp.o -lm
color_temp.o: color_temp.c color_temp.h color_temp_table.h
	gcc -c -Wall -Werror -fpic color_temp.c
deb: libcolor-temp.so
	mkdir -p libcolor-temp/usr/local/bin
	cp libcolor-temp.so libcolor-temp/usr/local/bin/
	cp -r DEBIAN libcolor-temp/
	dpkg-deb --build libcolor-temp
.PHONY: clean
clean:
	rm -f test libcolor-temp.deb libcolor-temp.so color_temp.o
	rm -rf libcolor-temp/

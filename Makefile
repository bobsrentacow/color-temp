pwd = $(shell pwd)

test: main.c libcolor-temp.so
	gcc -L$(pwd) -Wall -o test main.c -lcolor-temp
libcolor-temp.so: color_temp.o
	gcc -shared -Wall -o libcolor-temp.so color_temp.o -lm
color_temp.o: color_temp.c color_temp.h color_temp_table.h
	gcc -c -Wall -Werror -fpic color_temp.c
.PHONY: install
install:
	cp libcolor-temp.so /usr/lib/
	chmod 0755 /usr/lib/libcolor-temp.so
	ldconfig
.PHONY: uninstall
uninstall:
	rm /usr/lib/libcolor-temp.so
	ldconfig
.PHONY: clean
clean:
	rm -f test libcolor-temp.so color_temp.o

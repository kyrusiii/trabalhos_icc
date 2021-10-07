LDLIBS = -lm -I/usr/local/include -L/usr/local/lib -lmatheval

ep01: ep01.c
	gcc ep01.c -o ep01 $(LDLIBS)

clean:

purge: clean 
	rm ep01

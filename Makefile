all: ask1.o ask2.o ask3.o ask4.o demo.o
	gcc -I/home/misc/courses/hy457/libcurl/lib/include -L/home/misc/courses/hy457/libcurl/lib/lib -o antivirus ask1.o ask2.o ask3.o ask4.o demo.o -lcurl -lcrypto
ask1.o: src/ask1.c
	gcc -c src/ask1.c -lcrypto
ask2.o: src/ask2.c
	gcc -I/home/misc/courses/hy457/libcurl/lib/include -L/home/misc/courses/hy457/libcurl/lib/lib -c src/ask2.c -lcurl
ask3.o: src/ask3.c
	gcc -c src/ask3.c 
ask4.o: src/ask4.c
	gcc -c src/ask4.c
demo.o: demo.c
	gcc -c demo.c
ask1:
	./antivirus scan Target
ask2:
	./antivirus inspect Target
ask3:
	./antivirus monitor Target
ask4:
	./antivirus slice 156
clean: 
	rm -rf *.o
	rm -rf antivirus

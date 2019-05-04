all: creePolyominos.exe

creePolyominos.exe: obj/creePolyominos.o obj/series.o
	gcc -o $@ $^

obj/creePolyominos.o: creePolyominos.cpp
	gcc -O3 -o $@ -c $< 

obj/series.o: series.cpp
	gcc -O3 -o $@ -c $<
	
clean:
	rm creePolyominos.exe obj/*.o
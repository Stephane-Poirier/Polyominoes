all: creePolyominos.exe

creePolyominos.exe: obj/creePolyominos.o obj/series.o obj/border_queue.o
	g++ -o $@ $^

obj/creePolyominos.o: creePolyominos.cpp
	g++ -O3 -o $@ -c $<

obj/series.o: series.cpp
	g++ -O3 -o $@ -c $<

obj/border_queue.o: border_queue.cpp
	g++ -O3 -o $@ -c $<

clean:
	rm creePolyominos.exe obj/*.o
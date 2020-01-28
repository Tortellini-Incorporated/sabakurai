PORT=4730

kurai: out/ src/kurai.cpp src/socket.cpp
	g++ -DPORT=$(PORT) src/socket.cpp src/kurai.cpp -lncurses -o out/kurai

okurai: out/ src/metestkurai.c
	gcc -DPORT=$(PORT) src/metestkurai.c -lncurses -o out/kurai

saba: out/ src/saba.c
	gcc -DPORT=$(PORT) src/saba.c src/server.c -o out/saba

curses: out/ out/title.o out/split.o out/box.o out/window.o out/sleep.o src/curses.cpp
	g++ $(flags) src/curses.cpp out/title.o out/split.o out/box.o out/window.o out/sleep.o -lncurses -o out/curses

out/title.o: out/ src/title.cpp src/title.hpp
	g++ $(flags) -c src/title.cpp -o out/title.o

out/split.o: out/ src/split.cpp src/split.hpp
	g++ $(flags) -c src/split.cpp -o out/split.o

out/box.o: out/ src/box.cpp src/box.hpp
	g++ $(flags) -c src/box.cpp -o out/box.o

out/window.o: out/ src/window.cpp src/window.hpp
	g++ $(flags) -c src/window.cpp -o out/window.o

out/sleep.o: out/ src/sleep.c src/sleep.h
	gcc -c src/sleep.c -o out/sleep.o

out:
	mkdir -p out

clean: 
	rm out/* -r

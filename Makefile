PORT=4730

kurai: out src/kurai.cpp src/socket.cpp
	g++ -DPORT=$(PORT) src/socket.cpp src/kurai.cpp -lncurses -o out/kurai

okurai: out src/metestkurai.c
	gcc -DPORT=$(PORT) src/metestkurai.c -lncurses -o out/kurai

saba: out src/saba.c
	gcc -DPORT=$(PORT) src/saba.c src/server.c -o out/saba

curses: out src/curses.cpp src/window.cpp
	g++ src/window.cpp src/curses.cpp -lncurses -o out/curses

out:
	mkdir -p out

clean: 
	rm out/* -r

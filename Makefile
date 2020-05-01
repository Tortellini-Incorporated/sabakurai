PORT=4730

sabakurai: out/socket.o out/command_prompt.o out/log.o out/player_list.o out/title.o out/split.o out/box.o out/window.o out/sleep.o src/sabakurai.cpp
	g++ $(flags) -DPORT=$(PORT) src/sabakurai.cpp out/socket.o out/command_prompt.o out/log.o out/player_list.o out/title.o out/split.o out/box.o out/window.o out/sleep.o -lncurses -o $@

saba: src/saba.c src/packetHandlers.c src/packetHandlers.h src/server.c src/packetHandlers.h
	gcc $(flags) -DPORT=$(PORT) src/saba.c src/packetHandlers.c src/server.c -o $@

key_test: src/key_test.cpp
	g++ $(flags) src/key_test.cpp -lncurses -o $@

out/socket.o: src/socket.cpp src/socket.hpp
	mkdir -p out/
	g++ $(flags) -c src/socket.cpp -o $@

out/command_prompt.o: src/command_prompt.cpp src/command_prompt.hpp
	mkdir -p out/
	g++ $(flags) -c src/command_prompt.cpp -o $@

out/log.o: src/log.cpp src/log.hpp
	mkdir -p out/
	g++ $(flags) -c src/log.cpp -o $@

out/player_list.o: src/player_list.cpp src/player_list.hpp
	mkdir -p out/
	g++ $(flags) -c src/player_list.cpp -o $@

out/title.o: src/title.cpp src/title.hpp
	mkdir -p out/
	g++ $(flags) -c src/title.cpp -o $@

out/split.o: src/split.cpp src/split.hpp
	mkdir -p out/
	g++ $(flags) -c src/split.cpp -o $@

out/box.o: src/box.cpp src/box.hpp
	mkdir -p out/
	g++ $(flags) -c src/box.cpp -o $@

out/window.o: src/window.cpp src/window.hpp
	mkdir -p out/
	g++ $(flags) -c src/window.cpp -o $@

out/sleep.o: src/sleep.c src/sleep.h
	mkdir -p out/
	gcc $(flags) -c src/sleep.c -o $@

.PHONY: clean
clean: 
	rm saba sabakurai key_test debug*.log out/* -rf

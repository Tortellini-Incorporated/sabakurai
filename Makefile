PORT=4730

kurai: out src/kurai.cpp src/socket.cpp
	g++ -lncurses -DPORT=$(PORT) src/socket.cpp src/kurai.cpp -o out/kurai

okurai: out src/metestkurai.c
	gcc -lncurses -DPORT=$(PORT) src/metestkurai.c -o out/kurai

saba: out src/saba.c
	gcc -DPORT=$(PORT) src/saba.c src/server.c -o out/saba

out:
	mkdir -p out

clean: 
	rm out/*

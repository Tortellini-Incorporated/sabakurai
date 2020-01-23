PORT=4730

kurai: out src/kurai.cpp src/socket.cpp
	g++ -lncurses -DPORT=$(PORT) src/socket.cpp src/kurai.cpp -o out/kurai

saba: out src/saba.c
	gcc -DPORT=$(PORT) src/saba.c -o out/saba

out:
	mkdir -p out

clean: 
	rm out/*

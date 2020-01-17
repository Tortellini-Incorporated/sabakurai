PORT=4730

kurai: src/kurai.c
	gcc -static -lncurses -DPORT=4730 src/kurai.c -o out/kurai

saba: src/saba.c
	gcc -static -DPORT=4730 src/saba.c -o out/saba

clean: 
	rm out/*

PORT=4730

kurai: src/kurai.c
	gcc -static -lncurses -DPORT=$(PORT) src/kurai.c -o out/kurai

saba: src/saba.c
	gcc -static -DPORT=$(PORT) src/saba.c -o out/saba

clean: 
	rm out/*

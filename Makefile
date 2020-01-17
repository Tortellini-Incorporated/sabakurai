PORT=4730

kurai: src/kurai.c
	gcc -lncurses -DPORT=$(PORT) src/kurai.c -o out/kurai

saba: src/saba.c
	gcc -DPORT=$(PORT) src/saba.c -o out/saba

clean: 
	rm out/*

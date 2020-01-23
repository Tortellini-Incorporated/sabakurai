PORT=4730

out:
	mkdir -p out

kurai: out src/kurai.c
	gcc -lncurses -DPORT=$(PORT) src/kurai.c -o out/kurai

okurai: out src/metestkurai.c
	gcc -lncurses -DPORT=$(PORT) src/metestkurai.c -o out/kurai

saba: out src/saba.c
	gcc -DPORT=$(PORT) src/saba.c src/server.c -o out/saba

clean: 
	rm out/*

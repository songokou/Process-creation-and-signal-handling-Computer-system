shell: shell.c
	gcc shell.c -o shell
monte: monte_carlo.c
	gcc monte_carlo.c -o monte
clean:
	rm -rf *.o shell
	rm -rf *.o monte


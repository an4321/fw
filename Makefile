
fw: fw.c
	@$(CC) fw.c -o fw -Wall -Wextra -pedantic -std=c99

clean:
	@rm fw

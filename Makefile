all: alpha_driver

alpha_lex.c: alpha_lex.l
	flex -o alpha_lex.c alpha_lex.l

alpha_driver: alpha_driver.c alpha_lex.c
	gcc -o alpha_driver alpha_driver.c alpha_lex.c

clean:
	rm -f alpha_driver alpha_lex.c

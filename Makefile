all: clean
	g++ gamblor.c -g -W -Wall -Werror -Wshadow

clean:
	\rm -rf a.out 

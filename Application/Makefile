all: Token_SRR

Token_SRR: main.o Sender.o Router.o Receiver.o Parser.o TokenGenerator.o
	gcc main.o Sender.o Router.o Receiver.o Parser.o TokenGenerator.o -o Token_SRR -lpthread

main.o: main.c
	gcc -c main.c

Sender.o: Sender.c
	gcc -c Sender.c

Router.o: Router.c
	gcc -c Router.c

Receiver.o: Receiver.c
	gcc -c Receiver.c

Parser.o: Parser.c
	gcc -c Parser.c

TokenGenerator.o: TokenGenerator.c
	gcc -c TokenGenerator.c

clean:
	rm -rf *o Token_SRR

SRC := src

all: servidor cliente

allCliente: cliente runCliente

allServidor: servidor runServidor

servidor: $(SRC)/servidor.c
	gcc -o $(SRC)/objServidor.o -c $(SRC)/servidor.c
	gcc -o servidor $(SRC)/objServidor.o
	
cliente: $(SRC)/cliente.c
	gcc -o $(SRC)/objCliente.o -c $(SRC)/cliente.c
	gcc -o cliente $(SRC)/objCliente.o

runCliente:
	./cliente

runServidor:
	./servidor

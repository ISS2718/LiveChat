SRC := src

all: servidor cliente

allCliente: cliente runCliente

allServidor: servidor runServidor

listaClientes: $(SRC)/listaClientes.c
	gcc -o $(SRC)/listaClientes.o -c $(SRC)/listaClientes.c

servidor: $(SRC)/servidor.c listaClientes
	gcc -o $(SRC)/objServidor.o -c $(SRC)/servidor.c
	gcc -o servidor.out $(SRC)/objServidor.o $(SRC)/listaClientes.o
	
cliente: $(SRC)/cliente.c listaClientes
	gcc -o $(SRC)/objCliente.o -c $(SRC)/cliente.c
	gcc -o cliente.out $(SRC)/objCliente.o $(SRC)/listaClientes.o

runCliente:
	./cliente.out

runServidor:
	./servidor.out

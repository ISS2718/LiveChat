SRC := src

DBFLAGS := -Wall -g3
COMMON_FLAGS := -march=native -O3 $(DEBUG)

all: servidor cliente

allCliente: cliente runCliente

allServidor: servidor runServidor

listaClientes: $(SRC)/listaClientes.c
	gcc $(COMMON_FLAGS) -o $(SRC)/listaClientes.o -c $(SRC)/listaClientes.c

servidor: $(SRC)/servidor.c listaClientes
	gcc $(COMMON_FLAGS) -o $(SRC)/objServidor.o -c $(SRC)/servidor.c
	gcc $(COMMON_FLAGS) -o servidor.out $(SRC)/objServidor.o $(SRC)/listaClientes.o
	
cliente: $(SRC)/cliente.c
	gcc $(COMMON_FLAGS) -o $(SRC)/objCliente.o -c $(SRC)/cliente.c
	gcc $(COMMON_FLAGS) -o cliente.out $(SRC)/objCliente.o

runCliente:
	./cliente.out

runServidor:
	./servidor.out

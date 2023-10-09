SRC := src

DBFLAGS := -Wall -g3
COMMON_FLAGS := -march=native -O3 $(DEBUG)

all: servidor cliente

allCliente: cliente runCliente

allServidor: servidor runServidor

listaClientes: $(SRC)/listaClientes.c
	gcc $(COMMON_FLAGS) -o $(SRC)/listaClientes.o -c $(SRC)/listaClientes.c

msg: $(SRC)/msg.c
	gcc $(COMMON_FLAGS) -o $(SRC)/msg.o -c $(SRC)/msg.c

servidor: $(SRC)/servidor.c listaClientes msg
	gcc $(COMMON_FLAGS) -o $(SRC)/objServidor.o -c $(SRC)/servidor.c
	gcc $(COMMON_FLAGS) -o servidor.out $(SRC)/objServidor.o $(SRC)/listaClientes.o $(SRC)/msg.o
	
cliente: $(SRC)/cliente.c msg
	gcc $(COMMON_FLAGS) -o $(SRC)/objCliente.o -c $(SRC)/cliente.c
	gcc $(COMMON_FLAGS) -o cliente.out $(SRC)/objCliente.o $(SRC)/msg.o

runCliente:
	./cliente.out

runServidor:
	./servidor.out

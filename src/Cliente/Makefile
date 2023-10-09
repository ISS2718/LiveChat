INCLUDES := ../Includes

DBFLAGS := -Wall -g3
COMMON_FLAGS := -march=native -O3 $(DEBUG)

all: cliente
	
cliente.o: cliente.c
	gcc $(COMMON_FLAGS) -o cliente.o -c cliente.c

main.o: main.c
	gcc $(COMMON_FLAGS) -o main.o -c main.c

cliente: main.o cliente.o $(INCLUDES)/listaClientes.o $(INCLUDES)/msg.o
	gcc $(COMMON_FLAGS) -o cliente.out main.o cliente.o $(INCLUDES)/listaClientes.o $(INCLUDES)/msg.o

run:
	./cliente.out

clear:
	rm *.o *.out
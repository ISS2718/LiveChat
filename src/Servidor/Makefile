INCLUDES := ../Includes

DBFLAGS := -Wall -g3
COMMON_FLAGS := -march=native -O3 $(DEBUG)

all: servidor
	
servidor.o: servidor.c
	gcc $(COMMON_FLAGS) -o servidor.o -c servidor.c

main.o: main.c
	gcc $(COMMON_FLAGS) -o main.o -c main.c

servidor: main.o servidor.o $(INCLUDES)/listaClientes.o $(INCLUDES)/msg.o
	gcc $(COMMON_FLAGS) -o servidor.out main.o servidor.o $(INCLUDES)/listaClientes.o $(INCLUDES)/msg.o

run:
	./servidor.out

clear:
	rm *.o *.out
DBFLAGS := -Wall -g3
COMMON_FLAGS := -march=native -O3 $(DEBUG)

all: listaClientes.o msg.o

listaClientes.o: listaClientes.c
	gcc $(COMMON_FLAGS) -o listaClientes.o -c listaClientes.c

msg.o: msg.c
	gcc $(COMMON_FLAGS) -o msg.o -c msg.c

clear:
	rm *.o
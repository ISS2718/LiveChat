SRC := src
SERVIDOR := $(SRC)/Servidor
CLIENTE := $(SRC)/Cliente
INCLUDES := $(SRC)/Includes

DBFLAGS := -Wall -g3
COMMON_FLAGS := -march=native -O3 $(DEBUG)

all: includes servidor cliente
	clear

allCliente: cliente runCliente

allServidor: servidor runServidor

includes: $(INCLUDES)/listaClientes.c $(INCLUDES)/msg.c
	$(MAKE) -C $(INCLUDES) all

servidor: includes $(SERVIDOR)/main.c $(SERVIDOR)/servidor.c
	$(MAKE) -C $(SERVIDOR) all
	
cliente: includes $(CLIENTE)/main.c $(CLIENTE)/cliente.c
	$(MAKE) -C $(CLIENTE) all

runCliente:
	$(CLIENTE)/cliente.out

runServidor:
	$(SERVIDOR)/servidor.out

clear:
	$(MAKE) -C $(INCLUDES) clear
	$(MAKE) -C $(SERVIDOR) clear
	$(MAKE) -C $(CLIENTE) clear
	clear

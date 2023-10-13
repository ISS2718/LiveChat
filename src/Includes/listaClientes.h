#ifndef _LISTA_CLIENTES_H_
#define _LISTA_CLIENTES_H_

#include <netinet/in.h>
#include <time.h>

#include "config.h"


/**
 * @struct InfoCliente
 * @brief Estrutura para armazenar informações sobre um cliente.
 * 
 * @var char InfoCliente::nome 
 * O nome do cliente.
 * 
 * @var char InfoCliente::user 
 * O nome de usuário do cliente.
 * 
 * @var int InfoCliente::moderador 
 * Indica se o cliente é um moderador.
 * 
 * @var int InfoCliente::cor 
 * A cor associada ao cliente.
 * 
 * @var int InfoCliente::mute 
 * Indica se o cliente está silenciado.
 */
typedef struct InfoCliente {
    char nome[TAM_NOME];
    char user[TAM_USER];
    int moderador;
    int cor;
    int mute;
}InfoCliente;

/**
 * @struct Cliente
 * @brief Estrutura para representar um cliente conectado ao servidor.
 * 
 * @var struct sockaddr_in Cliente::endereco 
 * O endereço do cliente.
 * 
 * @var InfoCliente Cliente::registro 
 * As informações do cliente.
 * 
 * @var struct Cliente * Cliente::proximo 
 * Um ponteiro para o próximo cliente na lista.
 */
typedef struct cliente {
    struct sockaddr_in endereco;
    InfoCliente registro;
    struct cliente * proximo;
}Cliente;

/**
 * @typedef ListaClientes
 * @brief Ponteiro para a estrutura Cliente, representando uma lista ligada de clientes.
 */
typedef Cliente * ListaClientes;

/**
 * @fn ListaClientes* criaListaClientes()
 * @brief Cria uma nova lista de clientes.
 * 
 * @return retorna um ponteiro para a lista criada.
 */
ListaClientes * criaListaClientes();

/**
 * @fn InfoCliente criaRegistroCliente(char* infoGeral)
 * @brief um regristro de cliente a partir de uma linha de informações sobre o cliente.
 * @param infoGeral string com informação do cliente.
 * @return registro de informações do cliente.
 *  
 * @note a string "infoGeral" deve ser uma linha com dois parâmetros iniciada com CODIGO_REGISTRO (definido em config.h) da seguinte forma: "#nome#usuário".
*/
InfoCliente criaRegistroCliente(char * infoGeral);

/**
 * @fn void liberaListaClientes(ListaClientes * listaClientes)
 * @brief Desaloca a memória reservada para a lista de clientes.
 * @param listaClientes lista de clientes conectados no servidor.
*/
void liberaListaClientes(ListaClientes * listaClientes);
Cliente * criaCliente(struct sockaddr_in endereco, InfoCliente registro);
void insereListaClientes(Cliente * cliente, ListaClientes * listaClientes);
void imprimeRegistro(InfoCliente registro);
void imprimeListaClientes(ListaClientes * listaClientes);

/**
 * @fn void imprimeListaClientes(ListaClientes* listaClientes)
 * @brief Verifica se existe cliente na lista, através do seu registro.
 * @param registro registro de informações do cliente.
 * @param listaClientes lista de clientes conectados no servidor.
 * @return cliente está na lista (0) ou não está (1).
*/
int enderecosIguais(struct sockaddr_in A, struct sockaddr_in B);

/**
 * @fn int existeClienteLista(InfoCliente registro, ListaClientes * listaClientes)
 * @brief Verifica se existe cliente na lista, através do seu endereço.
 * @param endereco endereço de informações do cliente.
 * @param listaClientes lista de clientes conectados no servidor.
 * @return cliente está na lista (0) ou não está (1).
*/
int existeClienteLista(InfoCliente registro, ListaClientes * listaClientes);

/**
 * @fn int existeEnderecoLista(struct sockaddr_in endereco, ListaClientes * listaClientes)
 * @brief Verifica se dois endereços são iguais.
 * @param A primeiro endereço.
 * @param B segundo endereço.
 * @return endereços iguais (1) ou endereços diferentes (0).
*/
int existeEnderecoLista(struct sockaddr_in endereco, ListaClientes * listaClientes);

/**
 * @fn Cliente * retornaClientePorEndereco(struct sockaddr_in endereco, ListaClientes * listaClientes)
 * @brief Retorna um cliente buscado pelo seu endereço.
 * @param endereco endereço do cliente a se buscar.
 * @param listaClientes lista de clientes conectados no servidor.
 * @return cliente encontrado ou NULL se não encontrado.
*/
Cliente * retornaClientePorEndereco(struct sockaddr_in endereco, ListaClientes * listaClientes);

/**
 * @fn Cliente * retornaClientePorUsuario(char * usuario, ListaClientes * listaClientes)
 * @brief Retorna um cliente buscado pelo seu usuário.
 * @param usuario endereço do cliente a se buscar.
 * @param listaClientes lista de clientes conectados no servidor.
 * @return cliente encontrado ou NULL se não encontrado.
*/
Cliente * retornaClientePorUsuario(char * usuario, ListaClientes * listaClientes);

/**
 * @fn InfoCliente retornaRegistroPorEndereco(struct sockaddr_in endCliente, ListaClientes * listaClientes)
 * @brief Retorna registro de um cliente buscando pelo seu endereço.
 * @param endCliente endereço do cliente a se buscar.
 * @param listaClientes lista de clientes conectados.
 * @return registros do cliente conectado, ou um registro com valores nulos.
*/
InfoCliente retornaRegistroPorEndereco(struct sockaddr_in endCliente, ListaClientes * listaClientes);

/**
 * @fn void removeClientePorEndereco(struct sockaddr_in endCliente, ListaClientes * listaClientes)
 * @brief Remove um cliente buscando pelo seu endereço.
 * @param endCliente endereço do cliente a se remover.
 * @param listaClientes lista de clientes conectados.
*/
void removeClientePorEndereco(struct sockaddr_in endCliente, ListaClientes * listaClientes);

/**
 * @fn void removeClientePorUsuario(char * usuario, ListaClientes * listaClientes)
 * @brief Remove um cliente buscando pelo seu usuário.
 * @param usuario usuário do cliente a se remover.
 * @param listaClientes lista de clientes conectados.
*/
void removeClientePorUsuario(char * usuario, ListaClientes * listaClientes);
#endif
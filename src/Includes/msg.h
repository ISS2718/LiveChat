#ifndef _MSG_H_
#define _MSG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "listaClientes.h"

/**
 * @def CLIENTE_JA_CONECTADO
 * @brief Macro de mensagem de cliente já comentado.
 *
 * @note Essa macro é printada somente no terminal do Servidor.
 */
#define CLIENTE_JA_CONECTADO "Cliente ja esta conectado! \n"

/**
 * @def SUCESSO_CONEXAO_CLIENTE
 * @brief Macro de mensagem de sucesso na conexão com o cliente.
 *
 * @note Essa macro é printada somente no terminal do Servidor.
 */
#define SUCESSO_CONEXAO_CLIENTE "Cliente foi conectado com sucesso.\n"

/**
 * @def ERRO_CONECTAR_CLIENTE
 * @brief Macro de mensagem de erro ao conectra o cliente.
 *
 * @note Essa macro é printada somente no terminal do Servidor.
 */
#define ERRO_CONECTAR_CLIENTE "Nao foi possivel conectar o cliente.\n"

/**
 * @def ERRO_MENSAGEM
 * @brief Macro de mensagem de erro ao enviar a mensagem.
 *
 * @note Essa macro é printada somente no terminal do Servidor.
 */
#define ERRO_MENSAGEM "Nao foi possivel enviar a mensagem.\n"

/**
 * @def CLIENTE_NAO_ENCONTRADO
 * @brief Macro de mensagem de cliente não encontrado.
 *
 * @note Essa macro é printada somente no terminal do Servidor.
 */
#define CLIENTE_NAO_ENCONTRADO "Cliente nao foi encontrado! \n"

/**
 * @def AVISO
 * @brief Macro para definir um print como uma mensagem de "aviso".
 *
 * Usado da seguinte forma printf(AVISO "mensagem").
 * 
 * @note Essa macro é utilizada tanto no Cliente e quanto no Servidor.
 */
#define AVISO "\x1B[31mA\x1B[33mV\x1B[31mI\x1B[33mS\x1B[31mO\x1B[33m:\x1B[39m "

/**
 * @def ERRO
 * @brief Macro para definir um print como uma mensagem de "erro".
 *
 * Usado da seguinte forma printf(ERRO "mensagem").
 * 
 * @note Essa macro é utilizada tanto no Cliente e quanto no Servidor.
 */
#define ERRO "\x1B[31mERRO:\x1B[39m "

/**
 * @def MENSAGEM
 * @brief Macro para definir um print como uma mensagem de "mensagem".
 *
 * Usado da seguinte forma printf(MENSAGEM "mensagem").
 * 
 * @note Essa macro é utilizada somente no Servidor.
 */
#define MENSAGEM "\x1B[33mMENSAGEM:\x1B[39m "

/**
 * @def SISTEMA
 * @brief Macro para definir um print como uma mensagem de "sistema".
 *
 * Usado da seguinte forma printf(SISTEMA "mensagem").
 * 
 * @note Essa macro é utilizada tanto no Cliente e quanto no Servidor.
 */
#define SISTEMA  "\x1B[33mSISTEMA:\x1B[39m "

/**
 * @def AMARELO
 * @brief Macro para definir a cor de escrita do terminal para amarelo.
 * 
 * Usado da seguinte forma printf(AMARELO "mensagem")  e normalmente 
 * combinado com a macro de RESET printf(AMARELO "mensagem" RESET).
 *
 * @note Essa macro é utilizada tanto no Cliente e quanto no Servidor.
 */
#define AMARELO "\x1B[33m"

/**
 * @def RESET
 * @brief Macro para redefinir a cor de escrita do terminal para padrão.
 * 
 * Usado da seguinte forma printf(RESET "mensagem")  e normalmente
 * combinado com a macro de AMARELO printf(AMARELO "mensagem" RESET).
 *
 * @note Essa macro é utilizada tanto no Cliente e quanto no Servidor.
 */
#define RESET "\x1B[39m"

/**
 * @fn void tirabarran(char* msg);
 * @brief Função para tirar "\n" do final da string.
 *
 * Esta função remove o caractere de nova linha ('\n') do final de uma string,
 * obtida pela funação fgets().
 *
 * @param msg Um ponteiro para a string que será modificada.
 *
 * @note Esta função só tira o '\n' do final da mensagem, isto é, antes do '\0'.
 */
void tirabarran(char* msg);
char * mensagemServidor(Cliente * cliente, const char objeto[], const char cor[]);
char * mensagemServidorClientes(Cliente * remetente, Cliente * destinatario, const char objeto[], const char cor []);
char * mensagemCliente(Cliente * cliente, const char objeto[]);
char * mensagemErro(const char objeto[]);

#endif
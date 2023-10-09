#ifndef _CONFIG_H_
#define _CONFIG_H_

#define PORTA 8080
#define TAM_MSG 256
#define TAM_NOME 64
#define TAM_USER 32

#define NUM_CORES_USERS 9
#define COR_MOD 10;

#define CODIGO_REGISTRO '#'

/**
 * @def USUARIO_CADASTRADO
 * @brief Servidor envia caso usuário já esteja cadastrado.
 * 
 * @note Só o servidor envia.
 */
#define USUARIO_CADASTRADO "/UC"

/**
 * @def DESCONECTAR
 * @brief Servidor envia caso o usuário deva ser desconectado
 * 
 * @note Só o servidor envia
 */
#define DESCONECTAR "/cabo"

/**
 * @def MUTE
 * @brief Código para silenciar um usuário.
 *
 * Este comando representa o comando para silenciar um usuário em uma string.
 * O comando será usado na forma "/mute usuário ação",
 * onde "usuário" é o nome do usuário que será mutado no servidor e "ação" pode assumir 2 valores true ou false
 * indicando se o usuário deve ser silenciado ou des-silenciado respoectivamente.
 *
 * @note Só funciona se for um moderador.
 */
#define MUTE "/mute"


/**
 * @def FECHAR_CLIENTE
 * @brief Código para desconectar do srevidor e fechar o cliente.
 *
 *
 * @note Funciona pra qualquer usuário.
 */
#define FECHAR_CLIENTE "/quit"

/**
 * @def FECHAR_SERVIDOR
 * @brief Código para  fechar desconectar todos os clientes e fechar servidor.
 *
 *
 * @note Só funciona se o usuário for um moderador.
 */
#define FECHAR_SERVIDOR "/fservidor"

/**
 * @def SUSSURRO
 * @brief Código para .
 *
 *
 * @note Funciona pra qualquer usuário.
 */
#define SUSSURRO "/priv"

/**
 * @def MOD
 * @brief Código para .
 *
 *
 * @note Só funciona se o usuário for um moderador.
 */
#define MOD "/mod"

/**
 * @def MOSTRAR_CLIENTES
 * @brief Código para .
 *
 *
 * @note Funciona pra qualquer usuário.
 */
#define MOSTRAR_CLIENTES "/users"


/**
 * @def MODERADOR1
 * @brief Usuário com nivel moderador por padrão.
 *
 * @note Esse usuario pode conceder nivel moderador temporário.
 */
#define MODERADOR1 "ikuyorih9"

/**
 * @def MODERADOR12
 * @brief Usuário com nivel moderador por padrão.
 *
 * @note Esse usuario pode conceder nivel moderador temporário.
 */
#define MODERADOR2 "iss2718"

#endif
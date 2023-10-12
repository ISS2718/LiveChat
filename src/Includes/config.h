#ifndef _CONFIG_H_
#define _CONFIG_H_

/**
 * @def PORTA
 * @brief Porta utilzada para conexão inicial com servidor.
 *
 * @note Essa macro é utilizada tanto no Cliente e quanto no Servidor.
 */
#define PORTA 8080

/**
 * @def TAM_MSG
 * @brief Macro de definição de tamanho da mensagem
 *
 * @note Essa macro é utilizada tanto no Cliente e quanto no Servidor.
 */
#define TAM_MSG 256

/**
 * @def COR_MOD
 * @brief Macro de definição de tamanho do nome real do usuário.
 *
 * @note Essa macro é utilizada tanto no Cliente e quanto no Servidor.
 */
#define TAM_NOME 64

/**
 * @def TAM_USER
 * @brief Macro de definição de tamanho de usuário.
 *
 * @note Essa macro é utilizada tanto no Cliente e quanto no Servidor.
 */
#define TAM_USER 32

/**
 * @def TAM_IP
 * @brief Macro de definição de tamanho de endereço de IP.
 *
 * @note Essa macro é utilizada somente no Cliente.
 */
#define TAM_IP 16


/**
 * @def NUM_CORES_USERS
 * @brief Macro de definição de quantidade de cores para usuários.
 *
 * @note Essa macro é utilizada somente no Servidor.
 */
#define NUM_CORES_USERS 9

/**
 * @def COR_MOD
 * @brief Macro de definição da cor de moderador.
 *
 * @note Essa macro é utilizada somente no Servidor.
 */
#define COR_MOD 10;

/**
 * @def CODIGO_REGISTRO
 * @brief Macro de definição do caracete para envio/recebimento do nome + usurário.
 *
 * @note Essa macro é utilizada tanto no Cliente e quanto no Servidor.
 */
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
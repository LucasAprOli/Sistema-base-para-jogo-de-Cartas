#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <stdexcept>
#include <string>

// ============================================================
// Q2 (A) — Hierarquia de excecoes propria do dominio. A base
// herda de std::runtime_error; as especificas herdam da base,
// permitindo captura tanto pela base (generica) quanto pelo
// tipo especifico (fina), conforme a necessidade de quem chama.
// (nomeada game_error, e nao domain_error, para nao colidir
// visualmente com o std::domain_error do <stdexcept>.)
// ============================================================
class game_error : public std::runtime_error {
public:
    using std::runtime_error::runtime_error; // herda os construtores
};

// Lancada quando os dados de uma carta violam uma invariante do
// dominio (nome vazio, custo de mana negativo etc.) — erro de
// VALIDACAO, detectado no proprio construtor de card.
class invalid_card_error : public game_error {
public:
    explicit invalid_card_error(const std::string& msg)
        : game_error("carta invalida: " + msg) {}
};

// Lancada quando uma operacao exige um RECURSO que nao esta
// disponivel: tabuleiro sem espaco livre, arquivo de estado
// ausente ou ilegivel no disco, etc.
class resource_unavailable_error : public game_error {
public:
    explicit resource_unavailable_error(const std::string& msg)
        : game_error("recurso indisponivel: " + msg) {}
};

#endif // ERRORS_HPP

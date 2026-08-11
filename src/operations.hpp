#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP

#include <optional>
#include <variant>
#include <functional>
#include <string>
#include <vector>
#include <memory>

#include "cards.hpp"

// ============================================================
// Q2 (B) — busca que PODE FALHAR: em vez de lançar exceção ou
// devolver ponteiro nulo, devolve std::optional. std::nullopt
// representa "não encontrada" de forma explícita no tipo de
// retorno. Usamos reference_wrapper porque card é abstrata (não
// pode ser copiada/retornada por valor).
// ============================================================
inline std::optional<std::reference_wrapper<const card>>
find_card_by_name(const std::vector<std::unique_ptr<card>>& deck,
                   const std::string& name) {
    for (const auto& c : deck) {
        if (c->get_name() == name) {
            return std::cref(*c); // achou
        }
    }
    return std::nullopt; // nao achou
}

// ============================================================
// Q2 (C) — variant: o resultado de "comprar" uma carta do topo do
// baralho é OU a própria carta (sucesso) OU uma mensagem de erro
// (baralho vazio) — dois tipos distintos, tratados com std::visit.
// ============================================================
using draw_result = std::variant<std::unique_ptr<card>, std::string>;

inline draw_result draw_card(std::vector<std::unique_ptr<card>>& deck) {
    if (deck.empty()) {
        return std::string{"baralho vazio: nao ha cartas para comprar"};
    }
    std::unique_ptr<card> top = std::move(deck.back());
    deck.pop_back();
    return top;
}

#endif // OPERATIONS_HPP

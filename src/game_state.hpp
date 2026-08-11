#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include <nlohmann/json.hpp>

#include <memory>
#include <string>
#include <vector>

#include "cards.hpp"
#include "errors.hpp"

using json = nlohmann::json;

// ============================================================
// Q4 (A)(B) — Estado completo a ser persistido: a versao do
// FORMATO (para evoluir o esquema no futuro sem quebrar arquivos
// antigos) e o baralho de cartas.
// ============================================================
struct game_state {
    int version = 1;
    std::vector<std::unique_ptr<card>> deck;
};

// ------------------------------------------------------------
// to_json não-intrusivo para cada tipo CONCRETO de carta
// (ADL padrão do nlohmann/json — nenhuma dessas classes conhece
// a biblioteca de JSON).
// ------------------------------------------------------------
inline void to_json(json& j, const monster_card& c) {
    j = json{{"type", c.type()},
             {"name", c.get_name()},
             {"mana_cost", c.get_mana_cost()},
             {"attack", c.get_attack()},
             {"defense", c.get_defense()}};
}

inline void to_json(json& j, const spell_card& c) {
    j = json{{"type", c.type()},
             {"name", c.get_name()},
             {"mana_cost", c.get_mana_cost()},
             {"effect", c.get_effect()},
             {"magic_power", c.get_magic_power()}};
}

inline void to_json(json& j, const trap_card& c) {
    j = json{{"type", c.type()},
             {"name", c.get_name()},
             {"mana_cost", c.get_mana_cost()},
             {"trap_power", c.get_trap_power()}};
}

// to_json para a BASE polimórfica: despacha para o overload do
// tipo concreto certo, guiado pelo campo "type" (Q4-B).
inline void to_json(json& j, const card& c) {
    if (c.type() == "monstro") { to_json(j, dynamic_cast<const monster_card&>(c)); return; }
    if (c.type() == "magia") { to_json(j, dynamic_cast<const spell_card&>(c)); return; }
    if (c.type() == "armadilha") { to_json(j, dynamic_cast<const trap_card&>(c)); return; }
    throw invalid_card_error("tipo desconhecido ao serializar: " + c.type());
}

// ------------------------------------------------------------
// Desserialização polimórfica: card é abstrata (não é default-
// construtível), então o from_json por ADL do nlohmann não se
// aplica diretamente a ela. Em vez disso usamos uma FÁBRICA
// manual que lê o campo "type" e constrói o tipo concreto certo
// — o mesmo papel do from_json, só que explícito.
// ------------------------------------------------------------
inline std::unique_ptr<card> card_from_json(const json& j) {
    const std::string type = j.at("type").get<std::string>();
    const std::string name = j.at("name").get<std::string>();
    const int mana_cost = j.at("mana_cost").get<int>();

    if (type == "monstro") {
        return std::make_unique<monster_card>(
            name, mana_cost, j.at("attack").get<int>(), j.at("defense").get<int>());
    }
    if (type == "magia") {
        return std::make_unique<spell_card>(
            name, mana_cost, j.at("effect").get<std::string>(), j.at("magic_power").get<int>());
    }
    if (type == "armadilha") {
        return std::make_unique<trap_card>(
            name, mana_cost, j.at("trap_power").get<int>());
    }
    throw invalid_card_error("tipo desconhecido na desserializacao: " + type);
}

// game_state completo -> json (grava o campo "version" na raiz).
inline json game_state_to_json(const game_state& state) {
    json j;
    j["version"] = state.version;
    j["deck"] = json::array();
    for (const auto& c : state.deck) {
        j["deck"].push_back(*c); // ADL -> to_json(json&, const card&)
    }
    return j;
}

// json -> game_state completo, tratando a versão do formato.
inline game_state game_state_from_json(const json& j) {
    game_state state;
    state.version = j.at("version").get<int>();

    // Q4 (B) — ponto de extensao para futuras versoes do formato:
    // hoje so a v1 e suportada; uma v2 poderia, por exemplo, migrar
    // campos antigos aqui antes de popular o game_state.
    if (state.version != 1) {
        throw game_error("versao de estado nao suportada: " + std::to_string(state.version));
    }

    for (const auto& card_json : j.at("deck")) {
        state.deck.push_back(card_from_json(card_json));
    }
    return state;
}

// Compara dois estados PELO CONTEUDO (game_state guarda unique_ptr,
// que não é copiável nem comparável diretamente) — usado nos
// testes de round-trip de serialização.
inline bool same_content(const game_state& a, const game_state& b) {
    return game_state_to_json(a) == game_state_to_json(b);
}

#endif // GAME_STATE_HPP

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <memory>
#include <vector>

#include "cards.hpp"
#include "generics.hpp"
#include "operations.hpp"
#include "game_state.hpp"
#include "repository.hpp"

// ============================================================
// TEST_CASE — destrutor virtual: cadeia derivada -> base (TP2).
// ============================================================
TEST_CASE("Destrutor virtual executa derivada antes da base", "[TP2]") {
    card* c = new monster_card("Teste", 3, 1000, 500);
    REQUIRE(c->get_name() == "Teste");
    REQUIRE(c->calculate_power() == Catch::Approx(1500.0f));
    delete c; // ~monster_card deve rodar antes de ~card
    SUCCEED("Objeto derivado destruido via ponteiro base sem erros");
}

// ============================================================
// TEST_CASE — polimorfismo dinâmico via vector<unique_ptr<card>> (TP2).
// ============================================================
TEST_CASE("Polimorfismo dinamico calcula o poder correto por derivada", "[TP2]") {
    std::vector<std::unique_ptr<card>> deck;
    deck.push_back(std::make_unique<monster_card>("Monstro", 5, 1200, 800));  // poder 2000
    deck.push_back(std::make_unique<spell_card>("Magia", 4, "efeito", 900));  // poder 900
    deck.push_back(std::make_unique<trap_card>("Armadilha", 2, 300));         // poder 300

    REQUIRE(deck[0]->calculate_power() == Catch::Approx(2000.0f));
    REQUIRE(deck[1]->calculate_power() == Catch::Approx(900.0f));
    REQUIRE(deck[2]->calculate_power() == Catch::Approx(300.0f));

    const card* strongest = highest_power_card(deck);
    REQUIRE(strongest != nullptr);
    REQUIRE(strongest->get_name() == "Monstro");
}

// ============================================================
// TEST_CASE — interface pura activatable, passada por referência (TP2).
// ============================================================
TEST_CASE("Interface pura activatable eh usada sem conhecer o tipo concreto", "[TP2]") {
    spell_card magic("Raio", 3, "Causa 400 de dano", 400);
    const activatable& ref = magic; // upcast para a interface pura
    REQUIRE_NOTHROW(ref.activate());

    trap_card trap("Barreira", 2, 250);
    REQUIRE_NOTHROW(use_activatable(trap));
}

// ============================================================
// TEST_CASE (Q1) — template genérico + CRTP + concept.
// ============================================================
TEST_CASE("registry<T> guarda itens de tipos diferentes e counted conta instancias", "[Q1]") {
    registry<int> reg;
    reg.add(10);
    reg.add(20);
    REQUIRE(reg.size() == 2);
    REQUIRE(reg.at(1) == 20);

    int alive_before = monster_card::alive();
    {
        monster_card temp("Temporaria", 1, 100, 100);
        REQUIRE(monster_card::alive() == alive_before + 1);
    }
    REQUIRE(monster_card::alive() == alive_before); // destruida ao sair do escopo
}

TEST_CASE("sum_total exige o concept calculable", "[Q1]") {
    std::vector<monster_card> monsters;
    monsters.emplace_back("A", 1, 100, 50);
    monsters.emplace_back("B", 1, 200, 30);
    REQUIRE(sum_total(monsters) == Catch::Approx(380.0));
    // sum_total(std::vector<int>{1,2,3}); // nao compila: int nao satisfaz calculable
}

// ============================================================
// TEST_CASE (Q2) — exceção pela base, optional e variant.
// ============================================================
TEST_CASE("Construtor de card lanca invalid_card_error, capturavel pela base", "[Q2]") {
    REQUIRE_THROWS_AS(monster_card("", 5, 10, 10), game_error);
    REQUIRE_THROWS_AS(monster_card("Nome valido", -1, 10, 10), game_error);
}

TEST_CASE("busca com optional cobre achou e nao achou", "[Q2]") {
    std::vector<std::unique_ptr<card>> deck;
    deck.push_back(std::make_unique<monster_card>("Alvo", 3, 500, 500));

    REQUIRE(find_card_by_name(deck, "Alvo").has_value());
    REQUIRE_FALSE(find_card_by_name(deck, "Fantasma").has_value());
}

TEST_CASE("draw_card usa variant para sucesso e erro", "[Q2]") {
    std::vector<std::unique_ptr<card>> deck;
    deck.push_back(std::make_unique<spell_card>("Unica", 1, "efeito", 10));

    draw_result success = draw_card(deck);
    REQUIRE(std::holds_alternative<std::unique_ptr<card>>(success));

    draw_result failure = draw_card(deck); // baralho ja vazio
    REQUIRE(std::holds_alternative<std::string>(failure));
}

// ============================================================
// TEST_CASE (Q4) — serialização round-trip e DIP com repositório em memória.
// ============================================================
TEST_CASE("Serializacao round-trip preserva o estado", "[Q4]") {
    game_state original;
    original.version = 1;
    original.deck.push_back(std::make_unique<monster_card>("M", 3, 100, 200));
    original.deck.push_back(std::make_unique<trap_card>("A", 2, 50));

    memory_repository repo; // sem tocar disco
    repo.save(original);
    game_state reloaded = repo.load();

    REQUIRE(same_content(original, reloaded));
}

TEST_CASE("game usa memory_repository via DIP, sem efeito colateral", "[Q4]") {
    game_state original;
    original.deck.push_back(std::make_unique<spell_card>("Feitico", 4, "efeito", 300));

    memory_repository repo;
    game g(repo); // injecao de dependencia

    g.save_match(original);
    game_state loaded = g.load_match();

    REQUIRE(same_content(original, loaded));
}

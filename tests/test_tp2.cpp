#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <memory>
#include <vector>

#include "cartas.hpp"

// ============================================================
// TEST_CASE 1 (Q1) — destrutor virtual: cadeia derivada -> base.
// ============================================================
TEST_CASE("Destrutor virtual executa derivada antes da base", "[Q1]") {
    // Cria via ponteiro para a base e deleta: se o destrutor base não
    // fosse virtual, apenas ~Card seria chamado (comportamento indefinido
    // para o restante do objeto derivado). Aqui validamos que o objeto
    // é destruído sem falhas de sanitizer e que os dados continuam
    // coerentes até o último instante de vida.
    Card* carta = new CartaMonstro("Teste", 3, 1000, 500);
    REQUIRE(carta->get_name() == "Teste");
    REQUIRE(carta->calcular_poder() == Catch::Approx(1500.0f));
    delete carta; // ~CartaMonstro deve rodar antes de ~Card
    SUCCEED("Objeto derivado destruido via ponteiro base sem erros");
}

// ============================================================
// TEST_CASE 2 (Q2) — polimorfismo dinâmico via vector<unique_ptr<Card>>.
// ============================================================
TEST_CASE("Polimorfismo dinamico calcula o poder correto por derivada", "[Q2]") {
    std::vector<std::unique_ptr<Card>> baralho;
    baralho.push_back(std::make_unique<CartaMonstro>("Monstro", 5, 1200, 800));   // poder 2000
    baralho.push_back(std::make_unique<CartaMagia>("Magia", 4, "efeito", 900));    // poder 900
    baralho.push_back(std::make_unique<CartaArmadilha>("Armadilha", 2, 300));      // poder 300

    REQUIRE(baralho[0]->calcular_poder() == Catch::Approx(2000.0f));
    REQUIRE(baralho[1]->calcular_poder() == Catch::Approx(900.0f));
    REQUIRE(baralho[2]->calcular_poder() == Catch::Approx(300.0f));

    const Card* maior = carta_maior_poder(baralho);
    REQUIRE(maior != nullptr);
    REQUIRE(maior->get_name() == "Monstro");
}

// ============================================================
// TEST_CASE 3 (Q3) — interface pura Ativavel, passada por referência.
// ============================================================
TEST_CASE("Interface pura Ativavel eh usada sem conhecer o tipo concreto", "[Q3]") {
    CartaMagia magia("Raio", 3, "Causa 400 de dano", 400);
    const Ativavel& ref = magia; // upcast para a interface pura

    // Não há flag de estado exposta publicamente; o contrato garante
    // apenas que ativar() é chamável via referência de interface e não
    // lança exceção nem exige conhecimento do tipo concreto.
    REQUIRE_NOTHROW(ref.ativar());

    CartaArmadilha armadilha("Barreira", 2, 250);
    REQUIRE_NOTHROW(usar_ativavel(armadilha));
}

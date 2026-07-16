#include <string>
#include <iostream>
#include <memory>
#include <vector>

#include "cartas.hpp"

class GameBoard {
private:
    std::shared_ptr<Card> active_cards_[5];
    int card_count_;

public:
    GameBoard() : card_count_(0) {
        for (int i = 0; i < 5; ++i) {
            active_cards_[i] = nullptr;
        }
        std::cout << "GameBoard criado.\n";
    }

    ~GameBoard() {
        std::cout << "~GameBoard destruido (As cartas no campo nao foram deletadas por ser uma agregacao).\n";
    }

    int get_card_count() const { return card_count_; }

    bool play_card(const std::shared_ptr<Card>& card) {
        if (card_count_ >= 5) {
            std::cout << "Tabuleiro cheio! Nao foi possivel jogar " << card->get_name() << ".\n";
            return false;
        }
        active_cards_[card_count_] = card;
        card_count_++;
        std::cout << "Carta \"" << card->get_name() << "\" colocada no tabuleiro.\n";
        return true;
    }
};

class Player {
private:
    std::string nickname_;
    int life_points_;
    std::unique_ptr<GameBoard> board_;

public:
    Player(std::string nickname, int life_points)
        : nickname_(nickname), life_points_(life_points) {
        board_ = std::make_unique<GameBoard>();
        std::cout << "Jogador \"" << nickname_ << "\" criado.\n";
    }

    ~Player() {
        std::cout << "~Player(\"" << nickname_ << "\") destruido.\n";
    }

    std::string get_nickname() const { return nickname_; }
    int get_life_points() const { return life_points_; }
    GameBoard* get_board() const { return board_.get(); }

    void receive_damage(int amount) {
        life_points_ -= amount;
        std::cout << "Jogador " << nickname_ << " recebeu " << amount << " de dano! ";
        if (life_points_ <= 0) {
            life_points_ = 0;
            std::cout << "O jogador foi derrotado!\n";
        } else {
            std::cout << "Vida restante: " << life_points_ << "\n";
        }
    }
};

class Match {
private:
    std::string stadium_name_;
    int turn_;

public:
    Match(std::string stadium_name) : stadium_name_(stadium_name), turn_(1) {
        std::cout << "Partida iniciada na arena \"" << stadium_name_ << "\".\n";
    }

    ~Match() {
        std::cout << "~Match(\"" << stadium_name_ << "\") encerrada.\n";
    }

    std::string get_stadium_name() const { return stadium_name_; }
    int get_turn() const { return turn_; }

    void next_turn() {
        turn_++;
        std::cout << ">>> Avancando para o Turno " << turn_ << "! <<<\n";
    }

    void display_match_status(const Player& player) const {
        std::cout << "[Arena: " << stadium_name_ << " | Turno: " << turn_
                  << " | Turno de: " << player.get_nickname() << " (LP: " << player.get_life_points() << ")]\n";
    }
};


int main() {
    std::cout << "Criando as Cartas:\n";
    std::shared_ptr<Card> carta1 = std::make_shared<CartaMonstro>("Mago Negro", 7, 2500, 2100);
    std::shared_ptr<Card> carta2 = std::make_shared<CartaMonstro>("Dragao Branco", 8, 3000, 2500);
    std::cout << "\n";

    std::cout << "Criando Player:\n";
    std::unique_ptr<Player> jogador = std::make_unique<Player>("Yugi", 4000);
    std::cout << "\n";

    std::cout << "Criando Match:\n";
    Match partida("Arena dos Duelistas");
    std::cout << "\n";

    partida.display_match_status(*jogador);
    std::cout << "\n";

    carta1->display_info();

    jogador->get_board()->play_card(carta1);
    jogador->get_board()->play_card(carta2);

    jogador->receive_damage(1500);
    std::cout << "\n";

    partida.next_turn();
    partida.display_match_status(*jogador);
    std::cout << "\n";

    // ========================================================
    // Q1 (C) — Destrutor virtual: derivada -> base, via ponteiro base.
    // ========================================================
    std::cout << "=== Q1 (C): destrutor virtual via ponteiro para a base ===\n";
    Card* carta_bruta = new CartaMonstro("Exodia", 10, 4000, 4000);
    delete carta_bruta;
    // Esperado: "~CartaMonstro" impresso ANTES de "~Card"
    std::cout << "\n";

    // ========================================================
    // Q2 — Polimorfismo dinâmico com vector<unique_ptr<Card>>.
    // ========================================================
    std::cout << "=== Q2: polimorfismo dinamico com vector<unique_ptr<Card>> ===\n";
    std::vector<std::unique_ptr<Card>> baralho;
    baralho.push_back(std::make_unique<CartaMonstro>("Guerreiro Lendario", 5, 2200, 1800));
    baralho.push_back(std::make_unique<CartaMagia>("Bola de Fogo", 4, "Causa 500 de dano direto", 500));
    baralho.push_back(std::make_unique<CartaArmadilha>("Espelho de Forca", 3, 300));

    std::cout << "\n-- Despacho polimorfico (display_info / calcular_poder) --\n";
    for (const auto& carta : baralho) {
        carta->display_info();
        std::cout << "  Poder calculado: " << carta->calcular_poder() << "\n";
    }

    std::cout << "\n-- Q2 (D): carta com maior poder, via funcao livre --\n";
    const Card* maior = carta_maior_poder(baralho);
    if (maior != nullptr) {
        std::cout << "Maior poder: " << maior->get_name() << " (" << maior->calcular_poder() << ")\n";
    }
    std::cout << "\n";

    // ========================================================
    // Q3 (D) — Uso da interface pura Ativavel por referência.
    // ========================================================
    std::cout << "=== Q3 (D): uso da interface pura Ativavel ===\n";
    CartaMagia trovao("Trovao Divino", 6, "Causa 800 de dano em area", 800);
    usar_ativavel(trovao); // a funcao so conhece Ativavel&, nao CartaMagia
    std::cout << "\n";

    std::cout << "=== Q2 (C): destruicao do vetor de cartas (derivada antes da base) ===\n";
    baralho.clear();
    std::cout << "\n";

    std::cout << "Destruindo o Player (e consequentemente o GameBoard por Composicao):\n";
    jogador.reset();
    std::cout << "\n";

    std::cout << "Verificando que as cartas continuam intactas na memoria:\n";
    carta1->display_info();
    std::cout << "\n";

    std::cout << "Limpeza manual final das cartas criadas no main:\n";
    carta1.reset();
    carta2.reset();

    return 0;
}

#include <string>
#include <iostream>

class Card {

private:
    std::string name_;
    int mana_cost_;

public:

    Card(std::string name, int mana_cost) : name_(name), mana_cost_(mana_cost) {
        std::cout << "Carta \"" << name_ << "\" criada de forma independente.\n";
    }

    ~Card() {
        std::cout << "~Card(\"" << name_ << "\") destruida da memoria externa.\n";
    }

    // Getters
    std::string get_name() const { return name_; }
    int get_mana_cost() const { return mana_cost_; }

    void display_info() const {
        std::cout << "[Carta: " << name_ << " | Custo de Mana: " << mana_cost_ << "]\n";
    }
};

class GameBoard {

private:

    Card* active_cards_[5];
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

    // Getter
    int get_card_count() const { return card_count_; }

    bool play_card(Card* card) {
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
    // Composição: O objeto dependente GameBoard é alocado dinamicamente via ponteiro primitivo
    GameBoard* board_; 

public:
    
    Player(std::string nickname, int life_points) 
        : nickname_(nickname), life_points_(life_points) {
        board_ = new GameBoard(); 
        std::cout << "Jogador \"" << nickname_ << "\" criado.\n";
    }

    ~Player() {
        delete board_; // Deleta o GameBoard dependente
        std::cout << "~Player(\"" << nickname_ << "\") destruido.\n";
    }

    // Getters
    std::string get_nickname() const { return nickname_; }
    int get_life_points() const { return life_points_; }
    GameBoard* get_board() const { return board_; }

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


int main() {
  
    Card* carta1 = new Card("Mago Negro", 7);
    Card* carta2 = new Card("Dragao Branco", 8);
    std::cout << "\n";

    std::cout << "[2] Criando o dono da Composicao (Player):\n";
    Player* jogador = new Player("Yugi", 4000);
    std::cout << "\n";

    carta1->display_info();
    
    jogador->get_board()->play_card(carta1);
    jogador->get_board()->play_card(carta2);
    
    jogador->receive_damage(1500);
    std::cout << "\n";

    delete jogador; 
    std::cout << "\n";

    std::cout << "[5] Verificando que as cartas continuam intactas na memoria:\n";
    carta1->display_info();
    std::cout << "\n";

    std::cout << "[6] Limpeza manual final das cartas criadas no main:\n";
    delete carta1;
    delete carta2;

    std::cout << "\n--- FIM ---\n";
  
    return 0;
}

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <ranges>
#include <thread>
#include <future>
#include <mutex>

#include "cards.hpp"
#include "generics.hpp"
#include "operations.hpp"
#include "game_state.hpp"
#include "repository.hpp"

// ============================================================
// Classes do dominio (TP1/TP2), com um ajuste pontual no Q4-TP3:
// game_board::play_card agora LANCA resource_unavailable_error
// quando o tabuleiro esta cheio, em vez de devolver bool — um
// erro real de "recurso indisponivel" (Q2-A).
// ============================================================
class game_board {
private:
    std::shared_ptr<card> active_cards_[5];
    int card_count_;

public:
    game_board() : card_count_(0) {
        for (int i = 0; i < 5; ++i) {
            active_cards_[i] = nullptr;
        }
        std::cout << "game_board criado.\n";
    }

    ~game_board() {
        std::cout << "~game_board destruido (As cartas no campo nao foram deletadas por ser uma agregacao).\n";
    }

    int get_card_count() const { return card_count_; }

    void play_card(const std::shared_ptr<card>& c) {
        if (card_count_ >= 5) {
            throw resource_unavailable_error(
                "sem espaco no tabuleiro para \"" + c->get_name() + "\" (maximo de 5 cartas)");
        }
        active_cards_[card_count_] = c;
        card_count_++;
        std::cout << "Carta \"" << c->get_name() << "\" colocada no tabuleiro.\n";
    }
};

class player {
private:
    std::string nickname_;
    int life_points_;
    std::unique_ptr<game_board> board_;

public:
    player(std::string nickname, int life_points)
        : nickname_(nickname), life_points_(life_points) {
        board_ = std::make_unique<game_board>();
        std::cout << "Jogador \"" << nickname_ << "\" criado.\n";
    }

    ~player() {
        std::cout << "~player(\"" << nickname_ << "\") destruido.\n";
    }

    std::string get_nickname() const { return nickname_; }
    int get_life_points() const { return life_points_; }
    game_board* get_board() const { return board_.get(); }

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

class match {
private:
    std::string stadium_name_;
    int turn_;

public:
    match(std::string stadium_name) : stadium_name_(stadium_name), turn_(1) {
        std::cout << "Partida iniciada na arena \"" << stadium_name_ << "\".\n";
    }

    ~match() {
        std::cout << "~match(\"" << stadium_name_ << "\") encerrada.\n";
    }

    std::string get_stadium_name() const { return stadium_name_; }
    int get_turn() const { return turn_; }

    void next_turn() {
        turn_++;
        std::cout << ">>> Avancando para o Turno " << turn_ << "! <<<\n";
    }

    void display_match_status(const player& p) const {
        std::cout << "[Arena: " << stadium_name_ << " | Turno: " << turn_
                  << " | Turno de: " << p.get_nickname() << " (LP: " << p.get_life_points() << ")]\n";
    }
};


int main() {
    std::cout << "Criando as Cartas:\n";
    std::shared_ptr<card> card1 = std::make_shared<monster_card>("Mago Negro", 7, 2500, 2100);
    std::shared_ptr<card> card2 = std::make_shared<monster_card>("Dragao Branco", 8, 3000, 2500);
    std::cout << "\n";

    std::cout << "Criando player:\n";
    std::unique_ptr<player> current_player = std::make_unique<player>("Yugi", 4000);
    std::cout << "\n";

    std::cout << "Criando match:\n";
    match duel("Arena dos Duelistas");
    std::cout << "\n";

    duel.display_match_status(*current_player);
    std::cout << "\n";

    card1->display_info();

    current_player->get_board()->play_card(card1);
    current_player->get_board()->play_card(card2);

    current_player->receive_damage(1500);
    std::cout << "\n";

    duel.next_turn();
    duel.display_match_status(*current_player);
    std::cout << "\n";

    std::cout << "=== Destrutor virtual via ponteiro para a base ===\n";
    card* raw_card = new monster_card("Exodia", 10, 4000, 4000);
    delete raw_card;
    // Esperado: "~monster_card" impresso ANTES de "~card"
    std::cout << "\n";

    std::cout << "=== Polimorfismo dinamico com vector<unique_ptr<card>> ===\n";
    std::vector<std::unique_ptr<card>> deck;
    deck.push_back(std::make_unique<monster_card>("Guerreiro Lendario", 5, 2200, 1800));
    deck.push_back(std::make_unique<spell_card>("Bola de Fogo", 4, "Causa 500 de dano direto", 500));
    deck.push_back(std::make_unique<trap_card>("Espelho de Forca", 3, 300));
    deck.push_back(std::make_unique<monster_card>("Cavaleiro Sombrio", 6, 1900, 2600));

    std::cout << "\n-- Despacho polimorfico (display_info / calculate_power) --\n";
    for (const auto& c : deck) {
        c->display_info();
        std::cout << "  Poder calculado: " << c->calculate_power() << "\n";
    }

    std::cout << "\n-- Carta com maior poder, via funcao livre --\n";
    const card* strongest = highest_power_card(deck);
    if (strongest != nullptr) {
        std::cout << "Maior poder: " << strongest->get_name() << " (" << strongest->calculate_power() << ")\n";
    }
    std::cout << "\n";

    std::cout << "=== Uso da interface pura activatable ===\n";
    spell_card thunder("Trovao Divino", 6, "Causa 800 de dano em area", 800);
    use_activatable(thunder); // a funcao so conhece activatable&, nao spell_card
    std::cout << "\n";

    // ============================================================
    // Q1 — Programacao Generica: Templates, CRTP, Concepts e Ranges
    // ============================================================
    std::cout << "=== Q1 (A): template registry<T> com dois tipos diferentes ===\n";
    registry<monster_card> monster_registry;
    monster_registry.add(monster_card("Golem de Pedra", 4, 1500, 2200));
    monster_registry.add(monster_card("Fenix Ardente", 6, 2100, 1200));
    std::cout << "registry<monster_card> tem " << monster_registry.size() << " itens; "
              << "primeiro: " << monster_registry.at(0).get_name() << "\n";

    registry<int> roll_registry; // mesmo template, tipo totalmente diferente
    roll_registry.add(6);
    roll_registry.add(3);
    roll_registry.add(5);
    std::cout << "registry<int> (rolagens de dado) tem " << roll_registry.size() << " itens.\n\n";

    std::cout << "=== Q1 (B): CRTP - contagem estatica de instancias, sem vtable ===\n";
    std::cout << "monster_card vivas: " << monster_card::alive() << "\n";
    std::cout << "spell_card vivas: " << spell_card::alive() << "\n";
    std::cout << "trap_card vivas: " << trap_card::alive() << "\n\n";

    std::cout << "=== Q1 (C)(D): concept calculable restringindo sum_total ===\n";
    std::vector<monster_card> loose_monsters;
    loose_monsters.emplace_back("Titan de Gelo", 5, 1800, 2000);
    loose_monsters.emplace_back("Serpente Marinha", 4, 1600, 1400);
    // sum_total so compila porque monster_card satisfaz o concept calculable
    // (tem calculate_power() const -> float, convertivel a double).
    std::cout << "Soma total de poder (calculable): " << sum_total(loose_monsters) << "\n\n";

    std::cout << "=== Q1 (E): pipeline de ranges (filter + transform encadeados) ===\n";
    namespace rv = std::ranges::views;
    auto expensive_names = deck
        | rv::filter([](const auto& c) { return c->get_mana_cost() > 4; })
        | rv::transform([](const auto& c) { return c->get_name(); });
    std::cout << "Cartas com custo de mana > 4:\n";
    for (const auto& name : expensive_names) {
        std::cout << "  - " << name << "\n";
    }
    std::cout << "\n";

    // ============================================================
    // Q2 — Tratamento de Erros: excecoes, optional e variant
    // ============================================================
    std::cout << "=== Q2 (A)(D): excecao especifica capturada PELA BASE ===\n";
    try {
        monster_card invalid_card("", -3, 100, 100); // nome vazio E mana negativa
        (void)invalid_card;
    } catch (const game_error& e) { // captura invalid_card_error pela base game_error
        std::cout << "Erro capturado pela base: " << e.what() << "\n";
    }
    std::cout << "\n";

    std::cout << "=== Q2 (A): resource_unavailable_error quando o tabuleiro esta cheio ===\n";
    try {
        current_player->get_board()->play_card(std::make_shared<monster_card>("Espadachim", 2, 500, 400));
        current_player->get_board()->play_card(std::make_shared<monster_card>("Arqueira", 2, 400, 300));
        current_player->get_board()->play_card(std::make_shared<monster_card>("Curandeira", 1, 200, 500));
        // aqui o tabuleiro atinge 5 cartas (card1, card2 + estas 3);
        // a proxima deve lancar resource_unavailable_error, capturada pela base.
        current_player->get_board()->play_card(std::make_shared<monster_card>("Excedente", 3, 700, 700));
    } catch (const game_error& e) {
        std::cout << "Erro capturado pela base: " << e.what() << "\n";
    }
    std::cout << "\n";

    std::cout << "=== Q2 (B): busca com optional, nos dois casos ===\n";
    auto found = find_card_by_name(deck, "Bola de Fogo");
    auto not_found = find_card_by_name(deck, "Carta Inexistente");
    if (found.has_value()) {
        std::cout << "Achou: " << found->get().get_name() << " (poder " << found->get().calculate_power() << ")\n";
    }
    std::cout << "Buscar carta inexistente: has_value() = " << std::boolalpha << not_found.has_value() << "\n\n";

    std::cout << "=== Q2 (C): variant tratado com std::visit ===\n";
    std::vector<std::unique_ptr<card>> small_deck;
    small_deck.push_back(std::make_unique<spell_card>("Cura Menor", 2, "Recupera 200 de vida", 200));

    for (int attempt = 0; attempt < 2; ++attempt) { // 1a compra funciona, 2a esvazia o baralho
        draw_result result = draw_card(small_deck);
        std::visit([](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::unique_ptr<card>>) {
                std::cout << "Comprou: " << value->get_name() << "\n";
            } else {
                std::cout << "Erro na compra: " << value << "\n";
            }
        }, result);
    }
    std::cout << "\n";

    // ============================================================
    // Q3 — STL e Concorrencia
    // ============================================================
    std::cout << "=== Q3 (A): containers STL distintos e justificados ===\n";
    // std::map: indice ORDENADO por nome, util para listar em ordem alfabetica.
    std::map<std::string, const card*> index_by_name;
    // std::unordered_set: garante UNICIDADE dos tipos de carta presentes, acesso O(1).
    std::unordered_set<std::string> types_present;
    for (const auto& c : deck) {
        index_by_name[c->get_name()] = c.get();
        types_present.insert(c->type());
    }
    std::cout << "Indice (map) tem " << index_by_name.size() << " cartas; "
              << "tipos unicos (unordered_set): " << types_present.size() << "\n\n";

    std::cout << "=== Q3 (B): algoritmos STL + lambda com captura ===\n";
    std::vector<const card*> sorted_cards;
    for (const auto& c : deck) sorted_cards.push_back(c.get());

    std::sort(sorted_cards.begin(), sorted_cards.end(),
              [](const card* a, const card* b) { return a->calculate_power() < b->calculate_power(); });

    float power_limit = 1000.0f;
    auto strong_count = std::count_if(sorted_cards.begin(), sorted_cards.end(),
        [power_limit](const card* c) { return c->calculate_power() > power_limit; }); // lambda com captura

    double total_power = std::accumulate(sorted_cards.begin(), sorted_cards.end(), 0.0,
        [](double acc, const card* c) { return acc + c->calculate_power(); });

    std::cout << "Mais fraca: " << sorted_cards.front()->get_name()
              << " | Mais forte: " << sorted_cards.back()->get_name() << "\n";
    std::cout << "Cartas com poder > " << power_limit << ": " << strong_count << "\n";
    std::cout << "Poder total do baralho: " << total_power << "\n\n";

    std::cout << "=== Q3 (C)(D): calculo paralelo, protegido por mutex ===\n";
    // Cada carta calcula seu proprio poder de forma totalmente independente
    // (calculate_power() so le o estado da propria carta) - por isso e
    // seguro paralelizar sem nenhuma dependencia entre as tarefas.
    std::mutex sum_mutex;
    double parallel_sum = 0.0;
    std::vector<std::future<double>> tasks;
    for (const auto& c : deck) {
        const card* ptr = c.get();
        tasks.push_back(std::async(std::launch::async, [ptr]() {
            return static_cast<double>(ptr->calculate_power());
        }));
    }
    for (auto& task : tasks) {
        double partial = task.get();                 // coleta o resultado da thread
        std::lock_guard<std::mutex> lock(sum_mutex);  // regiao critica
        parallel_sum += partial;
    }
    std::cout << "Soma paralela: " << parallel_sum << " | Soma serial (Q3-B): " << total_power
              << " | iguais? " << std::boolalpha << (parallel_sum == total_power) << "\n\n";

    // ============================================================
    // Q4 — Serializacao e SOLID
    // ============================================================
    std::cout << "=== Q4 (A)(B): serializacao/desserializacao JSON com round-trip ===\n";
    game_state original_state;
    original_state.version = 1;
    original_state.deck.push_back(std::make_unique<monster_card>("Hidra de Tres Cabecas", 8, 2600, 2400));
    original_state.deck.push_back(std::make_unique<spell_card>("Renascimento", 5, "Traz uma carta do cemiterio", 0));

    json_repository json_repo("estado.json");
    json_repo.save(original_state);
    game_state reloaded_state = json_repo.load();
    std::cout << "Round-trip via arquivo JSON: conteudo identico? "
              << std::boolalpha << same_content(original_state, reloaded_state) << "\n\n";

    std::cout << "=== Q4 (C)(D): DIP - game usa memory_repository (sem tocar disco) ===\n";
    memory_repository memory_repo;       // implementacao de TESTE
    game game_with_memory(memory_repo);  // injecao de dependencia no construtor
    game_with_memory.save_match(original_state);
    game_state state_via_memory = game_with_memory.load_match();
    std::cout << "Round-trip via memoria (sem arquivo): conteudo identico? "
              << std::boolalpha << same_content(original_state, state_via_memory) << "\n\n";

    // ========================================================
    // Limpeza final (ciclo de vida dos objetos do TP1/TP2).
    // ========================================================
    std::cout << "=== Destruicao dos vetores de cartas (derivada antes da base) ===\n";
    deck.clear();
    small_deck.clear();
    std::cout << "\n";

    std::cout << "Destruindo o player (e consequentemente o game_board por Composicao):\n";
    current_player.reset();
    std::cout << "\n";

    std::cout << "Limpeza manual final das cartas criadas no main:\n";
    card1.reset();
    card2.reset();

    return 0;
}

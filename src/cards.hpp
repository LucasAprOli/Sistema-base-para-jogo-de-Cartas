#ifndef CARDS_HPP
#define CARDS_HPP

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <utility>

#include "errors.hpp"
#include "generics.hpp"

// ============================================================
// Q3 (A) — Interface pura: modela uma CAPACIDADE (ativar efeito),
// independente de a carta ser Monstro, Magia ou Armadilha.
// Sem estado, todos os métodos são puramente virtuais.
// ============================================================
class activatable {
public:
    virtual void activate() const = 0;
    virtual ~activatable() = default;
};

// ============================================================
// Base abstrata da hierarquia de cartas.
// Q2 (A) — o construtor VALIDA os dados e lanca invalid_card_error
// (erro de dominio) quando uma invariante e violada, em vez de
// deixar o objeto ser criado em estado inconsistente.
// ============================================================
class card {
private:
    std::string name_;
    int mana_cost_;

public:
    card(std::string name, int mana_cost)
        : name_(std::move(name)), mana_cost_(mana_cost) {
        if (name_.empty()) {
            throw invalid_card_error("nome da carta nao pode ser vazio");
        }
        if (mana_cost_ < 0) {
            throw invalid_card_error(
                "custo de mana nao pode ser negativo (" + std::to_string(mana_cost_) + ")");
        }
        std::cout << "Carta \"" << name_ << "\" criada.\n";
    }

    // Método virtual puro — obriga toda derivada a definir seu "poder".
    virtual float calculate_power() const = 0;

    // Método virtual não-puro — tem implementação padrão na base.
    virtual void display_info() const {
        std::cout << "[Carta: " << name_ << " | Custo de Mana: " << mana_cost_ << "]";
    }

    // Q4 (B) — identifica o tipo concreto por texto; usado como
    // campo "type" na (de)serializacao para reconstruir o tipo certo.
    virtual std::string type() const = 0;

    std::string get_name() const { return name_; }
    int get_mana_cost() const { return mana_cost_; }

    // Destrutor virtual obrigatório — garante destruição correta via ponteiro base.
    virtual ~card() {
        std::cout << "~card(\"" << name_ << "\") destruida.\n";
    }
};

// ============================================================
// Derivada concreta 1: monster_card.
// Q1 (B) — tambem herda de counted<monster_card> (CRTP): ganha
// contagem estatica de instancias vivas sem custo de vtable.
// ============================================================
class monster_card : public card, public counted<monster_card> {
private:
    int attack_;
    int defense_;

public:
    monster_card(std::string name, int mana_cost, int attack, int defense)
        : card(std::move(name), mana_cost), attack_(attack), defense_(defense) {
        std::cout << "monster_card \"" << get_name() << "\" criada.\n";
    }

    float calculate_power() const override {
        return static_cast<float>(attack_ + defense_);
    }

    void display_info() const override {
        card::display_info(); // chama a versão da base antes de complementar
        std::cout << " [Monstro | ATK: " << attack_ << " | DEF: " << defense_ << "]\n";
    }

    std::string type() const override { return "monstro"; }
    int get_attack() const { return attack_; }
    int get_defense() const { return defense_; }

    ~monster_card() override {
        std::cout << "~monster_card(\"" << get_name() << "\") destruida.\n";
    }
};

// ============================================================
// Derivada concreta 2: spell_card.
// Herança múltipla: card (base abstrata) + activatable (interface pura)
// + counted<spell_card> (CRTP, Q1-B).
// ============================================================
class spell_card : public card, public activatable, public counted<spell_card> {
private:
    std::string effect_;
    int magic_power_;

public:
    spell_card(std::string name, int mana_cost, std::string effect, int magic_power)
        : card(std::move(name), mana_cost), effect_(std::move(effect)), magic_power_(magic_power) {
        std::cout << "spell_card \"" << get_name() << "\" criada.\n";
    }

    float calculate_power() const override {
        return static_cast<float>(magic_power_);
    }

    void display_info() const override {
        std::cout << "[Carta: " << get_name() << " | Custo de Mana: " << get_mana_cost()
                  << "] [Magia | Efeito: " << effect_ << "]\n";
    }

    void activate() const override {
        std::cout << "Magia \"" << get_name() << "\" ativada! Efeito: " << effect_ << "\n";
    }

    std::string type() const override { return "magia"; }
    const std::string& get_effect() const { return effect_; }
    int get_magic_power() const { return magic_power_; }

    ~spell_card() override {
        std::cout << "~spell_card(\"" << get_name() << "\") destruida.\n";
    }
};

// ============================================================
// Derivada concreta 3: trap_card, marcada como `final`.
// Justificativa (também no README): cartas de armadilha têm efeito de
// campo fixo e único; permitir subclasses quebraria a garantia de que
// toda armadilha ativa exatamente um efeito reverso pré-definido.
// ============================================================
class trap_card final : public card, public activatable, public counted<trap_card> {
private:
    int trap_power_;

public:
    trap_card(std::string name, int mana_cost, int trap_power)
        : card(std::move(name), mana_cost), trap_power_(trap_power) {
        std::cout << "trap_card \"" << get_name() << "\" criada.\n";
    }

    float calculate_power() const override {
        return static_cast<float>(trap_power_);
    }

    void activate() const override {
        std::cout << "Armadilha \"" << get_name() << "\" ativada!\n";
    }

    std::string type() const override { return "armadilha"; }
    int get_trap_power() const { return trap_power_; }

    ~trap_card() override {
        std::cout << "~trap_card(\"" << get_name() << "\") destruida.\n";
    }
};

// ============================================================
// Função livre que opera via ponteiro para a base e
// retorna a carta de maior "poder" (não-proprietária).
// ============================================================
inline const card* highest_power_card(const std::vector<std::unique_ptr<card>>& cards) {
    if (cards.empty()) return nullptr;
    const card* highest = cards.front().get();
    for (const auto& c : cards) {
        if (c->calculate_power() > highest->calculate_power()) {
            highest = c.get();
        }
    }
    return highest;
}

// ============================================================
// Função que recebe a interface pura por referência,
// sem conhecer o tipo concreto por trás dela.
// ============================================================
inline void use_activatable(const activatable& card_ref) {
    card_ref.activate();
}

#endif // CARDS_HPP

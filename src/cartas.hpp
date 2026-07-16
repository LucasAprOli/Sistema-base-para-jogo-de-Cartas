#ifndef CARTAS_HPP
#define CARTAS_HPP

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <utility>

// ============================================================
// Q3 (A) — Interface pura: modela uma CAPACIDADE (ativar efeito),
// independente de a carta ser Monstro, Magia ou Armadilha.
// Sem estado, todos os métodos são puramente virtuais.
// ============================================================
class Ativavel {
public:
    virtual void ativar() const = 0;
    virtual ~Ativavel() = default;
};

// ============================================================
// Q1 (A) — Base abstrata da hierarquia de cartas.
// ============================================================
class Card {
private:
    std::string name_;
    int mana_cost_;

public:
    Card(std::string name, int mana_cost)
        : name_(std::move(name)), mana_cost_(mana_cost) {
        std::cout << "Carta \"" << name_ << "\" criada.\n";
    }

    // Método virtual puro — obriga toda derivada a definir seu "poder".
    virtual float calcular_poder() const = 0;

    // Método virtual não-puro — tem implementação padrão na base.
    virtual void display_info() const {
        std::cout << "[Carta: " << name_ << " | Custo de Mana: " << mana_cost_ << "]";
    }

    std::string get_name() const { return name_; }
    int get_mana_cost() const { return mana_cost_; }

    // Destrutor virtual obrigatório — garante destruição correta via ponteiro base.
    virtual ~Card() {
        std::cout << "~Card(\"" << name_ << "\") destruida.\n";
    }
};

// ============================================================
// Q1 (B) — Derivada concreta 1: CartaMonstro.
// Sobrescreve display_info() chamando Card::display_info() (Q1-B).
// ============================================================
class CartaMonstro : public Card {
private:
    int ataque_;
    int defesa_;

public:
    CartaMonstro(std::string name, int mana_cost, int ataque, int defesa)
        : Card(std::move(name), mana_cost), ataque_(ataque), defesa_(defesa) {
        std::cout << "CartaMonstro \"" << get_name() << "\" criada.\n";
    }

    float calcular_poder() const override {
        return static_cast<float>(ataque_ + defesa_);
    }

    void display_info() const override {
        Card::display_info(); // chama a versão da base antes de complementar
        std::cout << " [Monstro | ATK: " << ataque_ << " | DEF: " << defesa_ << "]\n";
    }

    ~CartaMonstro() override {
        std::cout << "~CartaMonstro(\"" << get_name() << "\") destruida.\n";
    }
};

// ============================================================
// Q1 (B) / Q3 (B) — Derivada concreta 2: CartaMagia.
// Herança múltipla: Card (base abstrata) + Ativavel (interface pura).
// ============================================================
class CartaMagia : public Card, public Ativavel {
private:
    std::string efeito_;
    int poder_magico_;

public:
    CartaMagia(std::string name, int mana_cost, std::string efeito, int poder_magico)
        : Card(std::move(name), mana_cost), efeito_(std::move(efeito)), poder_magico_(poder_magico) {
        std::cout << "CartaMagia \"" << get_name() << "\" criada.\n";
    }

    float calcular_poder() const override {
        return static_cast<float>(poder_magico_);
    }

    void display_info() const override {
        std::cout << "[Carta: " << get_name() << " | Custo de Mana: " << get_mana_cost()
                  << "] [Magia | Efeito: " << efeito_ << "]\n";
    }

    void ativar() const override {
        std::cout << "Magia \"" << get_name() << "\" ativada! Efeito: " << efeito_ << "\n";
    }

    ~CartaMagia() override {
        std::cout << "~CartaMagia(\"" << get_name() << "\") destruida.\n";
    }
};

// ============================================================
// Q3 (C) — Derivada concreta 3: CartaArmadilha, marcada como `final`.
// Justificativa (também no README): cartas de armadilha têm efeito de
// campo fixo e único; permitir subclasses quebraria a garantia de que
// toda armadilha ativa exatamente um efeito reverso pré-definido.
// ============================================================
class CartaArmadilha final : public Card, public Ativavel {
private:
    int poder_armadilha_;

public:
    CartaArmadilha(std::string name, int mana_cost, int poder_armadilha)
        : Card(std::move(name), mana_cost), poder_armadilha_(poder_armadilha) {
        std::cout << "CartaArmadilha \"" << get_name() << "\" criada.\n";
    }

    float calcular_poder() const override {
        return static_cast<float>(poder_armadilha_);
    }

    void ativar() const override {
        std::cout << "Armadilha \"" << get_name() << "\" ativada!\n";
    }

    ~CartaArmadilha() override {
        std::cout << "~CartaArmadilha(\"" << get_name() << "\") destruida.\n";
    }
};

// ============================================================
// Q2 (D) — Função livre que opera via ponteiro para a base e
// retorna a carta de maior "poder" (não-proprietária).
// ============================================================
inline const Card* carta_maior_poder(const std::vector<std::unique_ptr<Card>>& cartas) {
    if (cartas.empty()) return nullptr;
    const Card* maior = cartas.front().get();
    for (const auto& carta : cartas) {
        if (carta->calcular_poder() > maior->calcular_poder()) {
            maior = carta.get();
        }
    }
    return maior;
}

// ============================================================
// Q3 (D) — Função que recebe a interface pura por referência,
// sem conhecer o tipo concreto por trás dela.
// ============================================================
inline void usar_ativavel(const Ativavel& carta) {
    carta.ativar();
}

#endif // CARTAS_HPP

#ifndef GENERICS_HPP
#define GENERICS_HPP

#include <concepts>
#include <cstddef>
#include <vector>

// ============================================================
// Q1 (A) — Template de classe genuinamente reutilizavel: um
// registro tipado que guarda qualquer tipo T e permite
// adicionar/consultar por indice. Nao e um apelido de
// std::vector: e reaproveitado no main() com DOIS tipos bem
// diferentes entre si (uma carta do dominio e um inteiro),
// mostrando que a abstracao nao esta presa a um unico uso.
// ============================================================
template <typename T>
class registry {
    std::vector<T> items_;

public:
    void add(T item) { items_.push_back(std::move(item)); }
    const T& at(std::size_t index) const { return items_.at(index); }
    std::size_t size() const { return items_.size(); }

    auto begin() const { return items_.begin(); }
    auto end() const { return items_.end(); }
};

// ============================================================
// Q1 (B) — CRTP (Curiously Recurring Template Pattern): mixin
// que conta quantas instancias de Derived estao vivas, SEM usar
// nenhuma funcao virtual — ou seja, sem o custo de uma vtable.
// Cada classe que herda de counted<Derived> ganha seu PROPRIO
// contador estatico (um por instanciacao do template), diferente
// de uma unica contagem global compartilhada.
// ============================================================
template <typename Derived>
class counted {
protected:
    counted() { ++count_; }
    counted(const counted&) { ++count_; }
    counted(counted&&) noexcept { ++count_; }
    ~counted() { --count_; }

public:
    static int alive() { return count_; }

private:
    static inline int count_ = 0;
};

// ============================================================
// Q1 (C) — Concept (C++20): restringe um tipo T a ser
// "calculavel", isto e, a expor um metodo calculate_power() const
// cujo retorno seja convertivel para double.
// ============================================================
template <typename T>
concept calculable = requires(const T& t) {
    { t.calculate_power() } -> std::convertible_to<double>;
};

// ============================================================
// Q1 (D) — sum_total() so aceita tipos que satisfazem o
// concept calculable. Chamar sum_total com, por exemplo,
// std::vector<int> nao compila: o erro aponta diretamente para
// "constraints not satisfied" no concept calculable, em vez de
// uma cascata obscura de erros dentro do corpo do template.
//
// Experimento (NAO habilitado de proposito, para manter o build
// limpo — descomente para reproduzir o erro):
//
//     sum_total(std::vector<int>{1, 2, 3});
//     // erro: constraints not satisfied for template <calculable T>
//     //       ... nao existe t.calculate_power() para int
// ============================================================
template <calculable T>
double sum_total(const std::vector<T>& items) {
    double total = 0.0;
    for (const auto& item : items) total += item.calculate_power();
    return total;
}

#endif // GENERICS_HPP

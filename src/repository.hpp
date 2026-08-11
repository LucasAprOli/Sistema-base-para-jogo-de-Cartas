#ifndef REPOSITORY_HPP
#define REPOSITORY_HPP

#include <fstream>
#include <string>

#include "errors.hpp"
#include "game_state.hpp"

// ============================================================
// Q4 (C) — DIP (Dependency Inversion Principle): esta é a
// ABSTRAÇÃO. Classes de alto nível (game, abaixo) dependem
// apenas desta interface, nunca de uma implementação concreta.
// ============================================================
class repository {
public:
    virtual void save(const game_state& state) = 0;
    virtual game_state load() = 0;
    virtual ~repository() = default;
};

// ------------------------------------------------------------
// Implementação de PRODUÇÃO: persiste o estado em um arquivo
// JSON no disco.
// ------------------------------------------------------------
class json_repository : public repository {
    std::string path_;

public:
    explicit json_repository(std::string path) : path_(std::move(path)) {}

    void save(const game_state& state) override {
        std::ofstream output(path_);
        if (!output) {
            throw resource_unavailable_error("nao foi possivel abrir para escrita: " + path_);
        }
        output << game_state_to_json(state).dump(2);
    }

    game_state load() override {
        std::ifstream input(path_);
        if (!input) {
            throw resource_unavailable_error("arquivo de estado nao encontrado: " + path_);
        }
        json j;
        input >> j;
        return game_state_from_json(j);
    }
};

// ------------------------------------------------------------
// Implementação de TESTE: guarda o estado em memória, sem tocar
// disco nem rede — permite exercitar a lógica de alto nível
// (game) sem NENHUM efeito colateral.
// ------------------------------------------------------------
class memory_repository : public repository {
    json data_;
    bool has_data_ = false;

public:
    void save(const game_state& state) override {
        data_ = game_state_to_json(state);
        has_data_ = true;
    }

    game_state load() override {
        if (!has_data_) {
            throw resource_unavailable_error("nenhum estado foi salvo em memoria ainda");
        }
        return game_state_from_json(data_);
    }
};

// ============================================================
// Q4 (C) — Classe de ALTO NÍVEL: só conhece a abstração
// repository, recebida por INJEÇÃO DE DEPENDÊNCIA no
// construtor (nunca instancia uma implementação concreta).
// ============================================================
class game {
    repository& repository_;

public:
    explicit game(repository& repo) : repository_(repo) {}

    void save_match(const game_state& state) { repository_.save(state); }
    game_state load_match() { return repository_.load(); }
};

#endif // REPOSITORY_HPP

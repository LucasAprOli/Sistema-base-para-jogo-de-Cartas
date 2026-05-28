# Sistema-base-para-jogo-de-Cartas
Projeto POO - UFPB
por: Lucas Aprígio Santos de Oliveira    mat:20250019231

Descrição do Domínio:
  - Este projeto se trata de um sistema base para um jogo de cartas inspirados em jogos classicos como (gwent, yu-gi-oh, pokemon tcg).

## Diagrama UML
<img width="332" height="711" alt="image" src="https://github.com/user-attachments/assets/69b7a34b-b901-48de-8a66-5358804067ad" />

A relação entre o tabuleiro (GameBoard) e as cartas (Card) é uma Agregação.
As cartas existir independentemente do "todo" (o tabuleiro). As cartas geralmente são criadas em um deck ou na mão do jogador antes de irem para o tabuleiro. Se o tabuleiro for destruído (por exemplo, se a rodada acabar), as cartas em si não devem ser deletadas da memória do sistema se ainda pertencerem ao deck do jogador.

A relação entre o jogador (Player) e o seu tabuleiro (GameBoard) é uma Composição.
Nesse caso o ciclo de vida do tabuleiro está atrelado ao do jogador. Se o jogador sumir não tem sentido existir um tabuleiro.

## Uso de Smart Pointers

* **`Player` usa `unique_ptr` para o `GameBoard`:** O jogador é o único dono do seu tabuleiro. Se o jogador for destruído, o tabuleiro é destruído automaticamente com ele.
* **`GameBoard` usa `shared_ptr` para as `Card`s:** O tabuleiro apenas "pega emprestado" as cartas. Se o tabuleiro for destruído, as cartas continuam vivas na memória.
* **`main` usa `unique_ptr` para o `Player`:** O sistema do jogo (`main`) é o único dono do jogador.
* **`main` usa `shared_ptr` para as `Card`s:** As cartas nascem independentes no sistema do jogo e depois têm seu acesso compartilhado com o tabuleiro.

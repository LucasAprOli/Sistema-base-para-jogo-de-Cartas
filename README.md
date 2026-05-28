# Sistema-base-para-jogo-de-Cartas
Projeto POO - UFPB
por: Lucas Aprígio Santos de Oliveira    mat:20250019231

Descrição do Domínio:
  - Este projeto se trata de um sistema base para um jogo de cartas inspirados em jogos classicos como (gwent, yu-gi-oh, pokemon tcg).

## Diagrama UML
<img width="1082" height="237" alt="Captura de tela de 2026-05-28 14-13-14" src="https://github.com/user-attachments/assets/f5fd1a48-c8c5-4600-a241-f44551a362fe" />


A relação entre o tabuleiro (GameBoard) e as cartas (Card) é uma Agregação.
As cartas existem independentemente do "todo" (o tabuleiro). As cartas geralmente são criadas em um deck ou na mão do jogador antes de irem para o tabuleiro. Se o tabuleiro for destruído (por exemplo, se a rodada acabar), as cartas em si não devem ser deletadas da memória do sistema se ainda pertencerem ao deck do jogador.

A relação entre o jogador (Player) e o seu tabuleiro (GameBoard) é uma Composição.
Nesse caso o ciclo de vida do tabuleiro está atrelado ao do jogador. Se o jogador sumir não tem sentido existir um tabuleiro.

A relação entre a partida (Match) e o jogador (Player) é uma Dependência (Uso).
A partida não possui o jogador como um atributo fixo de estrutura. Ela apenas interage com ele de forma pontual, recebendo-o como referência em seus métodos para extrair informações do estado atual do jogo (como pontos de vida e nickname).

## Uso de Smart Pointers

* **`Player` usa `unique_ptr` para o `GameBoard`:** O jogador é o único dono do seu tabuleiro. Se o jogador for destruído, o tabuleiro é destruído automaticamente com ele.
* **`GameBoard` usa `shared_ptr` para as `Card`s:** O tabuleiro apenas "pega emprestado" as cartas. Se o tabuleiro for destruído, as cartas continuam vivas na memória.
* **`main` usa `unique_ptr` para o `Player`:** O sistema do jogo (`main`) é o único dono do jogador.
* **`main` usa `shared_ptr` para as `Card`s:** As cartas nascem independentes no sistema do jogo e depois têm seu acesso compartilhado com o tabuleiro.


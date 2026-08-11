cmake_minimum_required(VERSION 3.20)

# Define o nome do projeto
project(Cartas CXX)

# Exporte compile_commands.json automaticamente
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Q1 usa concepts e ranges (C++20) — padrao obrigatorio a partir do TP3
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Necessario para std::thread / std::async (Q3-C/D)
find_package(Threads REQUIRED)

# ============================================================
# Q4 (A) — nlohmann/json via FetchContent
# ============================================================
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(json)

# Define os arquivos de origem
set(SOURCES
    src/main.cpp
)

# Adiciona o executável
add_executable(Cartas ${SOURCES})

# Inclui o diretório de headers (ajuste para src/ ou include/ conforme seu uso)
target_include_directories(Cartas PRIVATE src)

target_link_libraries(Cartas PRIVATE nlohmann_json::nlohmann_json Threads::Threads)

# Aplica as flags de aviso (-Wall -Wextra) em todas as configurações
target_compile_options(Cartas PRIVATE -Wall -Wextra)

# Configura Sanitizers para o modo Debug
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR NOT CMAKE_BUILD_TYPE)
    target_compile_options(Cartas PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(Cartas PRIVATE -fsanitize=address,undefined)
endif()

# ============================================================
# Q5 (extra) — Catch2 v3 via FetchContent
# ============================================================
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(testes tests/test_tp3.cpp)
target_include_directories(testes PRIVATE src)
target_link_libraries(testes PRIVATE Catch2::Catch2WithMain nlohmann_json::nlohmann_json Threads::Threads)

include(CTest)
include(Catch)
catch_discover_tests(testes)

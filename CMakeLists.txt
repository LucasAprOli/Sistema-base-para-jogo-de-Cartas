cmake_minimum_required(VERSION 3.20)

# Define o nome do projeto
project(Cartas CXX)

# Exporte compile_commands.json automaticamente
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configura o padrão C++17 como obrigatório
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define os arquivos de origem 
set(SOURCES
    src/main.cpp
)

# Adiciona o executável
add_executable(Cartas ${SOURCES})

# Inclui o diretório de headers (ajuste para src/ ou include/ conforme seu uso)
target_include_directories(Cartas PRIVATE src)

# Aplica as flags de aviso (-Wall -Wextra) em todas as configurações
target_compile_options(Cartas PRIVATE -Wall -Wextra)

# Configura Sanitizers para o modo Debug
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR NOT CMAKE_BUILD_TYPE)
    target_compile_options(Cartas PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(Cartas PRIVATE -fsanitize=address,undefined)
endif()

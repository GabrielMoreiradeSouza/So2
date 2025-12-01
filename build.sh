#!/bin/bash

# Build script para Linux/WSL

echo "============================================"
echo "  SO-G - Sistema Operacional Simulado"
echo "  Script de Build para Linux/WSL"
echo "============================================"
echo ""

# Verifica se gcc existe
if ! command -v gcc &> /dev/null; then
    echo "ERRO: GCC não encontrado!"
    echo ""
    echo "Instale com:"
    echo "  sudo apt update"
    echo "  sudo apt install build-essential"
    echo ""
    exit 1
fi

# Usa o Makefile
if [ -f Makefile ]; then
    echo "Usando Makefile..."
    make
else
    echo "Makefile não encontrado!"
    exit 1
fi

echo ""
echo "Build concluído!"
echo ""
echo "Próximos passos:"
echo "  make format  - Formata o disco virtual"
echo "  make run     - Inicia o sistema"
echo ""

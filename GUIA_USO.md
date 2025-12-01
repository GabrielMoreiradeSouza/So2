# Guia de Uso do SO-G

## 🚀 INÍCIO RÁPIDO

### Passo 1: Compilar (WSL/Linux)

```bash
make
```

### Passo 2: Formatar o Disco

```bash
make format
```

### Passo 3: Executar

```bash
make run
```

### Passo 4: Login

```
Usuário: admin
Senha: senha123
```

## 📋 COMANDOS DO SHELL

Após o login, você estará no shell SO-G. Comandos disponíveis:

```bash
SO-G> ls              # Lista arquivos
SO-G> cat passwd      # Exibe conteúdo de arquivo
SO-G> chat            # Inicia chat IPC
SO-G> formatar        # Reformata o disco
SO-G> help            # Mostra ajuda
SO-G> exit            # Sai do shell
```

## 💬 TESTANDO O CHAT

### Terminal 1:
```bash
./bin/meu_so
# Login: admin / senha123
SO-G> chat
Você> Olá!
```

### Terminal 2:
```bash
./bin/meu_so  
# Login: user / user123
SO-G> chat
Você> Oi, como vai?
```

As mensagens aparecerão em ambos os terminais!

Para sair do chat: digite `/sair` ou pressione `Ctrl+C`

## ⚙️ NOTAS IMPORTANTES

### Windows
- Este projeto requer **POSIX** (fork, shared memory, semaphores)
- **Solução**: Use **WSL** (Windows Subsystem for Linux)
- Instalação do WSL: `wsl --install`

### Linux
- Já funciona nativamente
- Apenas certifique-se de ter GCC instalado

## 🛠️ TROUBLESHOOTING

### Erro ao compilar
```bash
sudo apt update
sudo apt install build-essential
```

### Disco não formatado
```bash
./bin/formatar
```

### Chat não funciona
- Certifique-se de estar no WSL/Linux
- POSIX IPC não funciona no Windows nativo

## 📖 USUÁRIOS PADRÃO

| Usuário | Senha |
|---------|-------|
| admin | senha123 |
| user | user123 |

## 🧪 TESTES AUTOMATIZADOS

```bash
make test
```

Executa:
- Formatação do disco
- Listagem de arquivos
- Leitura do arquivo passwd

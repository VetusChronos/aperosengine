AperosVoxel Engine
==================

[![Licença](https://img.shields.io/badge/license-LGPLv2.1%2B-blue.svg)](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.pt-br.html)

AperosVoxel é um mecanismo de jogo voxel de código aberto gratuito com fácil modificação e criação
de outros jogos.

AperosVoxel é baseado no [Minetest Engine](https://minetest.net):

> Direitos autorais (C) 2010-2024 Perttu Ahola <celeron55@gmail.com>
e colaboradores do Minetest (veja os comentários do arquivo fonte). 
Obrigada aos contribuidores!

Localização
-----------
* [English](../en/)

Índice
------

1. [Documentação adicional](#documentação-adicional)
2. [Controles padrões](#controles-padrões)
3. [Caminhos](#caminhos)
4. [Arquivo de configuração](#arquivo-de-configuração)
5. [Opções de linha de comando](#opções-por-linha-de-comando)
6. [Compilação](#compilação)
7. [Docker](#docker)
8. [Esquema de versão](#esquema-de-versão)


Documentação adicional
----------------------
- GitHub: https://github.com/yunasatoy/aperosengine/
- [Documentação do desenvolvedor](../en/engine/developing/) (inglês)
- [Documentação para modding](../en/modding/) (inglês)
- [docs/](../en/) diretório da documentação (inglês)

Controles padrões
-----------------
Todos os controles podem ser revinculados nas configurações.
Alguns podem ser alterados na caixa de diálogo de configuração principal na guia de configurações.

| Botão                         | Ação                                                           |
|-------------------------------|----------------------------------------------------------------|
| Move mouse                    | Olhar ao redor                                                 |
| W, A, S, D                    | Movimentar-se                                                  |
| Space                         | Saltar/subir                                                   |
| Shift                         | Esgueirar-se/descer                                            |
| Q                             | Dropa vários itens juntos                                      |
| Shift + Q                     | Dropa item único                                               |
| Botão esquerdo do mouse       | Cavar/bater/usar                                               |
| Botão direito do mouse        | Colocar/usar                                                   |
| Shift + botão direito do mouse| Construir (sem usar)                                           |
| E                             | Inventory menu                                                 |
| Scroll do mouse               | Selecionar item                                                |
| 0-9                           | Selecionar item                                                |
| Z                             | Zoom (precisa de privilegio de zoom)                           |
| T                             | Chat                                                           |
| /                             | Comandos                                                       |
| Esc                           | Pausar menu/abortar/sair (pausa apenas o jogo singleplayer)    |
| +                             | Aumentar o alcance de visualização                             |
| -                             | Diminuir o alcance de visualização                             |
| K                             | Ativar/desativar o modo fly (requer privilégio de voo)         |
| J                             | Ativar/desativar fast mode (precisa de privilégio fast)        |
| H                             | Ativar/desativar modo noclip (precisa de privilégio noclip)    |
| Ctrl                          | Aux1 (Mova-se rapidamente no fast mode. Os jogos podem adicionar recursos especiais)  |
| C                             | Altera os modos da câmera                                      |
| V                             | Altera os modos do minimapa                                    |
| Shift + V                     | Alterar a orientação do minimapa                               |
| F1                            | Ocultar/mostrar HUD                                            |
| F12                           | Ocultar/mostrar chat                                           |
| F7                            | Desabitlitar/habilitar fog                                     |
| F4                            | Desabilitar/habilitar atualização da câmera (Mapblocks não são mais atualizados quando desabilitados, desabilitados em versões de lançamento)                                                                                      |
| F3                            | Altera as telas de informações de depuração                    |
| F6                            | Altera as telas de informações do criador de perfil (profiler) |
| F10                           | Mostrar/ocultar console                                        |
| F2                            | Tirar captura de tela

Caminhos
--------
Locais:

* `bin` - Binários compilados
* `share` - Dados distribuídos somente de leitura
* `user` - Dados modificáveis ​​criados pelo usuário

Onde cada local está em cada plataforma:

* Fonte Windows .zip/RUN_IN_PLACE:
    * `bin` = `bin`
    * `share` = `.`
    * `user` = `.`
*Windows instalado:
    * `bin` = `C:\Program Files\AperosEngine\bin (Depende do local de instalação)`
    * `share` = `C:\Program Files\AperosEngine (Depende do local de instalação)`
    * `user` = `%APPDATA%\AperosEngine` ou `%APEROSENGINE_USER_PATH%`
*Linux instalado:
    * `bin` = `/usr/bin`
    * `share` = `/usr/share/aperosengine`
    * `user` = `~/.aperosengine` ou `$APEROSENGINE_USER_PATH`
*macOS:
    * `bin` = `Conteúdo/MacOS`
    * `share` = `Conteúdo/Recursos`
    * `user` = `Contents/User` ou `~/Library/Application Support/aperosengine` ou `$APEROSENGINE_USER_PATH`

Os mundos podem ser encontrados como pastas separadas em: `user/worlds/`

Arquivo de configuração
-----------------------
- Localização padrão:
    `user/aperosvoxel.conf`
- Este arquivo é criado fechando o AperosEngine pela primeira vez.
- Um arquivo diferente pode ser especificado na linha de comando:
    `--config <caminho-para-o-arquivo>`
- Uma compilação executada no local procurará o arquivo de configuração em
    `location_of_exe/../aperosvoxel.conf` e também `location_of_exe/../../aperosvoxel.conf`

Opções por linha de comando
---------------------------
- Use `--help`

Compilando
----------

- [Compilando em GNU/Linux](../en/engine/compiling/Linux.md)
- [Compilando no Windows](../en/engine/compiling/Windows.md)
- [Compilando no MacOS](../en/engine/compiling/MacOS.md)

Docker
------

- [Desenvolvendo aperosengineserver com Docker](../en/engine/developing/Docker.md)
- [Executando um servidor com Docker](../en/engine/Docker-Server.md)

Esquema de versão
-----------------
Aperos Engine usa o esquema `major.minor.patch.stage`.

- O `major` é incrementado quando a versão contém alterações importantes, todas as outras
os números são definidos como 0.
- O `minor` é incrementado quando o lançamento contém novos recursos definidos,
patch está definido como 0.
- O `patch` é incrementado quando o lançamento contém apenas correções de bugs e muitas
características menores/triviais consideradas necessárias.
- O `stage` é incrementado quando indica se o motor foi liberado, 
está em beta, alfa ou em dev
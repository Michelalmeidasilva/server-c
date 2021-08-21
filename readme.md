
# Trabalho prático de redes de computadores

Docente: Leonardo Bidese de Pinho
Entrega da primeira versão: 20/08/2021

## Descrição

O projeto consiste em um servidor HTTP 1.0 que é capaz de responder a até dez resquisições HTML simultâneas.

## Membros:

*   Maria Elizabeth Barcena
*   Michel Almeida da Silva
*   Thiago Porto Mendes

## Como executar o servidor e cliente:
### Como servidor:

Para rodar o servidor, você deve primeiramente definir as configurações de ambiente, ou seja qual é o path de onde estão os arquivos e também o numero da porta. Para isso há um arquivo chamado .env, portanto faça uma cópia do '.env.example' e o renomeie para '.env'

- $ cp .env.example .env

Dentro desse arquivo altere as variaveis como desejar, mas lembre-se de seguir o mesmo padrão abaixo:
- SERVER_PATH=/home/fulano/diretorio/ate/onde/voce/quiser
- PORT=8088

Como compilar?
- dependencias: [gcc](https://gcc.gnu.org/)

- gcc -pthread helper.c server-http-1.0.c -o server.http

Como executar com o curl na versão http 1.0? 
- dependencias: [curl](https://curl.se/)

### Como cliente pelo navegador

Pode ser acessado pelo navegador de sua preferência.

### Como cliente pelo curl:

Rodando o comando pelo terminal:
- curl "http://localhost:8088/index.html" --http1.0

Para facilitar foi feito um script rodando as 10 requisições em paralelo, faça o seguinte:
Você deve alterar também a porta e o endereço dentro desse arquivo de script chamado "requests-script"

 - $ chmod +x
 - $ ./requests-script


## Reconhecimentos:

*   Código baseado no repositório público:

[Özgür Hepsa?](https://github.com/ozgurhepsag/Multi-threaded-HTTP-Server)

*   Sobre o desenvolvimento do projeto podemos afirmar:

Este projeto foi desenvolvido integralmente pela equipe, sem ajuda não autorizada de alunos não membros do projeto no processo de codificação.


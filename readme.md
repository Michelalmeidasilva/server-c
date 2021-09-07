
# Trabalho prático de redes de computadores

Docente: Leonardo Bidese de Pinho
Entrega da segunda versão: 03/09/2021

## Descrição

O projeto consiste em um servidor HTTP 1.1 que é capaz de responder a até dez resquisições HTML simultâneas.

## Membros:

*   Maria Elizabeth Barcena
*   Michel Almeida da Silva
*   Thiago Porto Mendes

## Como executar o servidor e cliente:
### Como servidor:

Para rodar o servidor, você deve primeiramente definir as configurações de ambiente, ou seja qual é o path de onde estão os arquivos e também o numero da porta. Para isso há um altere no arquivo de header as variaveis PORT_DEFAULT e PATH. A variavel `path`indica onde estão os arquivos do servidor e a variavel `PORT_DEFAULT` qual a porta padrão do servidor. 

Como compilar?
- dependencias: [gcc](https://gcc.gnu.org/)

- gcc -pthread server-http-1.1.c -o server.http

Como executar com o curl na versão http 1.1? 
- dependencias: [curl](https://curl.se/)


### Como client pelo client.c
Compile o client.c com o gcc e execute. 
- Esse cliente servirá para fazer requisições http.

### Como cliente pelo navegador

Pode ser acessado pelo navegador de sua preferência.

### Como cliente pelo curl:

Rodando o comando pelo terminal:
- curl "http://localhost:8088/index.html" --http1.1

Para facilitar foi feito um script rodando as 10 requisições em paralelo, faça o seguinte:
Você deve alterar também a porta e o endereço dentro desse arquivo de script chamado "requests-script"

 - $ chmod +x
 - $ ./requests-script


## Reconhecimentos:

*   Código baseado no repositório público:

[Özgür Hepsa?](https://github.com/ozgurhepsag/Multi-threaded-HTTP-Server)





* Sobre o desenvolvimento do projeto podemos afirmar:

Este projeto foi desenvolvido integralmente pela equipe, sem ajuda não autorizada de alunos não membros do projeto no processo de codificação.


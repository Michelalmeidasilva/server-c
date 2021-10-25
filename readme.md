
## Descrição

Servidor HTTP 1.1.

## Como executar o servidor :

Para rodar o servidor, você deve primeiramente definir as configurações de ambiente,
para isso, altere no arquivo `server-source/env.h` as variaveis `PORT_DEFAULT` e `PATH` para suas respectivas configurações.

### Como compilar e executar?

Primero instale o compilador gcc:
- dependencias: [gcc](https://gcc.gnu.org/)

Logo após, entre na pasta `server-source` e execute o seguinte comando:

- gcc -pthread server-http-1.1.c -o server.http

 ***após isso será gerado um arquivo server.http de saída, este arquivo é o executavel do servidor*** 

E então execute no terminal com 

- ./server.http


### Como enviar requisições ao servidor?

Pode ser acessado pelo navegador de sua preferência e pode então ser acessado diretamente na barra de pesquisa, por exemplo: `http://localhost:3000/index.html`.


Também é possível através do programa curl, rode pelo terminal através da CLI:

- curl "http://localhost:3000/index.html" --http1.1

## Reconhecimentos:

*   Código baseado no repositório público:

[Özgür Hepsa?](https://github.com/ozgurhepsag/Multi-threaded-HTTP-Server)

* Utilização do código de filas no repositório publico:

[Queue-c](https://github.com/matthewhartstonge/c-queue)

* Sobre o desenvolvimento do projeto podemos afirmar:



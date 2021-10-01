#!/bin/bash

SERVER_ADDRESS=http://localhost:8080/index.html

echo "Fazendo requisições no endereço :  $SERVER_ADDRESS"

curl  $SERVER_ADDRESS --http1.1 & 
curl  $SERVER_ADDRESS --http1.1 & 
curl  $SERVER_ADDRESS --http1.1 &
curl  $SERVER_ADDRESS --http1.1 & 
curl  $SERVER_ADDRESS --http1.1 &
curl  $SERVER_ADDRESS --http1.1 &
curl  $SERVER_ADDRESS --http1.1 &
curl  $SERVER_ADDRESS --http1.1 &
curl  $SERVER_ADDRESS --http1.1 &
curl  $SERVER_ADDRESS --http1.1 &
curl  $SERVER_ADDRESS --http1.1  

wait




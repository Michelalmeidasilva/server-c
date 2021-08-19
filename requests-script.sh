#!/bin/bash

SERVER_ADDRESS=http://localhost:8888/index.html

echo "Fazendo requisições no endereço :  $SERVER_ADDRESS"

curl  $SERVER_ADDRESS --http1.0 & 
curl  $SERVER_ADDRESS --http1.0 & 
curl  $SERVER_ADDRESS --http1.0 &
curl  $SERVER_ADDRESS --http1.0 & 
curl  $SERVER_ADDRESS --http1.0 &
curl  $SERVER_ADDRESS --http1.0 &
curl  $SERVER_ADDRESS --http1.0 &
curl  $SERVER_ADDRESS --http1.0 &
curl  $SERVER_ADDRESS --http1.0 &
curl  $SERVER_ADDRESS --http1.0 &
curl  $SERVER_ADDRESS --http1.0  

wait




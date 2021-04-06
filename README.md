# Networked Rock-Paper-Scissors

Rock-paper-scissors implemented using sockets and TCP handshake. The handshake works as follows:
	The client, after successfully connecting to the server
		1. sends their player_name
		2. calls recv to receive servers player_name
	The server, after successfully accepting a connection
		1. calls recv to receive client player_name
		2. sends their player_name


#To run:
```
gcc -w -o rps rps.c
./rps Alice server [port]
./rps Bob client [addr]:[port]
```
...where "Alice" and "Bob" are the names of each player. The program accepts exactly "rock", "paper" or "scissors" as valid moves.

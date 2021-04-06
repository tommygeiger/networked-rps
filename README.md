# Networked Rock-Paper-Scissors

Rock-paper-scissors implemented using sockets over TCP

## To run:
```
git clone https://github.com/tommygeiger/networked-rps
cd networked-rps
gcc -w -o rps rps.c
./rps Alice server [port]
./rps Bob client [addr]:[port]
```
...where "Alice" and "Bob" are the names of each player. The program accepts exactly "rock", "paper" or "scissors" as valid moves.

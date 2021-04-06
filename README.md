# Networked Rock-Paper-Scissors

Text based rock-paper-scissors over TCP using sockets in C.

## To compile & run:
```
git clone https://github.com/tommygeiger/networked-rps
cd networked-rps
gcc -w -o rps rps.c
./rps Alice server [port]
./rps Bob client [addr]:[port]
```
...where "Alice" and "Bob" are the names of each player. The program accepts exactly "rock", "paper" or "scissors" as valid moves. Use `localhost` as addr to test locally.

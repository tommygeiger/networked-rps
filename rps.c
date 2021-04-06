#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_PENDING  1
#define MAX_LINE 	 256

int server(int SERVER_PORT, char *player);
int client(char *host, int SERVER_PORT, char *player);
int move_stoi(char *move);



int main (int argc, char *argv[]) {
	
	//Usage
	if (argc != 4) {
		fprintf(stderr, "Usage: %s player server/client port/addr:port\n", argv[0]);
		exit(1);
	}

	//Call server or client
	if (!strcmp(argv[2], "server")) {
		server(atoi(argv[3]), argv[1]);
	}
	else if (!strcmp(argv[2], "client")) {
		//Parse addr:port
		char *host = strtok(argv[3], ":");
		int port = atoi(strtok(NULL, ":"));
		client(host, port, argv[1]);
	}
	
	//Usage
	fprintf(stderr, "Invalid argument: %s. Use \"server\" or \"client\".\n", argv[2]);
	exit(1);
}



int server(int SERVER_PORT, char *player) {
	
	struct sockaddr_in sin;
	char buf[MAX_LINE], move[MAX_LINE], opponent[MAX_LINE];
	int addr_len, s, new_s, p_idx, o_idx;
	
	//Indexable table of outcomes, x=player y=opponent
	int table[3][3] = {{0, -1, 1},
						{1, 0, -1},
						{-1, 1, 0}};
	
	//Build address data structure
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(SERVER_PORT);
	
	//Setup passive open
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("server: socket");
		exit(1);
	}
	if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
		perror("server: bind");
		exit(1);
	}
	
	//Greet player and await connection
	printf("Welcome %s. Awaiting opponent connection...\n", player);
	listen(s, MAX_PENDING);
	while(1) {
		
		//Accept connection
		if ((new_s = accept(s, (struct sockaddr *)&sin, &addr_len)) < 0) {
			perror("server: accept");
			exit(1);
		}
		
		//Server handshake
		recv(new_s, opponent, sizeof(opponent), 0);
		player[MAX_LINE-1] = '\0';
		send(new_s, player, strlen(player)+1, 0);
		
		//Greet player and prompt move
		printf("Connected to opponent %s.\nEnter your move: ", opponent);
		
		//Move input & validation
		while (1) {
			fgets(move, sizeof(move), stdin);
			if (move_stoi(move) != -1){
				break;
			}
			printf("Invalid move. Enter \"rock\", \"paper\" or \"scissors\": ");
		}
		
		//Ready
		recv(new_s, buf, sizeof(buf), 0);
		send(new_s, "ready", strlen("ready"), 0);
		
		//Exchange move
		recv(new_s, buf, sizeof(buf), 0);
		move[MAX_LINE-1] = '\0';
		send(new_s, move, strlen(move)+1, 0);
		
		//Convert moves to ints
		p_idx = move_stoi(move);
		o_idx = move_stoi(buf);
		
		//Countdown
		printf("3...\n");
		sleep(1);
		printf("2...\n");
		sleep(1);
		printf("1...\n");
		sleep(1);
		
		//Print moves
		strtok(move, "\n");
		strtok(buf, "\n");
		printf("%s (you) chose %s.\n%s chose %s.\n", player, move, opponent, buf);
		
		//Print outcome
		switch (table[p_idx][o_idx]) {
			case 0:
				printf("It's a tie!\n");
				break;
			case 1:
				printf("%s (you) wins!\n", player);
				break;
			case -1:
				printf("%s wins!\n", opponent);
				break;
			default:
				perror("server: invalid outcome");
				break;
		}
		
		//Close sockets
		shutdown(s, SHUT_RDWR);
		close(s);
		shutdown(new_s, SHUT_RDWR);
		close(new_s);
		exit(0);
	}
}



int client(char *host, int SERVER_PORT, char *player) {
	
	struct hostent *hp;
	struct sockaddr_in sin;
	char buf[MAX_LINE], move[MAX_LINE], opponent[MAX_LINE];
	int s, p_idx, o_idx;
	
	//Indexable table of outcomes, x=player y=opponent
	int table[3][3] = {{0, -1, 1},
						{1, 0, -1},
						{-1, 1, 0}};
	
	//Translate host name into peer's IP address
	hp = gethostbyname(host);
	if (!hp) {
		fprintf(stderr, "client: unknown host: %s\n", host);
		exit(1);
	}
	
	//Build address data structure
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
	sin.sin_port = htons(SERVER_PORT);
	
	//Active open
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("client: socket");
		exit(1);
	}
	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		perror("client: connect");
		shutdown(s, SHUT_RDWR);
		close(s);
		exit(1);
	}
	
	//Client handshake
	player[MAX_LINE-1] = '\0';
	send(s, player, strlen(player)+1, 0);
	recv(s, opponent, sizeof(opponent), 0);
	
	//Greet player and prompt move
	printf("Welcome %s.\nConnected to opponent %s.\nEnter your move: ", player, opponent);
	
	//Move input & validation
	while (1) {
		fgets(move, sizeof(move), stdin);
		if (move_stoi(move) != -1){
			break;
		}
		printf("Invalid move. Enter \"rock\", \"paper\" or \"scissors\": ");
	}
	
	//Ready
	send(s, "ready", strlen("ready"), 0);
	recv(s, buf, sizeof(buf), 0);
	
	//Exchange move
	move[MAX_LINE-1] = '\0';
	send(s, move, strlen(move)+1, 0);
	recv(s, buf, sizeof(buf), 0);
	
	//Convert moves to ints
	p_idx = move_stoi(move);
	o_idx = move_stoi(buf);
	
	//Countdown
	printf("3...\n");
	sleep(1);
	printf("2...\n");
	sleep(1);
	printf("1...\n");
	sleep(1);
	
	//Print moves
	strtok(move, "\n");
	strtok(buf, "\n");
	printf("%s (you) chose %s.\n%s chose %s.\n", player, move, opponent, buf);
	
	//Print outcome
	switch (table[p_idx][o_idx]) {
		case 0:
			printf("It's a tie!\n");
			break;
		case 1:
			printf("%s (you) wins!\n", player);
			break;
		case -1:
			printf("%s wins!\n", opponent);
			break;
		default:
			perror("client: invalid outcome");
			break;
	}
	
	//Close socket
	shutdown(s, SHUT_RDWR);
	close(s);
	exit(0);
}



int move_stoi(char *move) {
	
	if (strstr(move, "rock") != NULL)
		return 0;
	if (strstr(move, "paper") != NULL)
		return 1;
	if (strstr(move, "scissors") != NULL)
		return 2;
	return -1; //Error
}

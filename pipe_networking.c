#include "pipe_networking.h"

/*=========================
  server_setup
  args:

  creates the WKP (upstream) and opens it, waiting for a
  connection.

  removes the WKP once a connection has been made

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_setup() {
    mkfifo("known", 0644);
    
    int from_client = open("known", O_RDONLY);
    remove("known");

  return from_client;
}

/*=========================
  server_connect
  args: int from_client

  handles the subserver portion of the 3 way handshake

  returns the file descriptor for the downstream pipe.
  =========================*/
int server_connect(int from_client) {
    int to_client;
    server_handshake(&to_client, from_client);
    return to_client;
}

/*=========================
  server_handshake
  args: int * to_client

  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client, int from_client) {
    char secret_name[100];
    read(from_client, secret_name, sizeof(secret_name));
    printf("Received secret name: %s\n", secret_name);
    
    *to_client = open(secret_name, O_WRONLY);
    write(*to_client, secret_name, sizeof(secret_name));
    
    char response[100];
    read(from_client, response, sizeof(response));
    printf("%s\n", response);
    
    return from_client;
}

/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
    int pid = getpid();
    char secret_name[100];
    sprintf(secret_name, "%d", pid);
    mkfifo(secret_name, 0644);
    
    *to_server = open("known", O_WRONLY);
    write(*to_server, secret_name, sizeof(secret_name));
    
    int from_server = open(secret_name, O_RDONLY);
    char acknowledgement[100];
    read(from_server, acknowledgement, sizeof(acknowledgement));
    if (strcmp(acknowledgement, secret_name) == 0) {
        printf("Received acknowledgement from server!\n");
        remove(secret_name);
        char * response = "Success";
        write(*to_server, response, sizeof(response));
    }
    
    return from_server;
}

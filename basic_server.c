#include "pipe_networking.h"
#include <ctype.h>

void processing(int to_client, int from_client) {
    char msg[100];
    int n = read(from_client, msg, sizeof(msg));
    if (n) {
        int i = 0;
        while (msg[i]) {
            msg[i] = toupper(msg[i]);
            i++;
        }

        write(to_client, msg, sizeof(msg));
        processing(to_client, from_client);
    }
}

void forking(int from_client) {
    int pid = fork();
    if (pid > 0) {
        forking(server_setup());
    } else {
        int to_client = server_connect(from_client);
        processing(to_client, from_client);
    }
}

int main() {
    int from_client = server_setup();
    forking(from_client);
}

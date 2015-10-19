#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int listener_fd = -1;

int init_listener(int port);
int process_client(int client_fd);

int main(int argc, char** argv) {
  //prepare input arguments:
  if (argc != 2) {
    printf("Usage:\n\t%s PORT\n", argv[0]);
    exit(1);
  }
  int port = atoi(argv[1]);

  if (init_listener(port) < 0) {
    puts("Error, init listener.");
    exit(1);
  }

  while (1) {
    struct sockaddr_in client_addr;
    int csize = sizeof(struct sockaddr_in);
    int client_fd = accept(listener_fd, (struct sockaddr*)&client_addr, &csize);
    if (client_fd < 0) {
      puts("Error, unable to accept new client.");
      exit(1);
    }
    process_client(client_fd);
  }


  return 0;
}

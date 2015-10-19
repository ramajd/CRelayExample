#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


#include "defs.h"

int sender_fd = -1;

int init_sender(const char* ip, int port);
int do_login_process();
int do_main_process();
int send_kbd_to_server(const char* cmd,
                       const char* success_response,
                       char* result);

int main(int argc, char** argv) {

  //prepare input arguments:
  if (argc != 3) {
    printf("Usage:\n\t%s IP_ADDRESS PORT\n", argv[0]);
    exit(1);
  }
  char ip[100];
  strcpy(ip, argv[1]);
  int port = atoi(argv[2]);

  sender_fd = init_sender(ip, port);
  if (sender_fd < 0) {
    exit(1);
  }

  if (do_login_process() < 0) {
    exit(1);
  }

  do_main_process();

  return 0;
}

int init_sender(const char* ip, int port) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    puts("Error Creating Socket.");
    return -1;
  }
  puts("socket created...");

  struct sockaddr_in srv_addr;
  struct hostent *server = gethostbyname(ip);
  if (server == NULL) {
    puts("Error, Invalid host.");
    return -1;
  }
  puts("host extracted");
  srv_addr.sin_addr.s_addr = inet_addr(ip);
  // bzero((char*)&srv_addr, sizeof(srv_addr));
  srv_addr.sin_family = AF_INET;
  // bcopy((char*)&server->h_addr, (char*)&srv_addr.sin_addr.s_addr, server->h_length);
  srv_addr.sin_port = htons(port);

  if (connect(fd, (struct sockaddr*)&srv_addr, sizeof(srv_addr)) < 0) {
    printf("Error connecting to %s:%d\n", ip, port);
    exit(1);
  }
  printf("successfully connected to %s:%d\n", ip, port);
  return fd;
}

int do_login_process() {

  if (send_kbd_to_server("USER", "USER:OK\n", NULL) < 0) {
    return -1;
  }

  if (send_kbd_to_server("PASS", "LOGIN:OK", NULL) < 0) {
    return -1;
  }
  return 1;
}

int do_main_process() {

}

int send_kbd_to_server(const char* cmd,
                       const char* success_response,
                       char* result) {
  char wbuff[BUFFER_SIZE];
  memset(wbuff, 0x00, BUFFER_SIZE);
  printf("Enter [%s]: ", cmd);
  fgets(wbuff, BUFFER_SIZE, stdin);
  int wres = send(sender_fd, wbuff, strlen(wbuff) - 1, 0);
  if (wres < 0) {
    puts("Error, write to socket.");
    return -1;
  }
  char rbuff[BUFFER_SIZE];
  memset(rbuff, 0x00, BUFFER_SIZE);
  int rres = recv(sender_fd, rbuff, BUFFER_SIZE, 0);
  if (rres < 0) {
    puts("Error, read from socket.");
    return -1;
  }
  if (result != NULL) {
    memcpy(result, rbuff, rres);
  }
  if (strncmp(success_response, rbuff, rres) != 0) {
    printf("Error, Invalid response from server:\n\t%s\n", rbuff);
    return -1;
  }
  return 0;
}

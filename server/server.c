#include "server.h"

int main(int argc, char const *argv[])
{
  if (argc != 2)
  {
    printf("Usage: ./server PORT\n");
    return 0;
  }

  if ((!is_number(argv[1])) || atoi(argv[1]) > 65535 || atoi(argv[1]) < 0)
  {
    printf("\nInvalid port number\n\n");
    exit(-1);
  }

  connect_to_database();
  signal(SIGINT, catch_ctrl_c_and_exit);

  struct sockaddr_in server; /* server's address information */
  struct sockaddr_in client; /* client's address information */
  const int PORT = atoi(argv[1]);
  char buff[BUFF_SIZE] = {0};
  int listen_fd, conn_fd, opt = TRUE, sin_size;
  pthread_t tid;

  // Step 1: Construct a TCP socket to listen connection request
  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  { /* calls socket() */
    perror("\nError: ");
    exit(-1);
  }

  // Step 2: Bind address to socket
  bzero(&server, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);              /* Remember htons() from "Conversions" section? =) */
  server.sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY puts your IP address automatically */

  // Allow socket descriptor to be reusable
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
  {
    perror("setsockopt() failed");
    close(listen_fd);
    exit(-1);
  }

  if (bind(listen_fd, (struct sockaddr *)&server, sizeof(server)) == -1)
  { /* calls bind() */
    perror("\nError: ");
    close(listen_fd);
    exit(-1);
  }

  printf("Server is running... Listener on port %d \n", PORT);

  // Step 3: Listen request from client
  if (listen(listen_fd, BACKLOG) == -1)
  { /* calls listen() */
    perror("\nError: ");
    close(listen_fd);
    exit(-1);
  }

  sin_size = sizeof(struct sockaddr_in);
  printf("Waiting for connections ...\n");

  while (1)
  {
    // accept request
    sin_size = sizeof(struct sockaddr_in);
    if ((conn_fd = accept(listen_fd, (struct sockaddr *)&client, &sin_size)) == -1){
      perror("\nError: ");
      continue;
    }

    printf("--------------\n");
    printf("New connection from [%s:%d] - (%d)\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), conn_fd);

    // Allocate memory for the new client fd to avoid overwriting in the loop
    int *client_fd = malloc(sizeof(int));
    if (client_fd == NULL) {
      perror("Malloc failed");
      close(conn_fd);
      continue;
    }
    *client_fd = conn_fd;

    add_client(conn_fd);

    // Create a new thread to handle the client
    if (pthread_create(&tid, NULL, thread_start, client_fd) != 0) {
      perror("Error creating thread");
      close(conn_fd);
      free(client_fd);
      continue;
    }
    pthread_detach(tid); // Detach thread to handle cleanup automatically
  }

  close(listen_fd); // close listening socket
  return 0;
}

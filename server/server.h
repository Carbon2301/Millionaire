#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <mysql/mysql.h>

#define BACKLOG 6
#define BUFF_SIZE 1024
#define TRUE 1
#define FALSE 0

enum msg_type
{
  DISCONNECT,
  LOGIN,
  LOGIN_SUCCESS,
  LOGGED_IN,
  WRONG_PASSWORD,
  ACCOUNT_NOT_EXIST,
  ACCOUNT_BLOCKED,
  SIGNUP,
  ACCOUNT_EXIST,
  SIGNUP_SUCCESS,
  CHANGE_PASSWORD,
  SAME_OLD_PASSWORD,
  CHANGE_PASSWORD_SUCCESS,
  PLAY_ALONE,
  PLAY_PVP,
  STOP_GAME,
  QUESTION,
  CHOICE_ANSWER,
  CORRECT_ANSWER,
  FOUND_PLAYER,
  NOT_FOUND_PLAYER,
  ENEMY_CURRENT_DATA,
  WIN,
  WIN_PVP,
  LOSE,
  LOSE_PVP,
  DRAW,
  OVER_TIME,
  LOGOUT,
  FIFTY_FIFTY,
  CALL_PHONE,
  VOTE,
  CHANGE_QUESTION
};

enum login_status
{
  AUTH,
  UN_AUTH
};

typedef struct _message
{
  enum msg_type type;
  char data_type[25];
  int length;
  char value[BUFF_SIZE];
} Message;

typedef struct _client
{
  char login_account[BUFF_SIZE];
  int conn_fd;
  int login_status; // [0: chua login] & [1: da login]
  int room_id;      // 0: offline, 1: in room 1, 2: in room 2, 3: in room 3
  struct _client *next;
} Client;
Client *head_client = NULL;

typedef struct _question
{
  char question[15][BUFF_SIZE];
  char a[15][BUFF_SIZE];
  char b[15][BUFF_SIZE];
  char c[15][BUFF_SIZE];
  char d[15][BUFF_SIZE];
  int answer[15];
  int reward[15];
} Question;

typedef struct _room
{
  int room_id;
  int client_fd[2];              // set default all = 0
  int status;                    // 0: no player enter, 1: waiting, 2: playing, 3: end
  int play_status[2];            // 0: playing, 1: out of room, 2: lose, 3: win, 4: draw
  int index_current_question[2]; // start from 1 to 15
  Question questions[2];
  int seconds[2];                // set default all = 0
  struct _room *next;
} Room;
Room *head_room = NULL;
int current_id_room = 0;

pthread_mutex_t mutex;

MYSQL *conn;

/*------------ Function ----------------*/
int connect_to_database();

int connect_to_database()
{
  char server[50], username[50], password[50], database[50];
  int port;

  FILE *f = fopen("config", "r");
  if (f == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }

  fscanf(f, "DB_HOST=%s\nDB_PORT=%d\nDB_DATABASE=%s\nDB_USERNAME=%s\nDB_PASSWORD=%s", server, &port, database, username, password);
  fclose(f);

  conn = mysql_init(NULL);
  if (!mysql_real_connect(conn, server, username, password, database, port, NULL, 0)) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    exit(1);
  }
  return 1;
}

#endif // SERVER_H

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
#include <openssl/sha.h>

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
  STOP_GAME,
  QUESTION,
  CHOICE_ANSWER,
  CORRECT_ANSWER,
  WIN,
  LOSE,
  OVER_TIME,
  LOGOUT,
  FIFTY_FIFTY,
  CALL_PHONE,
  CHANGE_QUESTION,
  ASK_AUDIENCE,
  HISTORY
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
  int login_status; // [0: not logged in] & [1: logged in]
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
  int id[15];
  int sum_a[15];
  int sum_b[15];
  int sum_c[15];
  int sum_d[15];
} Question;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

MYSQL *conn;

int connect_to_database();
void close_database();
int execute_query(char *query);
Question get_questions();
int fifty_fifty(Question q, int level, int incorrect_answer[2]);
int call_phone(Question q, int level);
int change_question(Question *q, int level, int id);
int ask_audience(const Question *q, int level, int sum[4]);
Client *new_client();
void catch_ctrl_c_and_exit(int sig);
void add_client(int conn_fd);
void delete_client(int conn_fd);
Client *find_client(int conn_fd);
int is_number(const char *s);
void *thread_start(void *client_fd);
void hash_user_password(const char *password, char *hashed_password);
int login(int conn_fd, char msg_data[BUFF_SIZE]);
int signup(char username[BUFF_SIZE], char password[BUFF_SIZE]);
int change_password(char username[BUFF_SIZE], char msg_data[BUFF_SIZE]);
int handle_play_game(Message msg, int conn_fd, Question *questions, int level, int id,char username[BUFF_SIZE]);
int handle_play_alone(int conn_fd, char username[BUFF_SIZE]);

/*---------------- Tính năng -------------------*/
int connect_to_database()
{
    char server[50] = "127.0.0.1";
    int port = 3306;
    char database[50] = "ailatrieuphu";
    char username[50] = "root";
    char password[50] = "TrinhAn04";

  conn = mysql_init(NULL);
  if (!mysql_real_connect(conn, server, username, password, database, port, NULL, 0)) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    exit(1);
  }
  return 1;
}

void close_database() {
    if (conn) {
        mysql_close(conn);
        printf("Database connection closed.\n");
    }
}

int execute_query(char *query)
{
  if (mysql_query(conn, query)) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    exit(1);
  }
  return 1;
}

Question get_questions(){
  MYSQL_RES *res;
  MYSQL_ROW row;

  Question questions;
  char query[1000];

  for (int i = 0; i < 15; i++)
  {
    sprintf(query, "SELECT question, a, b, c, d, answer, reward, id, sum_a, sum_b, sum_c, sum_d FROM questions WHERE level = %d ORDER BY RAND() LIMIT 1", i + 1);
    execute_query(query);
    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    strcpy(questions.question[i], row[0]);
    strcpy(questions.a[i], row[1]);
    strcpy(questions.b[i], row[2]);
    strcpy(questions.c[i], row[3]);
    strcpy(questions.d[i], row[4]);
    questions.answer[i] = atoi(row[5]);
    questions.reward[i] = atoi(row[6]);
    questions.id[i] = atoi(row[7]);
    questions.sum_a[i] = atoi(row[8]);
    questions.sum_b[i] = atoi(row[9]);
    questions.sum_c[i] = atoi(row[10]);
    questions.sum_d[i] = atoi(row[11]);
    mysql_free_result(res);
  }
  return questions;
}

int fifty_fifty(Question q, int level, int answers[2]) {
    srand(time(0));
    int correct_answer = q.answer[level - 1]; 
    int incorrect_answer;
    do {
        incorrect_answer = rand() % 4 + 1;
    } while (incorrect_answer == correct_answer);
    answers[0] = correct_answer;
    answers[1] = incorrect_answer;
    return 1;
}

int call_phone(Question q, int level){
  srand(time(0));
  return q.answer[level - 1];
}

int change_question(Question *q, int level, int id) {
  MYSQL_RES *res;
  MYSQL_ROW row;

  char query[1000];
  sprintf(query, 
          "SELECT question, a, b, c, d, answer, reward, id , sum_a, sum_b, sum_c, sum_d "
          "FROM questions "
          "WHERE level = %d AND id != %d "
          "ORDER BY RAND() LIMIT 1",
          level, id);
  execute_query(query);
  res = mysql_store_result(conn);
  if((row = mysql_fetch_row(res)) != NULL){
    strcpy(q->question[level - 1], row[0]);
    strcpy(q->a[level - 1], row[1]);
    strcpy(q->b[level - 1], row[2]);
    strcpy(q->c[level - 1], row[3]);
    strcpy(q->d[level - 1], row[4]);
    q->answer[level - 1] = atoi(row[5]);
    q->reward[level - 1] = atoi(row[6]);
    q->id[level-1] = atoi(row[7]);
    q->sum_a[level-1] = atoi(row[8]);
    q->sum_b[level-1] = atoi(row[9]);
    q->sum_c[level-1] = atoi(row[10]);
    q->sum_d[level-1] = atoi(row[11]);
    mysql_free_result(res);
  }
  else {
    return 0;
  }
  return 1;
}

int ask_audience(const Question *q, int level, int sum[4]) {
    sum[0] = q->sum_a[level - 1];
    sum[1] = q->sum_b[level - 1];
    sum[2] = q->sum_c[level - 1];
    sum[3] = q->sum_d[level - 1];
    return 1;
}

Client *new_client()
{
  Client *new = (Client *)malloc(sizeof(Client));
  new->login_status = UN_AUTH;
  new->next = NULL;
  return new;
}

void catch_ctrl_c_and_exit(int sig)
{
  char mesg[] = "\nServer is closing...\n";
  while (head_client != NULL)
  {
    if (send(head_client->conn_fd, mesg, strlen(mesg), 0) < 0)
    {
      perror("Send error");
      delete_client(head_client->conn_fd);
    }
    printf("\nClose socketfd: %d\n", head_client->conn_fd);
    delete_client(head_client->conn_fd);
  }
  printf("\nBye\n");
  close_database();
  exit(0);
}

pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

void add_client(int conn_fd)
{

  pthread_mutex_lock(&client_mutex);

  Client *new = new_client();
  new->conn_fd = conn_fd;
  if (head_client == NULL)
    head_client = new; 
  else
  {
    Client *tmp = head_client; 
    while (tmp->next != NULL)
      tmp = tmp->next; 
    tmp->next = new;   
  }

  pthread_mutex_unlock(&client_mutex);
}

void delete_client(int conn_fd)
{

  pthread_mutex_lock(&client_mutex);

  Client *tmp = head_client;
  Client *prev = NULL;
  while (tmp != NULL)
  {
    if (tmp->conn_fd == conn_fd)
    {
      if (prev == NULL)
        head_client = tmp->next;
      else
        prev->next = tmp->next;
      free(tmp);
      break;
    }
    prev = tmp;
    tmp = tmp->next;
  }

  pthread_mutex_unlock(&client_mutex);
}

Client *find_client(int conn_fd)
{
    pthread_mutex_lock(&client_mutex);
    Client *tmp = head_client;
    while (tmp != NULL)
    {
        if (tmp->conn_fd == conn_fd) 
        {
            Client *found = tmp;  
            pthread_mutex_unlock(&client_mutex);
            return found;
        }
        tmp = tmp->next;
    }

    pthread_mutex_unlock(&client_mutex);
    return NULL;  //Trả về NULL nếu không tìm thấy
}

int is_number(const char *s)
{
  while (*s != '\0')
  {
    if (!isdigit((unsigned char)*s))
      return 0; 
    s++;
  }
  return 1; 
}

void hash_user_password(const char *password, char *hashed_password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)password, strlen(password), hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hashed_password + (i * 2), "%02x", hash[i]);
    }
    hashed_password[SHA256_DIGEST_LENGTH * 2] = '\0';
}

int login(int conn_fd, char msg_data[BUFF_SIZE]) {
    MYSQL_RES *res;
    MYSQL_ROW row;

    Client *cli = head_client, *tmp = head_client;
    char username[50], password[50], hashed_password[SHA256_DIGEST_LENGTH * 2 + 1];
    char query[256];
    int re = -1;

    strcpy(username, strtok(msg_data, " "));
    strcpy(password, strtok(NULL, " "));

    while (cli != NULL && cli->conn_fd != conn_fd)
        cli = cli->next;

    // Mã hóa mật khẩu người dùng nhập vào
    hash_user_password(password, hashed_password);

    sprintf(query, "SELECT * FROM account WHERE username = '%s'", username);
    execute_query(query);
    res = mysql_use_result(conn);

    if ((row = mysql_fetch_row(res)) == NULL) {
        re = ACCOUNT_NOT_EXIST; 
    } else {
        if (strcmp(row[2], hashed_password) == 0) {
            if (strcmp(row[3], "1") == 0) {
                while (tmp != NULL) {
                    if (strcmp(tmp->login_account, username) == 0 && tmp->login_status == AUTH) {
                        re = LOGGED_IN;
                        break;
                    }
                    tmp = tmp->next;
                }
                if (re != LOGGED_IN) {
                    strcpy(cli->login_account, username);
                    cli->login_status = AUTH;
                    re = LOGIN_SUCCESS;
                }
            } else {
                re = ACCOUNT_BLOCKED;
            }
        } else {
            re = WRONG_PASSWORD;
        }
    }

    mysql_free_result(res);
    return re;
}

int signup(char username[], char password[])
{
    MYSQL_RES *res;
    MYSQL_ROW row;

    char query[256];
    char hashed_password[SHA256_DIGEST_LENGTH * 2 + 1]; // Chuỗi hash SHA-256
    int re;

    // Mã hóa mật khẩu
    hash_user_password(password, hashed_password);

    pthread_mutex_lock(&mutex);

    sprintf(query, "SELECT * FROM account WHERE username = '%s'", username);
    execute_query(query);
    res = mysql_use_result(conn);
    if ((row = mysql_fetch_row(res)) == NULL)
    {
        mysql_free_result(res);
        sprintf(query, "INSERT INTO account(username, password, status) VALUES('%s', '%s', 1)", username, hashed_password);
        execute_query(query);
        res = mysql_use_result(conn);
        re = SIGNUP_SUCCESS;
    }
    else
        re = ACCOUNT_EXIST;

    mysql_free_result(res);

    pthread_mutex_unlock(&mutex);

    return re;
}


int change_password(char username[], char new_password[]) {
    MYSQL_RES *res;
    MYSQL_ROW row;

    char query[256];
    char hashed_password[SHA256_DIGEST_LENGTH * 2 + 1];
    char current_hashed_password[SHA256_DIGEST_LENGTH * 2 + 1];
    int re;

    hash_user_password(new_password, hashed_password);

    sprintf(query, "SELECT * FROM account WHERE username = '%s'", username);
    execute_query(query);
    res = mysql_use_result(conn);

    if ((row = mysql_fetch_row(res)) != NULL) {
        strncpy(current_hashed_password, row[2], SHA256_DIGEST_LENGTH * 2 + 1);
        if (strcmp(current_hashed_password, hashed_password) == 0) {
            re = SAME_OLD_PASSWORD; 
        } else {
            mysql_free_result(res);
            sprintf(query, "UPDATE account SET password = '%s' WHERE username = '%s'", hashed_password, username);
            execute_query(query);
            res = mysql_use_result(conn);

            re = CHANGE_PASSWORD_SUCCESS;
        }
    } else {
        re = ACCOUNT_NOT_EXIST;
    }

    mysql_free_result(res);
    return re;
}

void update_answer_sum(int id, int answer) {
    char query[500];
    pthread_mutex_lock(&mutex);
    switch (answer) {
        case 1:
            snprintf(query, sizeof(query), "UPDATE questions SET sum_a = sum_a + 1 WHERE id = %d", id);
            break;
        case 2:
            snprintf(query, sizeof(query), "UPDATE questions SET sum_b = sum_b + 1 WHERE id = %d", id);
            break;
        case 3:
            snprintf(query, sizeof(query), "UPDATE questions SET sum_c = sum_c + 1 WHERE id = %d", id);
            break;
        case 4:
            snprintf(query, sizeof(query), "UPDATE questions SET sum_d = sum_d + 1 WHERE id = %d", id);
            break;
        default:
            printf("Lựa chọn không hợp lệ!\n");
            break;
    }

    pthread_mutex_unlock(&mutex);
}

void insert_history(char username[], int level) {
    char query[500];
    pthread_mutex_lock(&mutex);
    snprintf(query, sizeof(query), "INSERT INTO history (username, correct_answers) VALUES ('%s', %d)", username, level - 1);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Lỗi khi chèn vào cơ sở dữ liệu: %s\n", mysql_error(conn));
    }
    pthread_mutex_unlock(&mutex);
}

void get_history_by_username(char user_name[], int conn_fd) {
    char query[500];
    snprintf(query, sizeof(query), 
        "SELECT username, correct_answers, DATE_ADD(play_time, INTERVAL 7 HOUR) AS adjusted_play_time "
        "FROM history WHERE username = '%s'", user_name);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Lỗi khi thực hiện truy vấn: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Lỗi khi lấy kết quả truy vấn: %s\n", mysql_error(conn));
        return;
    }

    int num_fields = mysql_num_fields(result);
    if (num_fields == 0) {
        printf("Không tìm thấy lịch sử cho username: %s\n", user_name);
        mysql_free_result(result);
        return;
    }

    char response[BUFF_SIZE] = "";
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        char row_result[256];
        snprintf(row_result, sizeof(row_result), "Username: %s, Correct Answers: %s, Time: %s\n",
                 row[0] ? row[0] : "NULL",
                 row[1] ? row[1] : "NULL",
                 row[2] ? row[2] : "NULL");

        if (strlen(response) + strlen(row_result) < sizeof(response)) {
            strncat(response, row_result, sizeof(response) - strlen(response) - 1);
        } else {
            printf("Dữ liệu quá lớn, không thể thêm vào.\n");
            break;
        }
    }

    Message msg;
    msg.type = HISTORY;
    strcpy(msg.data_type, "string");
    msg.length = strlen(response);
    strcpy(msg.value, response);

    ssize_t bytes_sent = send(conn_fd, &msg, sizeof(msg), 0);
    if (bytes_sent == -1) {
        perror("Gửi lịch sử thất bại");
    }
    mysql_free_result(result);
}

int handle_play_game(Message msg, int conn_fd, Question *questions, int level, int id, char username[]){
    char str[100];
    int answer;

    switch (msg.type)
    {
    case OVER_TIME:
      msg.type = OVER_TIME;
      send(conn_fd, &msg, sizeof(msg), 0);
      printf("[%d]: Over time\n", conn_fd);
      break;
     case FIFTY_FIFTY:
        printf("[%d]: Client yêu cầu trợ giúp 50/50 cho câu hỏi %d\n", conn_fd, level);
        int answers[2];
        fifty_fifty(*questions, level, answers);
        msg.type = FIFTY_FIFTY;
        snprintf(msg.value, sizeof(msg.value), "%d và %d", answers[0], answers[1]);
        send(conn_fd, &msg, sizeof(msg), 0);
        break;
    case CALL_PHONE:
      printf("[%d]: Client yêu cầu trợ giúp gọi điện thoại cho người thân câu hỏi %d\n", conn_fd, level);
      int phone_answer[1];
      phone_answer[0] = call_phone(*questions, level);  
      msg.type = CALL_PHONE;
      snprintf(msg.value, sizeof(msg.value), "%d", phone_answer[0]); 
      send(conn_fd, &msg, sizeof(msg), 0);
      break;
    case CHANGE_QUESTION:
      printf("[%d]: Client yêu cầu trợ giúp đổi câu hỏi %d\n", conn_fd, level);
      change_question(questions, level, id);
      msg.type = CHANGE_QUESTION;
      send(conn_fd, &msg, sizeof(msg), 0);
      break;
    case ASK_AUDIENCE:
      printf("[%d]: Client yêu cầu trợ giúp hỏi ý kiến khán giả cho câu hỏi %d\n", conn_fd, level);
      int sum[4];
      ask_audience(questions, level, sum);  
      msg.type = ASK_AUDIENCE;
      int sum_answer = sum[0] + sum[1] + sum[2] + sum[3];
      float sum_1 = (float)sum[0] / sum_answer * 100;
      float sum_2 = (float)sum[1] / sum_answer * 100;
      float sum_3 = (float)sum[2] / sum_answer * 100;
      float sum_4 = (float)sum[3] / sum_answer * 100;
      snprintf(msg.value, sizeof(msg.value), 
          "Tỷ lệ chọn phương án 1 là: %.2f%%\n"
          "Tỷ lệ chọn phương án 2 là: %.2f%%\n"
          "Tỷ lệ chọn phương án 3 là: %.2f%%\n"
          "Tỷ lệ chọn phương án 4 là: %.2f%%\n", sum_1, sum_2, sum_3, sum_4);
      send(conn_fd, &msg, sizeof(msg), 0);
      break;
    case CHOICE_ANSWER:
      answer = atoi(strtok(msg.value, "|"));
      if (answer == 0){
        msg.type = STOP_GAME;
        insert_history(username, level);
      if(level <= 1){
        sprintf(str, "Đáp án: %d\nSố tiền thưởng của bạn: 0", questions->answer[level - 1]);
        strcpy(msg.value, str);
        send(conn_fd, &msg, sizeof(msg), 0);
        printf("[%d]: Xin dừng cuộc chơi!\n", conn_fd);
        break;
      }
      else {
        sprintf(str, "Đáp án: %d\nSố tiền thưởng của bạn: %d", questions->answer[level - 1], questions->reward[level - 2]);
        strcpy(msg.value, str);
        send(conn_fd, &msg, sizeof(msg), 0);
        printf("[%d]: Xin dừng cuộc chơi!\n", conn_fd);
        break;
      }
      }
      else if (questions->answer[level - 1] == answer)
      {
        update_answer_sum(id, answer);
        sprintf(str, "Đáp án: %d\nSố tiền thưởng của bạn: %d", questions->answer[level - 1], questions->reward[level - 1]);
        strcpy(msg.value, str);
        if (level == 15)
        {
          msg.type = WIN;
          insert_history(username, level);
          send(conn_fd, &msg, sizeof(msg), 0);
          printf("[%d]: WIN!\n", conn_fd);
        }
        else{
          msg.type = CORRECT_ANSWER;
          send(conn_fd, &msg, sizeof(msg), 0);
          printf("[%d]: Trả lời đúng câu hỏi %d\n", conn_fd, level );
          return 0;
        }
      }
      else
      {
        update_answer_sum(id, answer);
        msg.type = LOSE;
        insert_history(username, level);
        if (level <= 5) {
        sprintf(str, "Đáp án: %d\nSố tiền thưởng của bạn: 0", questions->answer[level - 1]);
        strcpy(msg.value, str);
        send(conn_fd, &msg, sizeof(msg), 0);
        printf("[%d]: LOSE\n", conn_fd);
        break;
        } else if (level <= 10) {
        sprintf(str, "Đáp án: %d\nSố tiền thưởng của bạn: 2000", questions->answer[level - 1]);
        strcpy(msg.value, str);
        send(conn_fd, &msg, sizeof(msg), 0);
        printf("[%d]: LOSE\n", conn_fd);
        break;
        } else {
        sprintf(str, "Đáp án: %d\nSố tiền thưởng của bạn: 22000", questions->answer[level - 1]);
        strcpy(msg.value, str);
        send(conn_fd, &msg, sizeof(msg), 0);
        printf("[%d]: LOSE\n", conn_fd);
        break;
        }
      }
      break;

    default:
      break;
    } 

    return 1;
}

int handle_play_alone(int conn_fd, char username[])
{
  Message msg;
  Question questions = get_questions();
  char str[2048];
  int level = 0;
  int re;
  int id;

  while (level < 15)
  {
initQuestion:
    msg.type = QUESTION;
    sprintf(str, "%d", level + 1);
    strcpy(msg.value, str);
    msg.type = QUESTION;
    snprintf(str, sizeof(str), "Câu %d: %s\n", level + 1, questions.question[level]);
    strcpy(msg.value, str); 

    snprintf(str, sizeof(str), "1. %.500s\n2. %.500s\n3. %.500s\n4. %.500s\n",
         questions.a[level], questions.b[level], questions.c[level], questions.d[level]);
    strcat(msg.value, str); 

    send(conn_fd, &msg, sizeof(msg), 0);
    level++;

recvLabel:
    int recvBytes = recv(conn_fd, &msg, sizeof(msg), 0);

        // Xử lý lỗi hoặc timeout
        if (recvBytes <= 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                printf("[%d]: Timeout: Không có phản hồi từ '%s' cho câu hỏi %d\n", conn_fd, username, level);
            } else if (recvBytes == 0) {
                printf("[%d]: Client đã ngắt kết nối trong khi trả lời câu hỏi %d\n", conn_fd, level);
            } else {
                perror("Recv error");
            }
            close(conn_fd);
            delete_client(conn_fd);
            return -1;
        }

    switch (msg.type)
    {
    case OVER_TIME:
    case STOP_GAME:
      handle_play_game(msg, conn_fd, &questions, level, id, username);
      return 0;
    case CHOICE_ANSWER:
      id = questions.id[level-1];
      re = handle_play_game(msg, conn_fd, &questions, level, id, username);
      if(re == 0) continue;
      return 0;
    case FIFTY_FIFTY:
    case CALL_PHONE:
    case CHANGE_QUESTION:
      id = questions.id[level-1];
      handle_play_game(msg, conn_fd, &questions, level, id, username);
      level--;
      goto initQuestion;
    case ASK_AUDIENCE:
      id = questions.id[level-1];
      handle_play_game(msg, conn_fd, &questions, level, id, username);
      level--;
    default:
      break;
    }
  }
  return 1;
}

void *thread_start(void *client_fd)
{
  pthread_detach(pthread_self());

  int recvBytes, re;
  Message msg;
  int conn_fd = *((int *)client_fd);
    free(client_fd);

    // Thiết lập timeout cho socket
    struct timeval timeout;
    timeout.tv_sec = 20;  // Thời gian chờ 20 giây
    timeout.tv_usec = 0;

    if (setsockopt(conn_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        perror("Error setting socket timeout");
        close(conn_fd);
        pthread_exit(NULL);
    }

    Client *cli = head_client;

  while (cli->conn_fd != conn_fd && cli != NULL)
    cli = cli->next;

  while ((recvBytes = recv(conn_fd, &msg, sizeof(msg), 0)) > 0)
  {
    if (msg.type == DISCONNECT)
    {
      recvBytes = 0;
      break;
    }

    switch (cli->login_status)
    {
    case AUTH:
      switch (msg.type)
      {
    case CHANGE_PASSWORD:
      re = change_password(cli->login_account, msg.value);

      char response_message[256];

      if (re == SAME_OLD_PASSWORD) {
          snprintf(response_message, sizeof(response_message), "Mật khẩu mới trùng với mật khẩu cũ!");
          printf("[%d] Mật khẩu mới trùng với mật khẩu cũ!\n", conn_fd);
      } 
      else if (re == CHANGE_PASSWORD_SUCCESS) {
          snprintf(response_message, sizeof(response_message), "Thay đổi mật khẩu thành công!");
          printf("[%d] Thay đổi mật khẩu thành công!\n", conn_fd);
      }
      strncpy(msg.value, response_message, sizeof(msg.value) - 1);
      msg.type = re;
      send(conn_fd, &msg, sizeof(msg), 0);
      break;

      case PLAY_ALONE:
        printf("[%d]: '%s' đang chơi đơn!\n", conn_fd, cli->login_account);
        handle_play_alone(conn_fd, cli->login_account);
        break;
      case HISTORY:
        printf("[%d]: '%s' yêu cầu xem lịch sử đấu!\n", conn_fd, cli->login_account);
        get_history_by_username(cli->login_account, conn_fd);
        break;
      case LOGOUT:
        printf("[%d]: Goodbye '%s'\n", conn_fd, cli->login_account);
        cli->login_status = UN_AUTH;
        break;
      }
      break;
    case UN_AUTH:
      switch (msg.type)
      {
      case LOGIN:
        re = login(conn_fd, msg.value);
        if (re == LOGIN_SUCCESS)
        {
          msg.type = LOGIN_SUCCESS;
          printf("[%d] Đăng nhập thành công!\n", conn_fd);
          send(conn_fd, &msg, sizeof(msg), 0);
        }
        else if (re == LOGGED_IN)
        {
          msg.type = LOGGED_IN;
          printf("[%d] Tài khoản đã được đăng nhập ở nơi khác!\n", conn_fd);
          send(conn_fd, &msg, sizeof(msg), 0);
        }
        else if (re == ACCOUNT_BLOCKED)
        {
          msg.type = ACCOUNT_BLOCKED;
          printf("[%d] Tài khoản đã bị khóa\n", conn_fd);
          send(conn_fd, &msg, sizeof(msg), 0);
        }
        else if (re == ACCOUNT_NOT_EXIST)
        {
          msg.type = ACCOUNT_NOT_EXIST;
          printf("[%d] Tài khoản không tồn tại\n", conn_fd);
          send(conn_fd, &msg, sizeof(msg), 0);
        }
        else if (re == WRONG_PASSWORD)
        {
          msg.type = WRONG_PASSWORD;
          printf("[%d] Sai mật khẩu\n", conn_fd);
          send(conn_fd, &msg, sizeof(msg), 0);
        }
        break;
      case SIGNUP:
        {
          char username[BUFF_SIZE];
          char password[BUFF_SIZE];
          sprintf(username, "%s", strtok(msg.value, " "));
          sprintf(password, "%s", strtok(NULL, " "));
          
          re = signup(username, password);

          if (re == SIGNUP_SUCCESS)
          {
            msg.type = SIGNUP_SUCCESS;
            printf("[%d]: Đăng ký thành công!\n", conn_fd);
            send(conn_fd, &msg, sizeof(msg), 0);
          }
          else if (re == ACCOUNT_EXIST)
          {
            msg.type = ACCOUNT_EXIST;
            printf("[%d] Tài khoản đã tồn tại\n", conn_fd);
            send(conn_fd, &msg, sizeof(msg), 0);
          }
        }
      }
      break;
    }
  }
   // Kiểm tra lý do thoát khỏi vòng lặp
    if (recvBytes <= 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            printf("[%d]: Timeout: Không có phản hồi nào trong 20 giây\n", conn_fd);
        } else if (recvBytes == 0) {
            printf("[%d]: Client ngắt kết nối\n", conn_fd);
        } else {
            perror("Recv error");
        }
        close(conn_fd);
        delete_client(conn_fd);
    }

  pthread_exit(NULL);
}

#endif 
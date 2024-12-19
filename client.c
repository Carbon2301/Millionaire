#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ctype.h>
#include <time.h>

#define MAX_LINE 1024
#define BUFF_SIZE 1024

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
  HISTORY,
  PLAY_PVP,
  FOUND_PLAYER,
  NOT_FOUND_PLAYER,
  ENEMY_CURRENT_DATA,
  WIN_PVP,
  LOSE_PVP,
  DRAW
};

typedef struct _message
{
  enum msg_type type;
  char data_type[25];
  int length;
  char value[BUFF_SIZE];
} Message;

typedef struct _account
{
  char username[MAX_LINE];
  int login_status; // 0: not login; 1: logged in
} Account;

  int is_number(const char *s);
  int validate_ip(char *ip);
  int menu_not_login();
  int menu_logged();
  int connect_to_server(char ip[], int port);
  int disconnect_to_server();
  int login(char username[], char password[]);
  int signup(char username[], char password[]);
  int logout();
  int change_password(char password[]);
  void receive_history(int sockfd);
  int show_menu_not_login();
  int show_menu_logged();
  int play_alone();
  int play_pvp();

int sockfd;
int recvBytes, sendBytes;
char sendBuff[MAX_LINE] = {0}, recvBuff[MAX_LINE];
struct sockaddr_in server, client;
Account acc;
int fifty_fifty_used = 0;
int call_phone_used = 0;
int change_question_used = 0;
int ask_audience_used = 0;

 int is_number(const char *s)
 {
   while (*s)
   {
     if (!isdigit(*s))
     { 
       return 0;
     }
     s++; 
   }
   return 1;
 }

 int validate_ip(char *ip)
 { 
   int i, num, dots = 0;
   char *ptr;
   if (ip == NULL)
     return 0;
   ptr = strtok(ip, "."); 
   if (ptr == NULL)
     return 0;
   while (ptr)
   {
     if (!is_number(ptr)) 
       return 0;
     num = atoi(ptr); 
     if (num >= 0 && num <= 255)
     {
       ptr = strtok(NULL, "."); 
       if (ptr != NULL)
         dots++; 
     }
     else
       return 1;
   }
   if (dots != 3)
     return 0;
   return 1;
 }

 int menu_not_login()
 {
   char input[MAX_LINE];
   int op;
   do
   {
     printf("\nVui lòng chọn một trong các chức năng sau để tiếp tục:\n");
     printf("\t1. Đăng nhập\n");
     printf("\t2. Đăng ký\n");
     printf("\t3. Trở về\n");
     printf("Lựa chọn của bạn là: ");
     scanf(" %[^\n]", input);
     if (strlen(input) != 1 || !isdigit(input[0]))
       break;
     op = atoi(input);
   } while (op > 3 || op < 1);
   return op;
 }

 int menu_logged()
 {
   char input[MAX_LINE];
   int op;
   do
   {
     printf("\nMenu:\n");
     printf("\t1. Thay đổi mật khẩu.\n");
     printf("\t2. Chơi đơn. Khi vào chơi, lưu ý: \n");
     printf("\t\t'0': Xin dừng cuộc chơi\n");
     printf("\t\t'1 -> 4': Trả lời câu hỏi\n");
     printf("\t\t'5': Trợ giúp 50/50\n");
     printf("\t\t'6': Trợ giúp gọi điện thoại cho người thân\n");
     printf("\t\t'7': Trợ giúp đổi câu hỏi\n");
     printf("\t\t'8': Trợ giúp hỏi ý kiến khán giả\n");
     printf("\t3. Chơi với người khác - tạm thời chưa xử lí\n");
     printf("\t4. Hiển thị lịch sử ván đấu\n");
     printf("\t5. Đăng xuất.\n");
     printf("Lựa chọn của bạn là: ");
     scanf(" %[^\n]", input);
     if (strlen(input) != 1 || !isdigit(input[0]))
       break;
     op = atoi(input);
   } while (op > 5 || op < 1);
   return op;
 }

int connect_to_server(char serverIP[], int serverPort) {
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Tạo socket không thành công");
        return 0;
    }

    printf("Server Address: %s\n", serverIP);

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverPort);

    if (inet_pton(AF_INET, serverIP, &server.sin_addr) <= 0) {
        fprintf(stderr, "Không tồn tại địa chỉ hoặc địa chỉ không được hỗ trợ\n");
        return 0;
    }

    printf("\nĐang kết nối\n");

    // Kết nối đến server
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Kết nối thất bại");
        return 0;
    }

    printf("Đã kết nối!\n");
    return 1;
}

int disconnect_to_server()
{
  printf("Disconnect...\n");
  close(sockfd);
  return 1;
}

int login(char username[], char password[]) {
    Message msg;
    msg.type = LOGIN;
    strcpy(msg.data_type, "string");
    strcpy(msg.value, username);
    strcat(msg.value, " ");
    strcat(msg.value, password);
    msg.length = strlen(msg.value);
    if (send(sockfd, &msg, sizeof(Message), 0) < 0) {
        printf("Gửi dữ liệu không thành công");
    }

    recvBytes = recv(sockfd, &msg, sizeof(Message), 0);
    if (recvBytes == 0) {
        printf("Server đã ngắt kết nối\n");
        close(sockfd);
        exit(0);
    } else if (recvBytes < 0) {
        printf("Nhận dữ liệu không thành công");
    }

    return msg.type;
}

int signup(char username[], char password[]) {
    

    Message msg;
    msg.type = SIGNUP;
    strcpy(msg.data_type, "string");
    strcpy(msg.value, username);
    strcat(msg.value, " ");
    strcat(msg.value, password);
    msg.length = strlen(msg.value);
    if (send(sockfd, &msg, sizeof(Message), 0) < 0) {
        printf("Gửi dữ liệu không thành công");
    }

    recvBytes = recv(sockfd, &msg, sizeof(Message), 0);
    if (recvBytes == 0) {
        printf("Server đã ngắt kết nối\n");
        close(sockfd);
        exit(0);
    } else if (recvBytes < 0) {
        printf("Nhận dữ liệu không thành công");
    }

    return msg.type;
}

int logout(){
  Message msg;
  msg.type = LOGOUT;
  if (send(sockfd, &msg, sizeof(Message), 0) < 0)
  {
    printf("Gửi dữ liệu không thành công");
  }
  return msg.type;
}

void receive_history(int sockfd) {
    Message msg;
    ssize_t bytes_received;

    bytes_received = recv(sockfd, &msg, sizeof(msg), 0);
    if (bytes_received <= 0) {
        perror("Không thể nhận dữ liệu hoặc kết nối đã bị đóng");
        return;
    }
    if (msg.type == HISTORY) {
        printf("Nhận thông tin lịch sử từ server:\n");
        printf("%s\n", msg.value);
    }
}

int change_password(char password[]){
  Message msg;
  msg.type = CHANGE_PASSWORD;
  strcpy(msg.data_type, "string");
  strcat(msg.value, password);
  msg.length = strlen(msg.value);
  if (send(sockfd, &msg, sizeof(Message), 0) < 0)
  {
    printf("Gửi dữ liệu không thành công");
  }

  if (recv(sockfd, &msg, sizeof(Message), 0) < 0)
  {
    printf("Nhận dữ liệu không thành công");
  }

  return msg.type;
}

 int show_menu_not_login()
 {
   Message msg;
   char username[100], password[100];
   int show_menu_not_login = 1;
   while (show_menu_not_login)
   {
     int choice = menu_not_login();

     switch (choice)
     {
     case 1:
       msg.type = LOGIN;
       printf("Enter username: ");
       scanf(" %[^\n]", username);
       printf("Enter password: ");
       scanf(" %[^\n]", password);
       strcpy(msg.data_type, "string");
       strcpy(msg.value, username);
       strcat(msg.value, " ");
       strcat(msg.value, password);
       msg.length = strlen(msg.value);
       if (send(sockfd, &msg, sizeof(msg), 0) < 0)
       {
         printf("Gửi dữ liệu không thành công\n");
       }
       else
       {
         recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
         if (recvBytes < 0)
         {
           printf("Nhận dữ liệu không thành công\n");
         }
         else
         {
           if (msg.type == LOGIN_SUCCESS)
           {
             acc.login_status = 1;
             strcpy(acc.username, username);
             printf("Welcome, %s\n", msg.value);
             show_menu_logged();
           }
           else if (msg.type == ACCOUNT_BLOCKED)
          {
            printf("Tài khoản: '%s' đã bị khóa.\n", msg.value);
          }
           else if(msg.type == LOGGED_IN)
           {
             printf("Tài khoản: '%s' đang được đăng nhập ở nơi khác. Vui lòng thử lại!\n", msg.value);
           }
           else if(msg.type == ACCOUNT_NOT_EXIST)
           {
             printf("Tài khoản không tồn tại!");
           }
           else if(msg.type == WRONG_PASSWORD)
           {
             printf("Mật khẩu sai, vui lòng thử lại!");
           }
         }
       }
       break;
     case 2:

       printf("Username: ");
       scanf(" %[^\n]", username);
       printf("Password: ");
       scanf(" %[^\n]", password);

       if (strlen(username) + 1 + strlen(password) >= sizeof(msg.value)) {
         printf("Tên đăng nhập hoặc mật khẩu quá dài\n");
         break;
       }

       msg.type = SIGNUP;
       strcpy(msg.data_type, "string");

       snprintf(msg.value, sizeof(msg.value), "%s %s", username, password);
       msg.length = strlen(msg.value);

       if (send(sockfd, &msg, sizeof(msg), 0) < 0) {
         perror("Gửi dữ liệu không thành công");
         break;
       }

       recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
       if (recvBytes < 0) {
         perror("Nhận dữ liệu không thành công");
       } else if (recvBytes == 0) {
         printf("Kết nối bị đóng từ server\n");
       } else {
         if (msg.type == ACCOUNT_EXIST) {
           printf("Tài khoản đã tồn tại: %s\n", msg.value);
         } else if (msg.type == SIGNUP_SUCCESS) {
           printf("Đăng ký thành công tài khoản: %s\n", msg.value);
         }
       }
       break;

     case 3:
       printf("Trở về\n");
       msg.type = DISCONNECT;
       send(sockfd, &msg, sizeof(msg), 0);
       show_menu_not_login = 0;
       break;
     default:
       printf("Lựa chọn không hợp lệ\n");
       break;
     }
   }
   return 0;
 }

 int show_menu_logged()
 {
   int show_menu_login = 1;
   Message msg;
   char pass[BUFF_SIZE], re_pass[BUFF_SIZE];

   while (show_menu_login)
   {
     int choice = menu_logged();
     switch (choice)
     {
     case 1:
       while (1)
       {
         printf("Mật khẩu mới: ");
         scanf(" %[^\n]", pass);
         printf("Nhập lại mật khẩu mới: ");
         scanf(" %[^\n]", re_pass);
         if (strcmp(pass, "") == 0 || strcmp(re_pass, "") == 0)
         {
           printf("Mật khẩu không được trùng\n");
           continue;
         }

         if (strcmp(pass, re_pass) == 0)
         {
           msg.type = CHANGE_PASSWORD;
           strcpy(msg.value, pass);
           send(sockfd, &msg, sizeof(msg), 0);
           recv(sockfd, &msg, sizeof(msg), 0);
           printf("%s\n", msg.value);
           break;
         }
         else
         {
           printf("Mật khẩu không khớp\n");
         }
       }
       break;
     case 2:
       msg.type = PLAY_ALONE;
       send(sockfd, &msg, sizeof(msg), 0);
       play_alone();
       break;
     case 3:
       msg.type = PLAY_PVP;
       send(sockfd, &msg, sizeof(msg), 0);
       play_pvp();
       break;
     case 4:
       msg.type = HISTORY;
       send(sockfd, &msg, sizeof(msg), 0);
       receive_history(sockfd);
       break;
     case 5:
       msg.type = LOGOUT;
       send(sockfd, &msg, sizeof(msg), 0);
       printf("Bạn đã đăng xuất\n");
       acc.login_status = 0;
       show_menu_login = 0;
       break;
     default:
       printf("Lựa chọn không hợp lệ\n");
       break;
     }
   }
   return 0;
 }

int play_alone() {
    printf("Bạn chọn chơi đơn. Hãy trả lời các câu hỏi dưới đây để nhận phần thưởng!\n");
    Message msg;
    fifty_fifty_used = 0; // Reset số lần sử dụng 50/50 khi bắt đầu chơi
    call_phone_used = 0; //Reset số lần sử dụng call_phone khi bắt đầu chơi
    change_question_used = 0; //Reset số lần sử dụng change_question khi bắt đầu chơi
    ask_audience_used = 0; //Reset số lần sử dụng ask_audience khi bắt đầu chơi

    while (1) {
        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
        if (recvBytes == 0) {
            printf("Server đã ngắt kết nối\n");
            close(sockfd);
            exit(0);
        } else if (recvBytes < 0) {
            perror("Server đã ngắt kết nối");
            exit(0);
        } else {
            switch (msg.type) {
                case QUESTION:
                    printf("%s\n", msg.value);
                    printf("Nhập lựa chọn của bạn: ");
                    int answer;
                    scanf("%d", &answer);

                    if (answer == 5) {
                        // Kiểm tra xem người chơi đã dùng trợ giúp 50/50 chưa
                        if (fifty_fifty_used > 0) {
                            printf("Bạn đã hết quyền sử dụng 50/50!\n");
                            printf("Nhập lựa chọn của bạn: ");
                            scanf("%d", &answer);

                            if (answer == 0) {
                                msg.type = STOP_GAME;
                                send(sockfd, &msg, sizeof(msg), 0);
                                printf("Bạn đã dừng cuộc chơi!\n");
                                return 1;
                            }

                            // Gửi đáp án cuối cùng sau khi không sử dụng được trợ giúp
                            msg.type = CHOICE_ANSWER;
                            snprintf(msg.value, sizeof(msg.value), "%d", answer);
                            send(sockfd, &msg, sizeof(msg), 0);

                            // Nhận phản hồi từ server
                            recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                            if (msg.type == CORRECT_ANSWER) {
                                printf("Đúng rồi! %s\n", msg.value);
                            } else if (msg.type == WIN) {
                                printf("Bạn đã thắng! %s\n", msg.value);
                                return 1;
                            } else if (msg.type == LOSE) {
                                printf("Bạn đã thua! %s\n", msg.value);
                                return 1;
                            }
                            continue;  // Bỏ qua việc nhận lại câu hỏi
                        }

                        // Nếu chưa dùng trợ giúp, xử lý trợ giúp 50/50
                        msg.type = FIFTY_FIFTY;
                        send(sockfd, &msg, sizeof(msg), 0);
                        fifty_fifty_used++; // Đánh dấu đã sử dụng

                        // Nhận gợi ý 50/50 từ server
                        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                        if (msg.type == FIFTY_FIFTY) {
                            printf("Hai phương án còn lại: %s\n", msg.value);
                            printf("Nhập lựa chọn của bạn: ");
                            scanf("%d", &answer);

                            if (answer == 0) {
                                msg.type = STOP_GAME;
                                send(sockfd, &msg, sizeof(msg), 0);
                                printf("Bạn đã dừng cuộc chơi!\n");
                                return 1;
                            }

                            // Gửi đáp án cuối cùng sau khi nhận trợ giúp 50/50
                            msg.type = CHOICE_ANSWER;
                            snprintf(msg.value, sizeof(msg.value), "%d", answer);
                            send(sockfd, &msg, sizeof(msg), 0);

                            // Chờ phản hồi từ server mà không nhận lại câu hỏi
                            recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                            if (msg.type == CORRECT_ANSWER) {
                                printf("Đúng rồi! %s\n", msg.value);
                            } else if (msg.type == WIN) {
                                printf("Bạn đã thắng! %s\n", msg.value);
                                return 1;
                            } else if (msg.type == LOSE) {
                                printf("Bạn đã thua! %s\n", msg.value);
                                return 1;
                            }
                            continue;  // Bỏ qua việc nhận lại câu hỏi
                        }
                    } else if (answer == 6) {
                        // Kiểm tra xem người chơi đã dùng trợ giúp call ơhone chưa
                        if (call_phone_used > 0) {
                            printf("Bạn đã hết quyền sử dụng gọi điện cho người thân!\n");
                            printf("Nhập lựa chọn của bạn: ");
                            scanf("%d", &answer);

                            if (answer == 0) {
                                msg.type = STOP_GAME;
                                send(sockfd, &msg, sizeof(msg), 0);
                                printf("Bạn đã dừng cuộc chơi!\n");
                                return 1;
                            }

                            // Gửi đáp án cuối cùng sau khi không sử dụng được trợ giúp
                            msg.type = CHOICE_ANSWER;
                            snprintf(msg.value, sizeof(msg.value), "%d", answer);
                            send(sockfd, &msg, sizeof(msg), 0);

                            // Nhận phản hồi từ server
                            recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                            if (msg.type == CORRECT_ANSWER) {
                                printf("Đúng rồi! %s\n", msg.value);
                            } else if (msg.type == WIN) {
                                printf("Bạn đã thắng! %s\n", msg.value);
                                return 1;
                            } else if (msg.type == LOSE) {
                                printf("Bạn đã thua! %s\n", msg.value);
                                return 1;
                            }
                            continue;  // Bỏ qua việc nhận lại câu hỏi
                        }

                        // Nếu chưa dùng trợ giúp, xử lý trợ giúp call phone
                        msg.type = CALL_PHONE;
                        send(sockfd, &msg, sizeof(msg), 0);
                        call_phone_used++; // Đánh dấu đã sử dụng

                        // Nhận gợi ý call phone từ server
                        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                        if (msg.type == CALL_PHONE) {
                            printf("Câu trả lời của họ là: %s\n", msg.value);
                            printf("Nhập lựa chọn của bạn: ");
                            scanf("%d", &answer);

                            if (answer == 0) {
                                msg.type = STOP_GAME;
                                send(sockfd, &msg, sizeof(msg), 0);
                                printf("Bạn đã dừng cuộc chơi!\n");
                                return 1;
                            }

                            // Gửi đáp án cuối cùng sau khi nhận trợ giúp call phone
                            msg.type = CHOICE_ANSWER;
                            snprintf(msg.value, sizeof(msg.value), "%d", answer);
                            send(sockfd, &msg, sizeof(msg), 0);

                            // Chờ phản hồi từ server mà không nhận lại câu hỏi
                            recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                            if (msg.type == CORRECT_ANSWER) {
                                printf("Đúng rồi! %s\n", msg.value);
                            } else if (msg.type == WIN) {
                                printf("Bạn đã thắng! %s\n", msg.value);
                                return 1;
                            } else if (msg.type == LOSE) {
                                printf("Bạn đã thua! %s\n", msg.value);
                                return 1;
                            }
                            continue;  // Bỏ qua việc nhận lại câu hỏi
                        }
                    } else if (answer == 7) {
                    // Kiểm tra xem người chơi đã dùng trợ giúp đổi câu hỏi chưa
                    if (change_question_used > 0) {
                        printf("Bạn đã hết quyền sử dụng đổi câu hỏi!\n");
                        printf("Nhập lựa chọn của bạn: ");
                        scanf("%d", &answer);

                        if (answer == 0) {
                            msg.type = STOP_GAME;
                            send(sockfd, &msg, sizeof(msg), 0);
                            printf("Bạn đã dừng cuộc chơi!\n");
                            return 1;
                        }

                        // Gửi đáp án cuối cùng sau khi không sử dụng được trợ giúp
                        msg.type = CHOICE_ANSWER;
                        snprintf(msg.value, sizeof(msg.value), "%d", answer);
                        send(sockfd, &msg, sizeof(msg), 0);

                        // Nhận phản hồi từ server
                        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                        if (msg.type == CORRECT_ANSWER) {
                            printf("Đúng rồi! %s\n", msg.value);
                        } else if (msg.type == WIN) {
                            printf("Bạn đã thắng! %s\n", msg.value);
                            return 1;
                        } else if (msg.type == LOSE) {
                            printf("Bạn đã thua! %s\n", msg.value);
                            return 1;
                        }
                        continue; // Bỏ qua việc nhận lại câu hỏi
                    }

                        // Nếu chưa dùng trợ giúp, xử lý trợ giúp đổi câu hỏi
                        msg.type = CHANGE_QUESTION;
                        send(sockfd, &msg, sizeof(msg), 0);
                        change_question_used++; // Đánh dấu đã sử dụng

                        // Nhận câu hỏi mới từ server
                        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                        if (recvBytes > 0 && msg.type == QUESTION) {
                            printf("Câu hỏi mới: %s\n", msg.value);
                            printf("Nhập lựa chọn của bạn: ");
                            scanf("%d", &answer);

                            if (answer == 0) {
                                msg.type = STOP_GAME;
                                send(sockfd, &msg, sizeof(msg), 0);
                                printf("Bạn đã dừng cuộc chơi!\n");
                                return 1;
                            }

                            // Gửi đáp án cuối cùng sau khi nhận câu hỏi mới
                            msg.type = CHOICE_ANSWER;
                            snprintf(msg.value, sizeof(msg.value), "%d", answer);
                            send(sockfd, &msg, sizeof(msg), 0);

                            // Nhận phản hồi từ server
                            recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                            if (msg.type == CORRECT_ANSWER) {
                                printf("Đúng rồi! %s\n", msg.value);
                            } else if (msg.type == WIN) {
                                printf("Bạn đã thắng! %s\n", msg.value);
                                return 1;
                            } else if (msg.type == LOSE) {
                                printf("Bạn đã thua! %s\n", msg.value);
                                return 1;
                            }
                        } 
                    } else if (answer == 8) {
                        // Kiểm tra xem người chơi đã dùng trợ giúp call ơhone chưa
                        if (ask_audience_used > 0) {
                            printf("Bạn đã hết quyền sử dụng hỏi ý kiến khán giả!\n");
                            printf("Nhập lựa chọn của bạn: ");
                            scanf("%d", &answer);

                            if (answer == 0) {
                                msg.type = STOP_GAME;
                                send(sockfd, &msg, sizeof(msg), 0);
                                printf("Bạn đã dừng cuộc chơi!\n");
                                return 1;
                            }

                            // Gửi đáp án cuối cùng sau khi không sử dụng được trợ giúp
                            msg.type = CHOICE_ANSWER;
                            snprintf(msg.value, sizeof(msg.value), "%d", answer);
                            send(sockfd, &msg, sizeof(msg), 0);

                            // Nhận phản hồi từ server
                            recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                            if (msg.type == CORRECT_ANSWER) {
                                printf("Đúng rồi! %s\n", msg.value);
                            } else if (msg.type == WIN) {
                                printf("Bạn đã thắng! %s\n", msg.value);
                                return 1;
                            } else if (msg.type == LOSE) {
                                printf("Bạn đã thua! %s\n", msg.value);
                                return 1;
                            }
                            continue;  // Bỏ qua việc nhận lại câu hỏi
                        }

                        // Nếu chưa dùng trợ giúp, xử lý trợ giúp 
                        msg.type = ASK_AUDIENCE;
                        send(sockfd, &msg, sizeof(msg), 0);
                        ask_audience_used++; // Đánh dấu đã sử dụng

                        // Nhận gợi ý 
                        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                        if (msg.type == ASK_AUDIENCE) {
                            printf("Trợ giúp hỏi ý kiến khán giả: \n%s", msg.value);
                            printf("Nhập lựa chọn của bạn: ");
                            scanf("%d", &answer);

                            if (answer == 0) {
                                msg.type = STOP_GAME;
                                send(sockfd, &msg, sizeof(msg), 0);
                                printf("Bạn đã dừng cuộc chơi!\n");
                                return 1;
                            }

                            // Gửi đáp án cuối cùng sau khi nhận trợ giúp hỏi ý kiến khán giả
                            msg.type = CHOICE_ANSWER;
                            snprintf(msg.value, sizeof(msg.value), "%d", answer);
                            send(sockfd, &msg, sizeof(msg), 0);

                            // Chờ phản hồi từ server mà không nhận lại câu hỏi
                            recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                            if (msg.type == CORRECT_ANSWER) {
                                printf("Đúng rồi! %s\n", msg.value);
                            } else if (msg.type == WIN) {
                                printf("Bạn đã thắng! %s\n", msg.value);
                                return 1;
                            } else if (msg.type == LOSE) {
                                printf("Bạn đã thua! %s\n", msg.value);
                                return 1;
                            }
                            continue;  // Bỏ qua việc nhận lại câu hỏi
                        }
                    }
                    else {
                        // Trả lời câu hỏi bình thường
                        msg.type = CHOICE_ANSWER;
                        snprintf(msg.value, sizeof(msg.value), "%d", answer);
                        send(sockfd, &msg, sizeof(msg), 0);
                    }
                    break;

                case STOP_GAME:
                    printf("Bạn đã dừng cuộc chơi! %s\n", msg.value);
                    return 1;

                case CORRECT_ANSWER:
                    printf("Đúng rồi! %s\n", msg.value);
                    break;

                case WIN:
                    printf("Bạn đã thắng! %s\n", msg.value);
                    return 1;

                case LOSE:
                    printf("Bạn đã thua! %s\n", msg.value);
                    return 1;

                default:
                    printf("Nhận được tin nhắn không xác định từ server.\n");
                    break;
            }
        }
    }
}

int play_pvp()
{
  Message msg;
  recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
  if (recvBytes <= 0)
  {
    perror("The server terminated prematurely");
    exit(4);
    return 0;
  }

  printf("%s\n", msg.value);

  recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
  if (recvBytes <= 0)
  {
    perror("The server terminated prematurely");
    exit(4);
    return 0;
  }

  switch (msg.type)
  {
  case FOUND_PLAYER:
    printf("%s\n", msg.value);
    break;
  case NOT_FOUND_PLAYER:
    printf("%s\n", msg.value);
    return 0;
  }

  recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
  if (recvBytes <= 0)
  {
    perror("The server terminated prematurely");
    exit(4);
    return 0;
  }

  printf("Enter room %s\n", msg.value);

  while (1)
  {
    recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
    if (recvBytes < 0)
    {
      perror("The server terminated prematurely");
      exit(0);
      return 0;
    }
    else
    {
      switch (msg.type)
      {
      case QUESTION:
        printf("%s", msg.value);
        printf("Đáp án: ");
        msg.type = CHOICE_ANSWER;
        scanf(" %[^\n]", msg.value);
        send(sockfd, &msg, sizeof(msg), 0);
        break;
      case CORRECT_ANSWER:
        printf("%s\n", msg.value);
        break;
      case WIN:
      case LOSE:
      case DRAW:
        printf("%s\n", msg.value);
        return 1;
      }
    }
  }

  return 1;
}

 int main(int argc, char *argv[])
{
   if(argc != 3)
   {
       printf("Usage: %s <server_ip> <server_port>\n", argv[0]);
       exit(0);
   }
    
    if (connect_to_server(argv[1],atoi(argv[2])) == 0)
    {
      return 0;
    }
    
    show_menu_not_login();
    return 0;
}
#include "client.h"

int choice1, choice2, choice3, sockfd, show1, show2, show3;
int recvBytes, sendBytes;
char sendBuff[MAX_LINE] = {0}, recvBuff[MAX_LINE];
struct sockaddr_in server, client;
Account acc;
int fifty_fifty_used = 0;
int call_phone_used = 0;
int change_question_used = 0;

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
     printf("\nVui lòng ch?n m?t trong các ch?c nang sau d? ti?p t?c:\n");
     printf("\t1. Ðang nh?p\n");
     printf("\t2. Ðang ký\n");
     printf("\t3. Tr? v?\n");
     printf("L?a ch?n c?a b?n là: ");
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
     printf("\t1. Thay d?i m?t kh?u.\n");
     printf("\t2. Choi don. Khi vào choi, luu ý: \n");
     printf("\t\t'0': Xin d?ng cu?c choi\n");
     printf("\t\t'1 -> 4': Tr? l?i câu h?i\n");
     printf("\t\t'5': Tr? giúp 50/50\n");
     printf("\t\t'6': Tr? giúp g?i di?n tho?i cho ngu?i thân\n");
     printf("\t\t'7': Tr? giúp d?i câu h?i\n");
     printf("\t3. Choi v?i ngu?i khác - t?m th?i chua x? lí\n");
     printf("\t4. Ðang xu?t.\n");
     printf("L?a ch?n c?a b?n là: ");
     scanf(" %[^\n]", input);
     if (strlen(input) != 1 || !isdigit(input[0]))
       break;
     op = atoi(input);
   } while (op > 4 || op < 1);
   return op;
 }

int connect_to_server(char serverIP[], int serverPort) {
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("T?o socket không thành công");
        return 0;
    }

    printf("Server Address: %s\n", serverIP);

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverPort);

    if (inet_pton(AF_INET, serverIP, &server.sin_addr) <= 0) {
        fprintf(stderr, "Không t?n t?i d?a ch? ho?c d?a ch? không du?c h? tr?\n");
        return 0;
    }

    printf("\nÐang k?t n?i\n");

    // K?t n?i d?n server
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("K?t n?i th?t b?i");
        return 0;
    }

    printf("Ðã k?t n?i!\n");
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
        printf("G?i d? li?u không thành công");
    }

    recvBytes = recv(sockfd, &msg, sizeof(Message), 0);
    if (recvBytes == 0) {
        printf("Server dã ng?t k?t n?i\n");
        close(sockfd);
        exit(0);
    } else if (recvBytes < 0) {
        printf("Nh?n d? li?u không thành công");
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
        printf("G?i d? li?u không thành công");
    }

    recvBytes = recv(sockfd, &msg, sizeof(Message), 0);
    if (recvBytes == 0) {
        printf("Server dã ng?t k?t n?i\n");
        close(sockfd);
        exit(0);
    } else if (recvBytes < 0) {
        printf("Nh?n d? li?u không thành công");
    }

    return msg.type;
}

int logout(){
  Message msg;
  msg.type = LOGOUT;
  if (send(sockfd, &msg, sizeof(Message), 0) < 0)
  {
    printf("G?i d? li?u không thành công");
  }
  return msg.type;
}

int change_password(char password[]){
  Message msg;
  msg.type = CHANGE_PASSWORD;
  strcpy(msg.data_type, "string");
  strcat(msg.value, password);
  msg.length = strlen(msg.value);
  if (send(sockfd, &msg, sizeof(Message), 0) < 0)
  {
    printf("G?i d? li?u không thành công");
  }

  if (recv(sockfd, &msg, sizeof(Message), 0) < 0)
  {
    printf("Nh?n d? li?u không thành công");
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
         printf("G?i d? li?u không thành công\n");
       }
       else
       {
         recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
         if (recvBytes < 0)
         {
           printf("Nh?n d? li?u không thành công\n");
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
           else if(msg.type == LOGGED_IN)
           {
             printf("Tài kho?n: '%s' dang du?c dang nh?p ? noi khác. Vui lòng th? l?i!\n", msg.value);
           }
           else if(msg.type == ACCOUNT_NOT_EXIST)
           {
             printf("Tài kho?n không t?n t?i!");
           }
           else if(msg.type == WRONG_PASSWORD)
           {
             printf("M?t kh?u sai, vui lòng th? l?i!");
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
         printf("Tên dang nh?p ho?c m?t kh?u quá dài\n");
         break;
       }

       msg.type = SIGNUP;
       strcpy(msg.data_type, "string");

       snprintf(msg.value, sizeof(msg.value), "%s %s", username, password);
       msg.length = strlen(msg.value);

       if (send(sockfd, &msg, sizeof(msg), 0) < 0) {
         perror("G?i d? li?u không thành công");
         break;
       }

       recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
       if (recvBytes < 0) {
         perror("Nh?n d? li?u không thành công");
       } else if (recvBytes == 0) {
         printf("K?t n?i b? dóng t? server\n");
       } else {
         if (msg.type == ACCOUNT_EXIST) {
           printf("Tài kho?n dã t?n t?i: %s\n", msg.value);
         } else if (msg.type == SIGNUP_SUCCESS) {
           printf("Ðang ký thành công tài kho?n: %s\n", msg.value);
         }
       }
       break;

     case 3:
       printf("Tr? v?\n");
       msg.type = DISCONNECT;
       send(sockfd, &msg, sizeof(msg), 0);
       show_menu_not_login = 0;
       break;
     default:
       printf("L?a ch?n không h?p l?\n");
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
         printf("M?t kh?u m?i: ");
         scanf(" %[^\n]", pass);
         printf("Nh?p l?i m?t kh?u m?i: ");
         scanf(" %[^\n]", re_pass);
         if (strcmp(pass, "") == 0 || strcmp(re_pass, "") == 0)
         {
           printf("M?t kh?u không du?c trùng\n");
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
           printf("M?t kh?u không kh?p\n");
         }
       }
       break;
     case 2:
       msg.type = PLAY_ALONE;
       send(sockfd, &msg, sizeof(msg), 0);
       play_alone();
       break;
     case 4:
       msg.type = LOGOUT;
       send(sockfd, &msg, sizeof(msg), 0);
       printf("B?n dã dang xu?t\n");
       acc.login_status = 0;
       show_menu_login = 0;
       break;
     default:
       printf("L?a ch?n không h?p l?\n");
       break;
     }
   }
   return 0;
 }

int play_alone() {
    printf("B?n ch?n choi don. Hãy tr? l?i các câu h?i du?i dây d? nh?n ph?n thu?ng!\n");
    Message msg;
    fifty_fifty_used = 0; // Reset s? l?n s? d?ng 50/50 khi b?t d?u choi
    call_phone_used = 0; //Reset s? l?n s? d?ng call_phone khi b?t d?u choi
    change_question_used = 0; //Reset s? l?n s? d?ng change_question khi b?t d?u choi

    while (1) {
        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
        if (recvBytes == 0) {
            printf("Server dã ng?t k?t n?i\n");
            close(sockfd);
            exit(0);
        } else if (recvBytes < 0) {
            perror("Server dã ng?t k?t n?i");
            exit(0);
        } else {
            switch (msg.type) {
                case QUESTION:
                    printf("%s\n", msg.value);
                    printf("Nh?p l?a ch?n c?a b?n: ");
                    int answer;
                    scanf("%d", &answer);

                    if (answer == 0) {
                        msg.type = STOP_GAME;
                        send(sockfd, &msg, sizeof(msg), 0);
                        printf("B?n dã d?ng cu?c choi!\n");
                        return 1;
                    } else if (answer == 5) {
                        // Ki?m tra xem ngu?i choi dã dùng tr? giúp 50/50 chua
                        if (fifty_fifty_used > 0) {
                            printf("B?n dã h?t quy?n s? d?ng 50/50!\n");
                            printf("Nh?p l?a ch?n c?a b?n: ");
                            scanf("%d", &answer);

                            if (answer == 0) {
                                msg.type = STOP_GAME;
                                send(sockfd, &msg, sizeof(msg), 0);
                                printf("B?n dã d?ng cu?c choi!\n");
                                return 1;
                            }

                            // G?i dáp án cu?i cùng sau khi không s? d?ng du?c tr? giúp
                            msg.type = CHOICE_ANSWER;
                            snprintf(msg.value, sizeof(msg.value), "%d", answer);
                            send(sockfd, &msg, sizeof(msg), 0);

                            // Nh?n ph?n h?i t? server
                            recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                            if (msg.type == CORRECT_ANSWER) {
                                printf("Ðúng r?i! %s\n", msg.value);
                            } else if (msg.type == WIN) {
                                printf("B?n dã th?ng! %s\n", msg.value);
                                return 1;
                            } else if (msg.type == LOSE) {
                                printf("B?n dã thua! %s\n", msg.value);
                                return 1;
                            }
                            continue;  // B? qua vi?c nh?n l?i câu h?i
                        }

                        // N?u chua dùng tr? giúp, x? lý tr? giúp 50/50
                        msg.type = FIFTY_FIFTY;
                        send(sockfd, &msg, sizeof(msg), 0);
                        fifty_fifty_used++; // Ðánh d?u dã s? d?ng

                        // Nh?n g?i ý 50/50 t? server
                        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                        if (msg.type == FIFTY_FIFTY) {
                            printf("Hai phuong án còn l?i: %s\n", msg.value);
                            printf("Nh?p l?a ch?n c?a b?n: ");
                            scanf("%d", &answer);

                            if (answer == 0) {
                                msg.type = STOP_GAME;
                                send(sockfd, &msg, sizeof(msg), 0);
                                printf("B?n dã d?ng cu?c choi!\n");
                                return 1;
                            }

                            // G?i dáp án cu?i cùng sau khi nh?n tr? giúp 50/50
                            msg.type = CHOICE_ANSWER;
                            snprintf(msg.value, sizeof(msg.value), "%d", answer);
                            send(sockfd, &msg, sizeof(msg), 0);

                            // Ch? ph?n h?i t? server mà không nh?n l?i câu h?i
                            recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                            if (msg.type == CORRECT_ANSWER) {
                                printf("Ðúng r?i! %s\n", msg.value);
                            } else if (msg.type == WIN) {
                                printf("B?n dã th?ng! %s\n", msg.value);
                                return 1;
                            } else if (msg.type == LOSE) {
                                printf("B?n dã thua! %s\n", msg.value);
                                return 1;
                            }
                            continue;  // B? qua vi?c nh?n l?i câu h?i
                        }
                    } else if (answer == 6) {
                        // Ki?m tra xem ngu?i choi dã dùng tr? giúp call ohone chua
                        if (call_phone_used > 0) {
                            printf("B?n dã h?t quy?n s? d?ng g?i di?n cho ngu?i thân!\n");
                            printf("Nh?p l?a ch?n c?a b?n: ");
                            scanf("%d", &answer);

                            if (answer == 0) {
                                msg.type = STOP_GAME;
                                send(sockfd, &msg, sizeof(msg), 0);
                                printf("B?n dã d?ng cu?c choi!\n");
                                return 1;
                            }

                            // G?i dáp án cu?i cùng sau khi không s? d?ng du?c tr? giúp
                            msg.type = CHOICE_ANSWER;
                            snprintf(msg.value, sizeof(msg.value), "%d", answer);
                            send(sockfd, &msg, sizeof(msg), 0);

                            // Nh?n ph?n h?i t? server
                            recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                            if (msg.type == CORRECT_ANSWER) {
                                printf("Ðúng r?i! %s\n", msg.value);
                            } else if (msg.type == WIN) {
                                printf("B?n dã th?ng! %s\n", msg.value);
                                return 1;
                            } else if (msg.type == LOSE) {
                                printf("B?n dã thua! %s\n", msg.value);
                                return 1;
                            }
                            continue;  // B? qua vi?c nh?n l?i câu h?i
                        }

                        // N?u chua dùng tr? giúp, x? lý tr? giúp call phone
                        msg.type = CALL_PHONE;
                        send(sockfd, &msg, sizeof(msg), 0);
                        call_phone_used++; // Ðánh d?u dã s? d?ng

                        // Nh?n g?i ý call phone t? server
                        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                        if (msg.type == CALL_PHONE) {
                            printf("Câu tr? l?i c?a h? là: %s\n", msg.value);
                            printf("Nh?p l?a ch?n c?a b?n: ");
                            scanf("%d", &answer);

                            if (answer == 0) {
                                msg.type = STOP_GAME;
                                send(sockfd, &msg, sizeof(msg), 0);
                                printf("B?n dã d?ng cu?c choi!\n");
                                return 1;
                            }

                            // G?i dáp án cu?i cùng sau khi nh?n tr? giúp call phone
                            msg.type = CHOICE_ANSWER;
                            snprintf(msg.value, sizeof(msg.value), "%d", answer);
                            send(sockfd, &msg, sizeof(msg), 0);

                            // Ch? ph?n h?i t? server mà không nh?n l?i câu h?i
                            recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                            if (msg.type == CORRECT_ANSWER) {
                                printf("Ðúng r?i! %s\n", msg.value);
                            } else if (msg.type == WIN) {
                                printf("B?n dã th?ng! %s\n", msg.value);
                                return 1;
                            } else if (msg.type == LOSE) {
                                printf("B?n dã thua! %s\n", msg.value);
                                return 1;
                            }
                            continue;  // B? qua vi?c nh?n l?i câu h?i
                        }
                    } else if (answer == 7) {
                    // Ki?m tra xem ngu?i choi dã dùng tr? giúp d?i câu h?i chua
                    if (change_question_used > 0) {
                        printf("B?n dã h?t quy?n s? d?ng d?i câu h?i!\n");
                        printf("Nh?p l?a ch?n c?a b?n: ");
                        scanf("%d", &answer);

                        if (answer == 0) {
                            msg.type = STOP_GAME;
                            send(sockfd, &msg, sizeof(msg), 0);
                            printf("B?n dã d?ng cu?c choi!\n");
                            return 1;
                        }

                        // G?i dáp án cu?i cùng sau khi không s? d?ng du?c tr? giúp
                        msg.type = CHOICE_ANSWER;
                        snprintf(msg.value, sizeof(msg.value), "%d", answer);
                        send(sockfd, &msg, sizeof(msg), 0);

                        // Nh?n ph?n h?i t? server
                        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                        if (msg.type == CORRECT_ANSWER) {
                            printf("Ðúng r?i! %s\n", msg.value);
                        } else if (msg.type == WIN) {
                            printf("B?n dã th?ng! %s\n", msg.value);
                            return 1;
                        } else if (msg.type == LOSE) {
                            printf("B?n dã thua! %s\n", msg.value);
                            return 1;
                        }
                        continue; // B? qua vi?c nh?n l?i câu h?i
                    }

                        // N?u chua dùng tr? giúp, x? lý tr? giúp d?i câu h?i
                        msg.type = CHANGE_QUESTION;
                        send(sockfd, &msg, sizeof(msg), 0);
                        change_question_used++; // Ðánh d?u dã s? d?ng

                        // Nh?n câu h?i m?i t? server
                        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                        if (recvBytes > 0 && msg.type == QUESTION) {
                            printf("Câu h?i m?i: %s\n", msg.value);
                            printf("Nh?p l?a ch?n c?a b?n: ");
                            scanf("%d", &answer);

                            if (answer == 0) {
                                msg.type = STOP_GAME;
                                send(sockfd, &msg, sizeof(msg), 0);
                                printf("B?n dã d?ng cu?c choi!\n");
                                return 1;
                            }

                            // G?i dáp án cu?i cùng sau khi nh?n câu h?i m?i
                            msg.type = CHOICE_ANSWER;
                            snprintf(msg.value, sizeof(msg.value), "%d", answer);
                            send(sockfd, &msg, sizeof(msg), 0);

                            // Nh?n ph?n h?i t? server
                            recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                            if (msg.type == CORRECT_ANSWER) {
                                printf("Ðúng r?i! %s\n", msg.value);
                            } else if (msg.type == WIN) {
                                printf("B?n dã th?ng! %s\n", msg.value);
                                return 1;
                            } else if (msg.type == LOSE) {
                                printf("B?n dã thua! %s\n", msg.value);
                                return 1;
                            }
                        } 
                    }
                    else {
                        // Tr? l?i câu h?i bình thu?ng
                        msg.type = CHOICE_ANSWER;
                        snprintf(msg.value, sizeof(msg.value), "%d", answer);
                        send(sockfd, &msg, sizeof(msg), 0);
                    }
                    break;

                case STOP_GAME:
                    printf("B?n dã d?ng cu?c choi! %s\n", msg.value);
                    return 1;

                case CORRECT_ANSWER:
                    printf("Ðúng r?i! %s\n", msg.value);
                    break;

                case WIN:
                    printf("B?n dã th?ng! %s\n", msg.value);
                    return 1;

                case LOSE:
                    printf("B?n dã thua! %s\n", msg.value);
                    return 1;

                default:
                    printf("Nh?n du?c tin nh?n không xác d?nh t? server.\n");
                    break;
            }
        }
    }
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

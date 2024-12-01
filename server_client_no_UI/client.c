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
     printf("\nVui l�ng ch?n m?t trong c�c ch?c nang sau d? ti?p t?c:\n");
     printf("\t1. �ang nh?p\n");
     printf("\t2. �ang k�\n");
     printf("\t3. Tr? v?\n");
     printf("L?a ch?n c?a b?n l�: ");
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
     printf("\t2. Choi don. Khi v�o choi, luu �: \n");
     printf("\t\t'0': Xin d?ng cu?c choi\n");
     printf("\t\t'1 -> 4': Tr? l?i c�u h?i\n");
     printf("\t\t'5': Tr? gi�p 50/50\n");
     printf("\t\t'6': Tr? gi�p g?i di?n tho?i cho ngu?i th�n\n");
     printf("\t\t'7': Tr? gi�p d?i c�u h?i\n");
     printf("\t3. Choi v?i ngu?i kh�c - t?m th?i chua x? l�\n");
     printf("\t4. �ang xu?t.\n");
     printf("L?a ch?n c?a b?n l�: ");
     scanf(" %[^\n]", input);
     if (strlen(input) != 1 || !isdigit(input[0]))
       break;
     op = atoi(input);
   } while (op > 4 || op < 1);
   return op;
 }

int connect_to_server(char serverIP[], int serverPort) {
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("T?o socket kh�ng th�nh c�ng");
        return 0;
    }

    printf("Server Address: %s\n", serverIP);

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverPort);

    if (inet_pton(AF_INET, serverIP, &server.sin_addr) <= 0) {
        fprintf(stderr, "Kh�ng t?n t?i d?a ch? ho?c d?a ch? kh�ng du?c h? tr?\n");
        return 0;
    }

    printf("\n�ang k?t n?i\n");

    // K?t n?i d?n server
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("K?t n?i th?t b?i");
        return 0;
    }

    printf("�� k?t n?i!\n");
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
        printf("G?i d? li?u kh�ng th�nh c�ng");
    }

    recvBytes = recv(sockfd, &msg, sizeof(Message), 0);
    if (recvBytes == 0) {
        printf("Server d� ng?t k?t n?i\n");
        close(sockfd);
        exit(0);
    } else if (recvBytes < 0) {
        printf("Nh?n d? li?u kh�ng th�nh c�ng");
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
        printf("G?i d? li?u kh�ng th�nh c�ng");
    }

    recvBytes = recv(sockfd, &msg, sizeof(Message), 0);
    if (recvBytes == 0) {
        printf("Server d� ng?t k?t n?i\n");
        close(sockfd);
        exit(0);
    } else if (recvBytes < 0) {
        printf("Nh?n d? li?u kh�ng th�nh c�ng");
    }

    return msg.type;
}

int logout(){
  Message msg;
  msg.type = LOGOUT;
  if (send(sockfd, &msg, sizeof(Message), 0) < 0)
  {
    printf("G?i d? li?u kh�ng th�nh c�ng");
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
    printf("G?i d? li?u kh�ng th�nh c�ng");
  }

  if (recv(sockfd, &msg, sizeof(Message), 0) < 0)
  {
    printf("Nh?n d? li?u kh�ng th�nh c�ng");
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
         printf("G?i d? li?u kh�ng th�nh c�ng\n");
       }
       else
       {
         recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
         if (recvBytes < 0)
         {
           printf("Nh?n d? li?u kh�ng th�nh c�ng\n");
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
             printf("T�i kho?n: '%s' dang du?c dang nh?p ? noi kh�c. Vui l�ng th? l?i!\n", msg.value);
           }
           else if(msg.type == ACCOUNT_NOT_EXIST)
           {
             printf("T�i kho?n kh�ng t?n t?i!");
           }
           else if(msg.type == WRONG_PASSWORD)
           {
             printf("M?t kh?u sai, vui l�ng th? l?i!");
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
         printf("T�n dang nh?p ho?c m?t kh?u qu� d�i\n");
         break;
       }

       msg.type = SIGNUP;
       strcpy(msg.data_type, "string");

       snprintf(msg.value, sizeof(msg.value), "%s %s", username, password);
       msg.length = strlen(msg.value);

       if (send(sockfd, &msg, sizeof(msg), 0) < 0) {
         perror("G?i d? li?u kh�ng th�nh c�ng");
         break;
       }

       recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
       if (recvBytes < 0) {
         perror("Nh?n d? li?u kh�ng th�nh c�ng");
       } else if (recvBytes == 0) {
         printf("K?t n?i b? d�ng t? server\n");
       } else {
         if (msg.type == ACCOUNT_EXIST) {
           printf("T�i kho?n d� t?n t?i: %s\n", msg.value);
         } else if (msg.type == SIGNUP_SUCCESS) {
           printf("�ang k� th�nh c�ng t�i kho?n: %s\n", msg.value);
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
       printf("L?a ch?n kh�ng h?p l?\n");
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
           printf("M?t kh?u kh�ng du?c tr�ng\n");
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
           printf("M?t kh?u kh�ng kh?p\n");
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
       printf("B?n d� dang xu?t\n");
       acc.login_status = 0;
       show_menu_login = 0;
       break;
     default:
       printf("L?a ch?n kh�ng h?p l?\n");
       break;
     }
   }
   return 0;
 }

int play_alone() {
    printf("B?n ch?n choi don. H�y tr? l?i c�c c�u h?i du?i d�y d? nh?n ph?n thu?ng!\n");
    Message msg;
    fifty_fifty_used = 0; // Reset s? l?n s? d?ng 50/50 khi b?t d?u choi
    call_phone_used = 0; //Reset s? l?n s? d?ng call_phone khi b?t d?u choi
    change_question_used = 0; //Reset s? l?n s? d?ng change_question khi b?t d?u choi

    while (1) {
        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
        if (recvBytes == 0) {
            printf("Server d� ng?t k?t n?i\n");
            close(sockfd);
            exit(0);
        } else if (recvBytes < 0) {
            perror("Server d� ng?t k?t n?i");
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
                        printf("B?n d� d?ng cu?c choi!\n");
                        return 1;
                    } else if (answer == 5) {
                        // Ki?m tra xem ngu?i choi d� d�ng tr? gi�p 50/50 chua
                        if (fifty_fifty_used > 0) {
                            printf("B?n d� h?t quy?n s? d?ng 50/50!\n");
                            printf("Nh?p l?a ch?n c?a b?n: ");
                            scanf("%d", &answer);

                            if (answer == 0) {
                                msg.type = STOP_GAME;
                                send(sockfd, &msg, sizeof(msg), 0);
                                printf("B?n d� d?ng cu?c choi!\n");
                                return 1;
                            }

                            // G?i d�p �n cu?i c�ng sau khi kh�ng s? d?ng du?c tr? gi�p
                            msg.type = CHOICE_ANSWER;
                            snprintf(msg.value, sizeof(msg.value), "%d", answer);
                            send(sockfd, &msg, sizeof(msg), 0);

                            // Nh?n ph?n h?i t? server
                            recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                            if (msg.type == CORRECT_ANSWER) {
                                printf("��ng r?i! %s\n", msg.value);
                            } else if (msg.type == WIN) {
                                printf("B?n d� th?ng! %s\n", msg.value);
                                return 1;
                            } else if (msg.type == LOSE) {
                                printf("B?n d� thua! %s\n", msg.value);
                                return 1;
                            }
                            continue;  // B? qua vi?c nh?n l?i c�u h?i
                        }

                        // N?u chua d�ng tr? gi�p, x? l� tr? gi�p 50/50
                        msg.type = FIFTY_FIFTY;
                        send(sockfd, &msg, sizeof(msg), 0);
                        fifty_fifty_used++; // ��nh d?u d� s? d?ng

                        // Nh?n g?i � 50/50 t? server
                        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                        if (msg.type == FIFTY_FIFTY) {
                            printf("Hai phuong �n c�n l?i: %s\n", msg.value);
                            printf("Nh?p l?a ch?n c?a b?n: ");
                            scanf("%d", &answer);

                            if (answer == 0) {
                                msg.type = STOP_GAME;
                                send(sockfd, &msg, sizeof(msg), 0);
                                printf("B?n d� d?ng cu?c choi!\n");
                                return 1;
                            }

                            // G?i d�p �n cu?i c�ng sau khi nh?n tr? gi�p 50/50
                            msg.type = CHOICE_ANSWER;
                            snprintf(msg.value, sizeof(msg.value), "%d", answer);
                            send(sockfd, &msg, sizeof(msg), 0);

                            // Ch? ph?n h?i t? server m� kh�ng nh?n l?i c�u h?i
                            recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                            if (msg.type == CORRECT_ANSWER) {
                                printf("��ng r?i! %s\n", msg.value);
                            } else if (msg.type == WIN) {
                                printf("B?n d� th?ng! %s\n", msg.value);
                                return 1;
                            } else if (msg.type == LOSE) {
                                printf("B?n d� thua! %s\n", msg.value);
                                return 1;
                            }
                            continue;  // B? qua vi?c nh?n l?i c�u h?i
                        }
                    } else if (answer == 6) {
                        // Ki?m tra xem ngu?i choi d� d�ng tr? gi�p call ohone chua
                        if (call_phone_used > 0) {
                            printf("B?n d� h?t quy?n s? d?ng g?i di?n cho ngu?i th�n!\n");
                            printf("Nh?p l?a ch?n c?a b?n: ");
                            scanf("%d", &answer);

                            if (answer == 0) {
                                msg.type = STOP_GAME;
                                send(sockfd, &msg, sizeof(msg), 0);
                                printf("B?n d� d?ng cu?c choi!\n");
                                return 1;
                            }

                            // G?i d�p �n cu?i c�ng sau khi kh�ng s? d?ng du?c tr? gi�p
                            msg.type = CHOICE_ANSWER;
                            snprintf(msg.value, sizeof(msg.value), "%d", answer);
                            send(sockfd, &msg, sizeof(msg), 0);

                            // Nh?n ph?n h?i t? server
                            recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                            if (msg.type == CORRECT_ANSWER) {
                                printf("��ng r?i! %s\n", msg.value);
                            } else if (msg.type == WIN) {
                                printf("B?n d� th?ng! %s\n", msg.value);
                                return 1;
                            } else if (msg.type == LOSE) {
                                printf("B?n d� thua! %s\n", msg.value);
                                return 1;
                            }
                            continue;  // B? qua vi?c nh?n l?i c�u h?i
                        }

                        // N?u chua d�ng tr? gi�p, x? l� tr? gi�p call phone
                        msg.type = CALL_PHONE;
                        send(sockfd, &msg, sizeof(msg), 0);
                        call_phone_used++; // ��nh d?u d� s? d?ng

                        // Nh?n g?i � call phone t? server
                        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                        if (msg.type == CALL_PHONE) {
                            printf("C�u tr? l?i c?a h? l�: %s\n", msg.value);
                            printf("Nh?p l?a ch?n c?a b?n: ");
                            scanf("%d", &answer);

                            if (answer == 0) {
                                msg.type = STOP_GAME;
                                send(sockfd, &msg, sizeof(msg), 0);
                                printf("B?n d� d?ng cu?c choi!\n");
                                return 1;
                            }

                            // G?i d�p �n cu?i c�ng sau khi nh?n tr? gi�p call phone
                            msg.type = CHOICE_ANSWER;
                            snprintf(msg.value, sizeof(msg.value), "%d", answer);
                            send(sockfd, &msg, sizeof(msg), 0);

                            // Ch? ph?n h?i t? server m� kh�ng nh?n l?i c�u h?i
                            recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                            if (msg.type == CORRECT_ANSWER) {
                                printf("��ng r?i! %s\n", msg.value);
                            } else if (msg.type == WIN) {
                                printf("B?n d� th?ng! %s\n", msg.value);
                                return 1;
                            } else if (msg.type == LOSE) {
                                printf("B?n d� thua! %s\n", msg.value);
                                return 1;
                            }
                            continue;  // B? qua vi?c nh?n l?i c�u h?i
                        }
                    } else if (answer == 7) {
                    // Ki?m tra xem ngu?i choi d� d�ng tr? gi�p d?i c�u h?i chua
                    if (change_question_used > 0) {
                        printf("B?n d� h?t quy?n s? d?ng d?i c�u h?i!\n");
                        printf("Nh?p l?a ch?n c?a b?n: ");
                        scanf("%d", &answer);

                        if (answer == 0) {
                            msg.type = STOP_GAME;
                            send(sockfd, &msg, sizeof(msg), 0);
                            printf("B?n d� d?ng cu?c choi!\n");
                            return 1;
                        }

                        // G?i d�p �n cu?i c�ng sau khi kh�ng s? d?ng du?c tr? gi�p
                        msg.type = CHOICE_ANSWER;
                        snprintf(msg.value, sizeof(msg.value), "%d", answer);
                        send(sockfd, &msg, sizeof(msg), 0);

                        // Nh?n ph?n h?i t? server
                        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                        if (msg.type == CORRECT_ANSWER) {
                            printf("��ng r?i! %s\n", msg.value);
                        } else if (msg.type == WIN) {
                            printf("B?n d� th?ng! %s\n", msg.value);
                            return 1;
                        } else if (msg.type == LOSE) {
                            printf("B?n d� thua! %s\n", msg.value);
                            return 1;
                        }
                        continue; // B? qua vi?c nh?n l?i c�u h?i
                    }

                        // N?u chua d�ng tr? gi�p, x? l� tr? gi�p d?i c�u h?i
                        msg.type = CHANGE_QUESTION;
                        send(sockfd, &msg, sizeof(msg), 0);
                        change_question_used++; // ��nh d?u d� s? d?ng

                        // Nh?n c�u h?i m?i t? server
                        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                        if (recvBytes > 0 && msg.type == QUESTION) {
                            printf("C�u h?i m?i: %s\n", msg.value);
                            printf("Nh?p l?a ch?n c?a b?n: ");
                            scanf("%d", &answer);

                            if (answer == 0) {
                                msg.type = STOP_GAME;
                                send(sockfd, &msg, sizeof(msg), 0);
                                printf("B?n d� d?ng cu?c choi!\n");
                                return 1;
                            }

                            // G?i d�p �n cu?i c�ng sau khi nh?n c�u h?i m?i
                            msg.type = CHOICE_ANSWER;
                            snprintf(msg.value, sizeof(msg.value), "%d", answer);
                            send(sockfd, &msg, sizeof(msg), 0);

                            // Nh?n ph?n h?i t? server
                            recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
                            if (msg.type == CORRECT_ANSWER) {
                                printf("��ng r?i! %s\n", msg.value);
                            } else if (msg.type == WIN) {
                                printf("B?n d� th?ng! %s\n", msg.value);
                                return 1;
                            } else if (msg.type == LOSE) {
                                printf("B?n d� thua! %s\n", msg.value);
                                return 1;
                            }
                        } 
                    }
                    else {
                        // Tr? l?i c�u h?i b�nh thu?ng
                        msg.type = CHOICE_ANSWER;
                        snprintf(msg.value, sizeof(msg.value), "%d", answer);
                        send(sockfd, &msg, sizeof(msg), 0);
                    }
                    break;

                case STOP_GAME:
                    printf("B?n d� d?ng cu?c choi! %s\n", msg.value);
                    return 1;

                case CORRECT_ANSWER:
                    printf("��ng r?i! %s\n", msg.value);
                    break;

                case WIN:
                    printf("B?n d� th?ng! %s\n", msg.value);
                    return 1;

                case LOSE:
                    printf("B?n d� thua! %s\n", msg.value);
                    return 1;

                default:
                    printf("Nh?n du?c tin nh?n kh�ng x�c d?nh t? server.\n");
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

#include "client.h"

int choice1, choice2, choice3, sockfd, show1, show2, show3;
int recvBytes, sendBytes;
char sendBuff[MAX_LINE] = {0}, recvBuff[MAX_LINE];
struct sockaddr_in server, client;
Account acc;


 int is_number(const char *s)
 {
   while (*s)
   {
     if (!isdigit(*s))
     { // if the character is not a number, return false
       return 0;
     }
     s++; // point to next character
   }
   return 1;
 }

 int validate_ip(char *ip)
 { // check whether the IP is valid or not
   int i, num, dots = 0;
   char *ptr;
   if (ip == NULL)
     return 0;
   ptr = strtok(ip, "."); // cut the string using dor delimiter
   if (ptr == NULL)
     return 0;
   while (ptr)
   {
     if (!is_number(ptr)) // check whether the sub string is holding only number or not
       return 0;
     num = atoi(ptr); // convert substring to number
     if (num >= 0 && num <= 255)
     {
       ptr = strtok(NULL, "."); // cut the next part of the string
       if (ptr != NULL)
         dots++; // increase the dot count
     }
     else
       return 1;
   }
   if (dots != 3) // if the number of dots are not 3, return false
     return 0;
   return 1;
 }


 int menu_not_login()
 {
   char input[MAX_LINE];
   int op;
   do
   {
     printf("\n************************************\n");
     printf("\t1. Đăng nhập\n");
     printf("\t2. Đăng ký\n");
     printf("\t3. Trở về\n");
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
     printf("\n************************************\n");
     printf("\t1. Thay đổi mật khẩu.\n");
     printf("\t2. Play Alone\n");
     printf("\t3. Play PVP - tam thoi chua xu li.\n");
     printf("\t4. Đăng xuất.\n");
     scanf(" %[^\n]", input);
     if (strlen(input) != 1 || !isdigit(input[0]))
       break;
     op = atoi(input);
   } while (op > 4 || op < 1);
   return op;
 }

int connect_to_server(char serverIP[], int serverPort) {
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return 0;
    }

    printf("Server Address: %s\n", serverIP);

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverPort);

    if (inet_pton(AF_INET, serverIP, &server.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address or address not supported\n");
        return 0;
    }

    printf("\nConnecting....\n");

    // Kết nối đến server
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        return 0;
    }

    printf("Connected!\n");
    return 1;
}


int disconnect_to_server()
{
  printf("Disconnect...\n");
  close(sockfd);
  return 1;
}

int login(char username[], char password[])
{
  Message msg;
  msg.type = LOGIN;
  strcpy(msg.data_type, "string");
  strcpy(msg.value, username);
  strcat(msg.value, " ");
  strcat(msg.value, password);
  msg.length = strlen(msg.value);
  if (send(sockfd, &msg, sizeof(Message), 0) < 0)
  {
    printf("Send failed");
  }

  if (recv(sockfd, &msg, sizeof(Message), 0) < 0)
  {
    printf("Receive failed");
  }

  return msg.type;
}

int signup(char username[], char password[])
{
  Message msg;
  msg.type = SIGNUP;
  strcpy(msg.data_type, "string");
  strcpy(msg.value, username);
  strcat(msg.value, " ");
  strcat(msg.value, password);
  msg.length = strlen(msg.value);
  if (send(sockfd, &msg, sizeof(Message), 0) < 0)
  {
    printf("Send failed");
  }

  if (recv(sockfd, &msg, sizeof(Message), 0) < 0)
  {
    printf("Receive failed");
  }

  return msg.type;
}

int logout(){
  Message msg;
  msg.type = LOGOUT;
  if (send(sockfd, &msg, sizeof(Message), 0) < 0)
  {
    printf("Send failed");
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
    printf("Send failed");
  }

  if (recv(sockfd, &msg, sizeof(Message), 0) < 0)
  {
    printf("Receive failed");
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
       printf("Username: ");
       scanf(" %[^\n]", username);
       printf("Password: ");
       scanf(" %[^\n]", password);
       strcpy(msg.data_type, "string");
       strcpy(msg.value, username);
       strcat(msg.value, " ");
       strcat(msg.value, password);
       msg.length = strlen(msg.value);
       if (send(sockfd, &msg, sizeof(msg), 0) < 0)
       {
         printf("Send failed\n");
       }
       else
       {
         recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
         if (recvBytes < 0)
         {
           printf("Receive failed\n");
         }
         else
         {
           if (msg.type == LOGIN_SUCCESS)
           {
             acc.login_status = 1;
             strcpy(acc.username, username);
             printf("%s\n", msg.value);
             show_menu_logged();
           }
           else
           {
             printf("%s\n", msg.value);
           }
         }
       }
       break;
     case 2:
       msg.type = SIGNUP;
       printf("Username: ");
       scanf(" %[^\n]", username);
       strcpy(msg.value, username);
       strcpy(msg.data_type, "string");
       if (send(sockfd, &msg, sizeof(msg), 0) < 0)
       {
         printf("Send failed\n");
       }
       else
       {
         recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
         if (recvBytes < 0)
         {
           printf("Receive failed\n");
         }
         else
         {
           if (msg.type == ACCOUNT_EXIST)
           {
             printf("%s!\n", msg.value);
             break;
           }
         }
       }
       printf("Password: ");
       scanf(" %[^\n]", password);
       strcat(msg.value, " ");
       strcat(msg.value, password);
       msg.length = strlen(msg.value);
       if (send(sockfd, &msg, sizeof(msg), 0) < 0)
       {
         printf("Send failed\n");
       }
       else
       {
         recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
         if (recvBytes < 0)
         {
           printf("Receive failed\n");
         }
         else
         {
           if (msg.type == SIGNUP_SUCCESS)
           {
             printf("%s\n", msg.value);
           }
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
     case 4:
       msg.type = LOGOUT;
       send(sockfd, &msg, sizeof(msg), 0);
       printf("Đăng xuất\n");
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

int play_alone()
{
    printf("Chơi một mình\n");
    Message msg;
    while (1)
    {
        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
        if (recvBytes < 0)
        {
            perror("Server đã ngắt kết nối");
            exit(0);
            return 0;
        }
        else
        {
            switch (msg.type)
            {
            case QUESTION:
                // Kiểm tra và in câu hỏi
                printf("Câu hỏi: %s\n", msg.value);
                printf("Đáp án của bạn: ");
                msg.type = CHOICE_ANSWER;
                scanf(" %[^\n]", msg.value);
                send(sockfd, &msg, sizeof(msg), 0);
                break;
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


 int main(int argc, char *argv[])
{
   if(argc != 3)
   {
       printf("Usage: %s <server_ip> <server_port>\n", argv[0]);
       exit(0);
   }
    connect_to_server(argv[1],atoi(argv[2]));
    
    show_menu_not_login();
    return 0;
}


 
 
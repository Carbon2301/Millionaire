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
     printf("\t2. Chơi đơn\n");
     printf("\t3. Chơi với người khác - tạm thời chưa xử lí\n");
     printf("\t4. Đăng xuất.\n");
     printf("Lựa chọn của bạn là: ");
     scanf(" %[^\n]", input);
     if (strlen(input) != 1 || !isdigit(input[0]))
       break;
     op = atoi(input);
   } while (op > 4 || op < 1);
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
           else
           {
             printf("%s\n", msg.value);
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
     case 4:
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
    printf("Bạn chọn chơi đơn. Hãy trả lời các câu hỏi dưới dây để nhận phần thưởng!\n");
    Message msg;
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
                    printf("Bạn đã thua! Đáp án đúng: %s\n", msg.value);
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
    
    if (connect_to_server(argv[1],atoi(argv[2])) == 0)
    {
      return 0;
    }
    
    show_menu_not_login();
    return 0;
}


 
 
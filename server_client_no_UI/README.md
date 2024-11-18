# Milionaire


## Tính năng

- Đăng ký, đăng nhập, đổi mật khẩu
- Chơi đơn
- Quyền trợ giúp


## Chạy chương trình

#### _1. Server_ ####

##### 2.1. Cài đặt MYSQL Server, library Dev MySQL #####
```sh
sudo apt install mysql-server libmysqlclient-dev
```

##### 2.2. Cấp quyền cho MySQL #####

```sh
mysql -u root -p
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'YOUR_PASSWORD';
FLUSH PRIVILEGES;

```

##### 2.4 Tạo database ailatrieuphu

```sh
mysql -u root -p
create database ailatrieuphu;
```

##### 2.5. Config and import database

```sh
mysql -u root -p ailatrieuphu < ailatrieuphu.sql 
```
##### 2.6. Run Server #####

```sh
cd server_client_no_UI
make clean
make all
./server <PORT>
```

#### _3. Client_ ####

```sh
cd server_client_no_UI
./client <IP server> <PORT>
```



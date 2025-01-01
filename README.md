# Milionaire


# Tính năng

- Đăng ký, đăng nhập, đổi mật khẩu
- Chơi đơn
- Quyền trợ giúp
- PVP online

# Chạy chương trình

## 1. Config ####

### 1.1 Cài đặt MYSQL Server, library Dev MySQL #####
```sh
sudo apt install mysql-server libmysqlclient-dev
```

### 1.2 Cấp quyền cho MySQL #####

Thay thế `'YOUR_USERNAME'` và `'YOUR_PASSWORD'` theo ý muốn
```sh
mysql -u 'YOUR_USERNAME' -p
```
```sql
ALTER USER 'YOUR_USERNAME'@'localhost' IDENTIFIED WITH mysql_native_password BY 'YOUR_PASSWORD';
FLUSH PRIVILEGES;
exit
```
Sau khi config cho database thì đổi `YOUR_USERNAME` `YOUR_PASSWORD` trong file `config` theo config ở trên
### 1.3 Tạo database ailatrieuphu

```sh
mysql -u 'YOUR_USERNAME' -p
```
```sql
create database ailatrieuphu;
exit
```

### 1.4 Config and import database

```sh
mysql -u YOUR_USERNAME -p ailatrieuphu < ailatrieuphu.sql 
```
## 2. Run
### 2.1 Make project
```sh
make clean && make all
```
### 2.2 Server

```sh
./server <PORT>
```

### 2.3 Client

```sh
./client <IP server> <PORT>
```


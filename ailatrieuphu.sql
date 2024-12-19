SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";

CREATE DATABASE IF NOT EXISTS `ailatrieuphu` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci;
USE `ailatrieuphu`;

CREATE TABLE `account` (
  `id` int NOT NULL AUTO_INCREMENT,
  `username` varchar(50) NOT NULL,
  `password` varchar(50) NOT NULL,
  `status` tinyint(1) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

INSERT INTO `account` (`id`, `username`, `password`, `status`) VALUES
(1, 'an', 'an', 1),
(2, 'a', 'a', 1),
(3, 'b', 'b', 1),
(4, 'c', 'c', 0);

CREATE TABLE `questions` (
  `id` int NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`id`),
  `level` int NOT NULL,
  `question` varchar(500) CHARACTER SET utf8mb4 COLLATE utf8mb4_vi_0900_as_cs NOT NULL,
  `a` varchar(500) CHARACTER SET utf8mb4 COLLATE utf8mb4_vi_0900_as_cs NOT NULL,
  `b` varchar(500) CHARACTER SET utf8mb4 COLLATE utf8mb4_vi_0900_as_cs NOT NULL,
  `c` varchar(500) CHARACTER SET utf8mb4 COLLATE utf8mb4_vi_0900_as_cs NOT NULL,
  `d` varchar(500) CHARACTER SET utf8mb4 COLLATE utf8mb4_vi_0900_as_cs NOT NULL,
  `answer` int NOT NULL,
  `reward` int NOT NULL,
  `sum_a` int NOT NULL,
  `sum_b` int NOT NULL,
  `sum_c` int NOT NULL,
  `sum_d` int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

CREATE TABLE `history` (
  `id` int NOT NULL AUTO_INCREMENT,
  `username` varchar(50) NOT NULL,
  `correct_answers` int NOT NULL,  -- Số câu trả lời đúng
  `play_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,  -- Thời gian ván đấu khi lưu vào CSDL
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;


INSERT INTO `questions` (`id`, `level`, `question`, `a`, `b`, `c`, `d`, `answer`, `reward`, `sum_a`, `sum_b`, `sum_c`, `sum_d`) VALUES
(1, 1, 'Điền tiếp vào câu tục ngữ sau: \"Chớp đông nhay nháy, gà gáy thì ....\"?', 'Toi', 'Mưa', 'Nắng', 'Râm', 2, 200 , 1 ,2 , 3, 4),
(2, 1, 'Bài hát \"Da nâu\" do ca sĩ nào thể hiện?', 'Ngô Thanh Vân', 'Phi Thanh Vân', 'Nguyễn Thanh Vân', 'Đặng Thanh Vân', 2, 200, 3, 2, 4, 1),
(3, 1, 'Trong các thiết bị sau, thiết bị nào thường dùng để tạo ra gió?', 'Điện thoại', 'Máy tính', 'Quạt', 'Đồng hồ', 3, 200, 6, 5, 3, 1),
(4, 2, 'Trong các nhân vật sau đây, nhân vật nào tốt bụng nhất?', 'Chaien', 'Chaikô', 'Mèo máy', 'Nobita', 3, 400, 1, 1, 2, 3),
(5, 2, 'Ai là người tiêu diệt Xên Bọ Hung trong truyện 7 viên ngọc rồng?', 'Santa tóc xù', 'Sôngôhan', 'Cađíc', 'Sôngôku', 2, 400, 4, 3, 2, 1),
(6, 2, 'Loại xe nào sau đây phải chạy trên đường ray?', 'Xe lửa', 'Xe hơi', 'Xe tải', 'Xe môtô', 1, 400, 5, 4 ,3 ,2),
(7, 3, 'Trong các chương trình sau đây, chương trình nào dùng để duyệt Web?', 'Unikey', 'Internet explorer', 'Excel', 'Notepad', 2, 600, 9, 4, 2, 5),
(8, 3, 'Theo ngôn ngữ Tin học, trong hệ nhị phân thì 1 + 1 = ?', '1', '2', '10', '100', 3, 600, 3, 4, 2, 1),
(9, 3, 'Thành phần nào của máu tham gia vào quá trình đông máu ở vết thương?', 'Hồng cầu', 'Bạch cầu', 'Tiểu cầu', 'Hemoglobin', 3, 600, 3, 4, 5, 6),
(10, 4, 'Từ nào trái nghĩa với từ thưa thớt?', 'Vắng vẻ', 'Vui vẻ', 'Đông đúc', 'Đầy đủ', 3, 1000, 4, 2, 1, 6),
(11, 4, 'Trong cờ Tướng, mỗi quân Tốt có thể đi được bao nhiêu vị trí?', '44', '45', '46', '47', 4, 1000, 5, 2, 1, 7),
(12, 4, 'Xét về sức nặng chịu được, con vật nào sau đây khoẻ nhất?', 'Voi', 'Kiến', 'Hà mã', 'Trâu', 2, 1000, 6, 5, 3, 2),
(13, 5, 'Dụng cụ nào sau đây biến đổi cơ năng thành điện năng?', 'Máy bơm nước', 'Đinamô', 'Máy hơi nước', 'Động cơ điện', 2, 2000, 5, 3, 7, 8),
(14, 5, 'Mặt trước của tờ giấy bạc 10.000 đồng bằng cotton có in hình gì?', 'Hồ Chí Minh', 'Khai thác dầu khí', 'Vịnh Hạ Long', 'Cảng Hải Phòng', 1, 2000, 7, 8, 3, 5),
(15, 5, 'Loại cây nào không thuộc nhóm cây thực phẩm?', 'Nấm hương', 'Măng', 'Quế', 'Mộc nhĩ', 3, 2000, 5, 4, 3, 2),
(16, 6, 'Đơn vị đo lực là gì?', 'Kg', 'N', 'm', 'Nm3', 2, 3000, 1, 6, 9, 5),
(17, 6, 'Từ "nó" trong câu: "Nó dài dài, màu nâu, hay nheo lại như chổi nắng" là loại từ gì?', 'Quan hệ từ', 'Đại từ', 'Tình thái từ', 'Chỉ từ', 4, 3000, 4, 1, 7, 8),
(18, 6, 'Lúc Bác Hồ ở Hồng Kông, Bác lấy tên là gì?', 'Hồ Chí Minh', 'Nguyễn Tất Thành', 'Tống Văn Sơ', 'Nguyễn Ái Quốc', 3, 3000, 2, 6, 2, 3),
(19, 7, 'Quốc kỳ của quốc gia nào chỉ có một màu xanh lá cây?', 'Marroc', 'Sudan', 'Lybia', 'Algieria', 3, 6000, 5, 6, 7, 3),
(20, 7, 'World Cup đầu tiên có bao nhiêu đội tuyển quốc gia tham dự?', '13', '14', '15', '16', 1, 6000, 2, 1, 7, 9),
(21, 7, 'Trong truyện Ông lão đánh cá và con cá vàng, ông lão đã ra bờ sông mấy lần?', '3', '4', '5', '6', 4, 6000, 2, 3, 5, 7),
(22, 8, 'Ai là tác giả của "Đại Việt Sử Ký toàn thư"?', 'Lê Văn Hưu', 'Nguyễn Trãi', 'Ngô Thì Nhậm', 'Ngô Sỹ Liên', 4, 10000, 7, 6, 5, 3),
(23, 8, 'Để tắt một cửa sổ trong Hệ điều hành Windows, có thể dùng tổ hợp phím nào?', 'Ctrl + F4', 'Alt + F4', 'Shift + F4', 'Ctrl + Shift + F4', 2, 10000, 5, 6, 7, 3),
(24, 8, 'Chức quan mang lại quyền tự chủ cho nước ta cuối thời chịu ách đô hộ của quân Nam Hán?', 'Thứ sử', 'Thái Thú', 'Tiết Độ Sứ', 'Hào trưởng', 3, 10000, 5, 6, 7, 3),
(25, 9, 'Hành tinh nào sau đây lớn thứ 3 trong hệ mặt trời?', 'Sao Thổ', 'Sao Thiên Vương', 'Sao Hải Vương', 'Trái đất', 2, 14000, 5, 6, 7, 3),
(26, 9, 'Các công trình kiến trúc nổi tiếng ở Huế còn lại đến ngày nay đã được xây dựng dưới thời?', 'Nhà Lê', 'Trịnh Nguyên phân tranh', 'Nhà Nguyễn', 'Tây Sơn', 3, 14000, 5, 6, 7, 3),
(27, 9, 'Huyện đảo Trường Sa thuộc tỉnh thành phố nào của Việt Nam?', 'Thừa Thiên Huế', 'Khánh Hoà', 'Quảng Nam', 'Quảng Ngãi', 2, 14000, 5, 6, 7, 3),
(28, 10, 'Trong bức tranh nổi tiếng Bữa tiệc ly vẽ chúa Jesus và 12 học trò, Leonardo de Vinci đã sử dụng chất liệu nào?', 'Sơn dầu trên toan', 'Sơn dầu trên gỗ', 'Sơn dầu và Tempera trên tường', 'Tempera trên toan', 3, 22000, 5, 6, 7, 3),
(29, 10, '"Đêm đông" là sáng tác của nhạc sĩ nào?', 'Phạm Tuyên', 'Đỗ Nhuận', 'Phan Huỳnh Điểu', 'Nguyễn Văn Thương', 4, 22000, 5, 6, 7, 3),
(30, 10, 'Bài thơ nào sau đây không nằm trong tập Nhật ký trong tù của Hồ Chí Minh?', 'Tân xuất ngục học đăng sơn', 'Tảo giải', 'Thế lộ nan', 'Tẩu lộ', 1, 22000, 5, 6, 7, 3),
(31, 11, 'Sản lượng điện bình quân theo đầu người cao nhất thế giới thuộc về?', 'Nhật Bản', 'Nga', 'Hoa Kỳ', 'Trung Quốc', 3, 30000, 5, 6, 7, 3),
(32, 11, 'Người châu Âu đầu tiên thám hiểm sông Mekong là người quốc gia nào?', 'Pháp', 'Bồ Đào Nha', 'Hà Lan', 'Tây Ban Nha', 2, 30000, 5, 6, 7, 3),
(33, 11, 'Đến năm 2002 tổ chức ASEAN gồm bao nhiêu quốc gia?', '10', '9', '8', '7', 1, 30000, 5, 6, 7, 3),
(34, 12, 'Núi Ngũ Hành Sơn ở Đà Nẵng được vua Minh Mạng nhà Nguyễn đặt tên vào thế kỷ nào?', 'Thế kỷ XII', 'Thế kỷ XIII', 'Thế kỷ XIV', 'Thế kỷ XV', 3, 40000, 5, 6, 7, 3),
(35, 12, 'Lò vi sóng còn gọi là lò vi ba có mặt trên thị trường vào năm nào?', '1984', '1956', '1800', '1967', 4, 40000, 5, 6, 7, 3),
(36, 12, 'Triều đại nhà Nguyễn bắt đầu từ năm nào?', '1802', '1790', '1845', '1860', 1, 40000, 5, 6, 7, 3),
(37, 13, 'Đường sắt Bắc Nam không đi qua tỉnh nào trong các tỉnh nào sau đây?', 'Ninh Thuận', 'Bình Thuận', 'Đồng Nai', 'Lâm Đồng', 4, 60000, 5, 6, 7, 3),
(38, 13, 'Hiệp định Chính phủ Pháp còn được gọi là nền Cộng hoà thứ mấy?', '2', '3', '4', '5', 4, 60000, 5, 6, 7, 3),
(39, 13, 'Ở vòng cực Nam, ngày duy nhất dài 24h là?', '21-03', '22-06', '23-09', '22-12', 4, 60000, 5, 6, 7, 3),
(40, 14, 'Quốc gia nào có tỷ lệ tử vong nhiều nhất thế giới?', 'Zimbabwe', 'Somalia', 'Afganistan', 'Mông Cổ', 3, 85000, 5, 6, 7, 3),
(41, 14, 'Ngày 01/09 là quốc khánh của quốc gia nào sau đây?', 'Uzbekistan', 'Uruguay', 'Uganda', 'Ukraine', 1, 85000, 5, 6, 7, 3),
(42, 14, 'Ngày 01/01 là quốc khánh của quốc gia nào sau đây?', 'Sri Lanka', 'Sudan', 'Syria', 'Singapore', 2, 85000, 5, 6, 7, 3),
(43, 15, 'Bạn hãy cho biết mức chở tối đa của xe đạp thồ là bao nhiêu?', '252Kgchuyến', '152 kgchuyến', '352Kgchuyến', '452Kgchuyến', 3, 150000, 5, 6, 7, 3),
(44, 15, 'Georgetown là thủ đô của quốc gia nào?', 'Guyana', 'Ghana', 'Gabong', 'Goergia', 1, 150000, 5, 6, 7, 3),
(45, 15, 'Vua Quang Trung đã đánh tan 29 vạn quân Thanh vào năm nào?', '1600', '1789', '1801', '1812', 2, 150000, 5, 6, 7, 3);
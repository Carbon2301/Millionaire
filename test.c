#include <stdio.h>
#include <pthread.h>
#include <unistd.h>  // Thư viện để sử dụng sleep

int time_left = 15;  // Thời gian đếm ngược từ 15 giây
int answered = 0;    // Cờ kiểm tra người dùng đã nhập đáp án chưa
char answer[100];    // Mảng lưu đáp án người dùng nhập

// Hàm đếm ngược thời gian
void* countdown(void* arg) {
    while (time_left > 0) {
        if (answered == 1) {
            break;  // Nếu người dùng đã nhập đáp án, dừng bộ đếm
        }
        printf("\rCòn lại: %d giây", time_left);  // In ra thời gian còn lại
        fflush(stdout);  // Đảm bảo in ngay lập tức
        sleep(1);  // Tạm dừng 1 giây
        time_left--;  // Giảm thời gian
    }

    if (answered == 0) {
        printf("\nHết giờ! Bạn đã thua cuộc.\n");
    }

    return NULL;
}

// Hàm nhập đáp án từ người dùng
void* get_answer(void* arg) {
    printf("Câu hỏi: 2 + 2 là bao nhiêu?\n");
    printf("Bạn có 15 giây để trả lời.\n");

    // Nhập đáp án của người dùng
    fgets(answer, sizeof(answer), stdin);

    // Kiểm tra đáp án và thay đổi cờ đã trả lời
    answered = 1;
    if (answer[0] == '4') {
        printf("Đáp án đúng! Chúc mừng bạn!\n");
    } else {
        printf("Đáp án sai. Bạn đã thua cuộc.\n");
    }

    return NULL;
}

int main() {
    pthread_t timer_thread, answer_thread;

    // Tạo hai luồng: một luồng cho bộ đếm thời gian, một luồng cho việc nhập đáp án
    pthread_create(&timer_thread, NULL, countdown, NULL);
    pthread_create(&answer_thread, NULL, get_answer, NULL);

    // Chờ cho cả hai luồng hoàn thành
    pthread_join(timer_thread, NULL);
    pthread_join(answer_thread, NULL);

    return 0;
}

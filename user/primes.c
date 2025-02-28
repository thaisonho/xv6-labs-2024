#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void sieve(int left_pipe[2]) __attribute__((noreturn));

void sieve(int left_pipe[2]) {
  int prime;
  int n;
  
  // Đọc số nguyên tố đầu tiên từ pipe bên trái
  if (read(left_pipe[0], &prime, sizeof(prime)) == 0) {
    // Nếu không đọc được gì (pipe đã đóng), thoát
    close(left_pipe[0]);
    exit(0);
  }
  
  // In ra số nguyên tố vừa đọc được
  printf("prime %d\n", prime);
  
  // Tạo pipe để kết nối với process tiếp theo
  int right_pipe[2];
  pipe(right_pipe);
  
  // Tạo process con mới
  int pid = fork();
  
  if (pid == 0) {
    // Process con: đóng đầu vào của pipe bên trái và đầu ra của pipe bên phải
    close(left_pipe[0]);
    close(right_pipe[1]);
    
    // Gọi đệ quy với pipe bên phải làm pipe bên trái mới
    sieve(right_pipe);
    
    // Dòng này sẽ không bao giờ được thực thi vì sieve sẽ gọi exit()
  } else {
    // Process cha: đóng đầu vào của pipe bên phải
    close(right_pipe[0]);
    
    // Đọc các số từ pipe bên trái và gửi chúng qua pipe bên phải nếu không phải là bội của prime
    while (read(left_pipe[0], &n, sizeof(n)) > 0) {
      if (n % prime != 0) {
        // Không phải bội của prime, gửi tới process tiếp theo
        write(right_pipe[1], &n, sizeof(n));
      }
    }
    
    // Đóng các pipe khi đã xử lý xong
    close(left_pipe[0]);
    close(right_pipe[1]);
    
    // Đợi process con kết thúc
    wait(0);
    
    // Kết thúc process này
    exit(0);
  }
}

int main(int argc, char *argv[]) {
  // Tạo pipe đầu tiên
  int p[2];
  pipe(p);
  
  // Tạo process con đầu tiên để bắt đầu sàng
  int pid = fork();
  
  if (pid == 0) {
    // Process con: đóng đầu ghi của pipe
    close(p[1]);
    
    // Bắt đầu quá trình sàng
    sieve(p);
    
    // Dòng này sẽ không bao giờ được thực thi vì sieve sẽ gọi exit()
  } else {
    // Process cha: đóng đầu đọc của pipe
    close(p[0]);
    
    // Gửi các số từ 2 đến 280 vào pipe
    for (int i = 2; i <= 280; i++) {
      write(p[1], &i, sizeof(i));
    }
    
    // Đóng pipe khi đã gửi xong
    close(p[1]);
    
    // Đợi process con kết thúc
    wait(0);
  }
  
  exit(0);
}
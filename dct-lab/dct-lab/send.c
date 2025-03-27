#include <stdio.h>
#include <stdlib.h>

int main() {
    char *remote_user = "student";            // Tên user trên máy nhận
    char *remote_ip = "192.168.116.140";      // Địa chỉ IP của máy nhận
    char *remote_path = "/home/student/";     // Đường dẫn lưu file trên máy nhận
    char *file_name = "stego_output.bin";           // Tên file cần gửi

    // Tạo lệnh SCP
    char scp_command[256];
    snprintf(scp_command, sizeof(scp_command), "scp %s %s@%s:%s",
             file_name, remote_user, remote_ip, remote_path);

    printf("Đang gửi file...\n");

    // Thực hiện lệnh SCP và lấy kết quả trả về
    int status = system(scp_command);

    // Kiểm tra kết quả trả về của lệnh SCP
    if (status == 0) {
        printf("YES\n");
    } else {
        printf("NO\n");
    }

    return 0;
}

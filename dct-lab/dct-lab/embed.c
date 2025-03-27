#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define BLOCK_SIZE 8  // DCT hoạt động trên khối 8x8
#define MAX_MSG_SIZE 1024  // Giới hạn độ dài thông điệp

// Hàm tính DCT
void dct(double block[BLOCK_SIZE][BLOCK_SIZE]) {
    double temp[BLOCK_SIZE][BLOCK_SIZE];
    double alpha, beta;

    for (int u = 0; u < BLOCK_SIZE; u++) {
        for (int v = 0; v < BLOCK_SIZE; v++) {
            temp[u][v] = 0.0;
            for (int x = 0; x < BLOCK_SIZE; x++) {
                for (int y = 0; y < BLOCK_SIZE; y++) {
                    alpha = (x == 0) ? sqrt(1.0 / BLOCK_SIZE) : sqrt(2.0 / BLOCK_SIZE);
                    beta = (y == 0) ? sqrt(1.0 / BLOCK_SIZE) : sqrt(2.0 / BLOCK_SIZE);
                    temp[u][v] += alpha * beta * block[x][y] * 
                        cos(((2 * x + 1) * u * M_PI) / (2 * BLOCK_SIZE)) * 
                        cos(((2 * y + 1) * v * M_PI) / (2 * BLOCK_SIZE));
                }
            }
        }
    }

    for (int i = 0; i < BLOCK_SIZE; i++)
        for (int j = 0; j < BLOCK_SIZE; j++)
            block[i][j] = temp[i][j];
}

// Giấu tin vào hệ số DCT (4,4)
void hide_message(double block[BLOCK_SIZE][BLOCK_SIZE], char bit) {
    if (bit == '1') {
        block[4][4] += 10.0;  // Nếu là bit 1, tăng giá trị hệ số
    } else {
        block[4][4] -= 10.0;  // Nếu là bit 0, giảm nhẹ
    }
}

// Đọc nội dung từ file `mess.txt`
int read_message_from_file(const char *filename, char *message, size_t max_len) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("NO\n");  // Không thể mở file thông điệp
        return 0;
    }
    fgets(message, max_len, file);
    fclose(file);
    return 1;  // Đọc thành công
}

// Giấu tin vào file nhị phân
void embed_message(const char *input_bin, const char *message_file, const char *output_bin) {
    FILE *infile = fopen(input_bin, "rb");
    if (infile == NULL) {
        printf("NO\n");
        return;
    }

    FILE *outfile = fopen(output_bin, "wb");
    if (outfile == NULL) {
        printf("NO\n");
        fclose(infile);
        return;
    }

    fseek(infile, 0, SEEK_END);
    long file_size = ftell(infile);
    rewind(infile);

    unsigned char *buffer = (unsigned char *)malloc(file_size);
    fread(buffer, 1, file_size, infile);
    fclose(infile);

    // Đọc thông điệp từ file `mess.txt`
    char message[MAX_MSG_SIZE];
    if (!read_message_from_file(message_file, message, MAX_MSG_SIZE)) {
        free(buffer);
        fclose(outfile);
        return;
    }

    // Chuyển thông điệp thành dãy bit
    size_t msg_len = strlen(message);
    char message_bits[msg_len * 8 + 1];
    message_bits[0] = '\0';

    for (size_t i = 0; i < msg_len; i++) {
        char temp[9];
        for (int j = 7; j >= 0; j--)
            temp[7 - j] = ((message[i] >> j) & 1) ? '1' : '0';
        temp[8] = '\0';
        strcat(message_bits, temp);
    }

    // Nhúng tin vào các khối 8x8 trong dữ liệu ảnh
    size_t bit_index = 0;
    for (long i = 0; i < file_size; i += BLOCK_SIZE * BLOCK_SIZE) {
        if (i + BLOCK_SIZE * BLOCK_SIZE > file_size) break;  // Nếu thiếu dữ liệu, bỏ qua

        double block[BLOCK_SIZE][BLOCK_SIZE];

        for (int x = 0; x < BLOCK_SIZE; x++)
            for (int y = 0; y < BLOCK_SIZE; y++)
                block[x][y] = (i + x * BLOCK_SIZE + y < file_size) ? buffer[i + x * BLOCK_SIZE + y] : 0;

        dct(block);  // Chuyển đổi sang miền tần số

        if (bit_index < strlen(message_bits)) {
            hide_message(block, message_bits[bit_index]);  // Nhúng tin vào hệ số DCT (4,4)
            bit_index++;
        }

        // Ghi lại dữ liệu đã giấu tin vào buffer
        for (int x = 0; x < BLOCK_SIZE; x++)
            for (int y = 0; y < BLOCK_SIZE; y++)
                buffer[i + x * BLOCK_SIZE + y] = (unsigned char)block[x][y];
    }

    fwrite(buffer, 1, file_size, outfile);
    fclose(outfile);
    free(buffer);

    printf("YES\n");  // Thành công
}

int main() {
    embed_message("output.bin", "mess.txt", "stego_output.bin");
    return 0;
}

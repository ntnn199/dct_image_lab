#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

#define BLOCK_SIZE 8  // DCT hoạt động trên khối 8x8

// Đọc ảnh JPEG và lưu thành file nhị phân
int save_binary_image(const char *input_file, const char *output_file, int *width, int *height) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE *infile = fopen(input_file, "rb");
    if (infile == NULL) {
        printf("NO\n");
        return 1;  // Lỗi: Không mở được file ảnh
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);

    if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) {
        printf("NO\n");
        fclose(infile);
        return 1;  // Lỗi: File không phải JPEG hợp lệ
    }

    jpeg_start_decompress(&cinfo);
    *width = cinfo.output_width;
    *height = cinfo.output_height;
    int channels = cinfo.output_components;

    int row_stride = (*width) * channels;
    unsigned char *buffer = (unsigned char *)malloc(row_stride);

    FILE *outfile = fopen(output_file, "wb");
    if (outfile == NULL) {
        printf("NO\n");
        fclose(infile);
        return 1;  // Lỗi: Không mở được file nhị phân
    }

    while (cinfo.output_scanline < *height) {
        jpeg_read_scanlines(&cinfo, &buffer, 1);
        fwrite(buffer, 1, row_stride, outfile);
    }

    fclose(outfile);
    free(buffer);
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 0;
}

// Chia ảnh thành các khối 8x8 (không in ra ma trận)
int split_into_blocks(const char *binary_file, int width, int height) {
    FILE *file = fopen(binary_file, "rb");
    if (file == NULL) {
        printf("NO\n");  // Lỗi khi mở file
        return 1;
    }

    int block_count_x = width / BLOCK_SIZE;
    int block_count_y = height / BLOCK_SIZE;

    unsigned char *image_data = (unsigned char *)malloc(width * height);
    fread(image_data, 1, width * height, file);
    fclose(file);

    free(image_data);
    return 0;
}

int main() {
    const char *input_image = "input.jpg";
    const char *binary_output = "output.bin";

    int width, height;

    // Chuyển ảnh thành nhị phân
    if (save_binary_image(input_image, binary_output, &width, &height) == 0) {
        // Nếu chuyển đổi thành công, chia thành khối 8x8
        if (split_into_blocks(binary_output, width, height) == 0) {
            printf("YES\n");  // Thành công
        } else {
            printf("NO\n");  // Lỗi chia khối
        }
    } else {
        printf("NO\n");  // Lỗi khi lưu file nhị phân
    }

    return 0;
}

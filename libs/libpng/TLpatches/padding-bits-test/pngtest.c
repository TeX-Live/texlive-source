#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void try_decode(const char *path, unsigned char fill) {
    FILE *fp = fopen(path, "rb");
    png_structp p = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(p);
    png_init_io(p, fp);
    png_read_info(p, info);
    int w = png_get_image_width(p, info);
    int h = png_get_image_height(p, info);
    int rb = png_get_rowbytes(p, info);
    (void)png_set_interlace_handling(p);
    png_read_update_info(p, info);
    unsigned char *row = malloc(rb);
    unsigned char crc = 0;
    for (int i = 0; i < h; i++) {
        memset(row, fill, rb);
        png_read_row(p, row, NULL);
        for (int k = 0; k < rb; k++) crc ^= row[k];
        if (i == 0) {
            printf("fill=0x%02x row0 last bytes:", fill);
            for (int k = rb - 4; k < rb; k++) printf(" %02x", row[k]);
            printf("\n");
        }
    }
    // printf("fill=0x%02x w=%d h=%d rb=%d xor=%02x\n", fill, w, h, rb, crc);
    free(row);
    png_destroy_read_struct(&p, &info, NULL);
    fclose(fp);
}

int main(int argc, char **argv) {
    try_decode(argv[1], 0x00);
    try_decode(argv[1], 0xff);
    try_decode(argv[1], 0xaa);
    return 0;
}

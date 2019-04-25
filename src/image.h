#ifndef IMAGE_H
#define IMAGE_H


#include <QColorDialog>
#include <QMessageBox>
#include <iostream>
#include <stdio.h>
#include <cmath>

class Image
{

#pragma pack (push, 1)
typedef struct{
    int16_t bfType;
    int32_t bfSize = 0;
    int16_t bfReserved1;
    int16_t bfReserved2;
    int32_t bfOffBits;
}bit_map_file_header;


typedef struct{
  int32_t biSize;
  int32_t biWidth;
  int32_t biHeight;
  int16_t biPlanes;
  int16_t biBitCount;
  int32_t biCompression;
  int32_t biSizeImage;
  int32_t biXPelsPerMeter;
  int32_t biYPelsPerMeter;
  int32_t biClrUsed;
  int32_t biClrImportant;
}bit_map_file_in_fo_header;
typedef struct{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
}rgb_tripple;

#pragma pack (pop)
typedef struct{
    int x;
    int y;
}List;
public:
    int max = 0;
    int x1;
    int y1;
    bit_map_file_header bit_head;
    bit_map_file_in_fo_header bit_in_head;
    rgb_tripple **rgb;
    QImage *image;
    int load_bmp(const char *);
    int save_bmp(const char *);
    int draw_rect(int x1, int y1, int x2, int y2, int thick, QColor color_lines, QColor color_flood, bool flag);
    int compression(int n, int m);
    int reflection(int x1, int x2, int y1, int y2, bool flag);
    int draw_pentagramm(int x, int y, int r, QColor color_circle, QColor color_lines, int thick_circle, int thick_lines);
    int draw_line(int x1, int y1, int x2, int y2, QColor color_lines);
    QPixmap get_pixmap();
//    int flood(int x, int y, int x0, int y0, int temp);
    int flood(int y, int x, int *x_t, int *y_t, unsigned char new_red, unsigned char new_green, unsigned char new_blue);
    int repaint();
    int rotate(int angle);
};

#endif // IMAGE_H

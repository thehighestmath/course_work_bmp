#include "image.h"
#include <iostream>
#include <QDebug>
#include <queue>
#include <cstdlib>
#include <iostream>
#define N 10000
#define B 1000
#define E 0.004

int Image::load_bmp(const char *fname)
{
    FILE *f = fopen(fname, "rb");
    if(!f)
        return -1;
    fread(&bit_head, sizeof(bit_head), 1, f);
    fread(&bit_in_head, sizeof(bit_in_head), 1, f);
    if (bit_in_head.biBitCount != 24){
        return -3;
    }
    if (bit_in_head.biWidth > N || bit_in_head.biHeight > N)
        return -2;
    int mx = bit_in_head.biWidth;
    int my = bit_in_head.biHeight;
    size_t padding = 0;
    if ((mx*3) % 4)
        padding = 4 - (mx*3) % 4;
    rgb = new rgb_tripple* [my];
    for (int i = 0; i < my; i++){
        rgb[i] = new rgb_tripple[mx];
    }
    fseek(f, long(bit_head.bfOffBits), SEEK_SET);
    for (int i = 0; i < my; i++){
        int j;
        for (j = 0; j < mx; j++){
            fread(&rgb[i][j], sizeof(rgb_tripple), 1, f);
        }
        if (padding)
            fread(&rgb[i][j], padding, 1, f);
    }
    fclose(f);
    return 0;
}

int Image::save_bmp(const char *fname){
    FILE *f = fopen(fname, "wb");
    if(!f)
        return -1;
    fwrite(&bit_head, sizeof(bit_head), 1, f);
    fwrite(&bit_in_head, sizeof(bit_in_head), 1, f);
    int mx = bit_in_head.biWidth;
    int my = bit_in_head.biHeight;
    size_t padding = 0;
    if ((mx*3) % 4)
        padding = 4 - (mx*3) % 4;
    fseek(f, long(bit_head.bfOffBits), SEEK_SET);
    for (int i = 0; i < my; i++){
        int j;
        for (j = 0; j < mx; j++){
            fwrite(&rgb[i][j], sizeof(rgb_tripple), 1, f);
        }
        if (padding)
            fwrite(&rgb[i][j], padding, 1, f);
    }
    fclose(f);
    return 0;
}

QPixmap Image::get_pixmap(){
    image = new QImage(bit_in_head.biWidth, bit_in_head.biHeight, QImage::Format_RGB16);
    QColor color = QColor(255, 255, 255);
    image->fill(color);
    QColor pixel;
    int y = bit_in_head.biHeight;
    int x = bit_in_head.biWidth;
    for (int i = y - 1; i >= 0; i--){
        for (int j = 0; j < x; j++){
            pixel.setRed(rgb[i][j].red);
            pixel.setGreen(rgb[i][j].green);
            pixel.setBlue(rgb[i][j].blue);
            image->setPixel(j, y - i - 1, pixel.rgb());
        }
    }
    return QPixmap::fromImage(*image);
}

int Image::draw_rect(int x1, int x2, int y1, int y2, int thick, QColor color_lines, QColor color_flood, bool flag){
    unsigned char red = static_cast<unsigned char>(color_lines.red());
    unsigned char green = static_cast<unsigned char>(color_lines.green());
    unsigned char blue = static_cast<unsigned char>(color_lines.blue());
    unsigned char red_flood = '\0';
    unsigned char green_flood = '\0';
    unsigned char blue_flood = '\0';
    if (flag){
        red_flood = static_cast<unsigned char>(color_flood.red());;
        green_flood = static_cast<unsigned char>(color_flood.green());;
        blue_flood = static_cast<unsigned char>(color_flood.blue());;
    }
    y2 = bit_in_head.biHeight - y2;
    y1 = bit_in_head.biHeight - y1;
    if (y2 < y1){
        std::swap(y1, y2);
    }
    if (x1 < x2){
        std::swap(x1, x2);
    }
    for (int i = y1; i <= y2; i++) {
        for (int j = x2; j <= x1; j++){
            if (abs(i-y1) < thick || abs(j-x1) < thick || abs(i-y2) < thick || abs(j-x2) < thick){
                rgb[i][j].red = red;
                rgb[i][j].green = green;
                rgb[i][j].blue = blue;
            }
            else if (flag){
                rgb[i][j].red = red_flood;
                rgb[i][j].green = green_flood;
                rgb[i][j].blue = blue_flood;
            }
        }
    }
    return 0;
}

int Image::compression(int m, int n){
    int mx = bit_in_head.biWidth;
    int my = bit_in_head.biHeight;
    int small_x = mx / m;
    int small_y = my / n;
    for (int i = 0; i < my - 1; i++){
        if (i % n){
            rgb[i][0].red = '\0';
            rgb[i][0].green = '\0';
            rgb[i][0].blue = '\0';
        }
    }

    for (int i = 0; i < my - 1;){
        if (rgb[i][0].red == '\0' && rgb[i][0].green == '\0' && rgb[i][0].blue == '\0'){
                delete rgb[i];
                memmove(rgb + i, rgb + i + 1, ((size_t(my) - size_t(i) - 1) * sizeof(rgb_tripple*)));
                i--;
        }
        i++;
    }

    for (int i = 0; i < small_y; i++){
        for (int j = 0; j < mx - 1; j++){
            if (j % m){
                rgb[i][j].red = '\0';
                rgb[i][j].green = '\0';
                rgb[i][j].blue = '\0';
            }
        }
    }

    for (int i = 0; i < my - 1; i++){
        for (int j = 0; j < mx - 1;){
            if (rgb[i][j].red == '\0' && rgb[i][j].green == '\0' && rgb[i][j].blue == '\0'){
                memmove(rgb[i] + j, rgb[i] + j + 1, ((size_t(mx) - size_t(j) - 1) * sizeof(rgb_tripple)));
                j--;
            }
            j++;
        }
    }

    for (int i = 0; i < small_y; i++){
        for (int k = 1; k < m; k++){
            for (int j = 0; j < small_x; j++){
                rgb[i][j + k * small_x].red = rgb[i][j].red;
                rgb[i][j + k * small_x].green = rgb[i][j].green;
                rgb[i][j + k * small_x].blue = rgb[i][j].blue;
            }
        }
    }
    for (int i = small_y; i < my; i++){
        rgb[i] = new rgb_tripple[mx];
    }
    for (int j = 0; j < mx; j++){
        for (int k = 1; k < n; k++){
            for (int i = 0; i < small_y; i++){
                rgb[i + k * small_y][j].red = rgb[i][j].red;
                rgb[i + k * small_y][j].green = rgb[i][j].green;
                rgb[i + k * small_y][j].blue = rgb[i][j].blue;
            }
        }
    }
    for (int i = 0; i < my; i++){
        for (int j = 0; j < mx; j++){
            if (i >= small_y * n || j >= small_x * m){
                rgb[i][j].red = 255;
                rgb[i][j].green = 255;
                rgb[i][j].blue = 255;
            }
        }
    }
    return 0;
}

int Image::reflection(int x1, int x2, int y1, int y2, bool flag){
    int mid;
    y2 = bit_in_head.biHeight - y2;
    y1 = bit_in_head.biHeight - y1;
    if (flag)
        mid = (x1 + x2) / 2;
    else
        mid = (y1 + y2) / 2;

    if (y2 < y1){
        std::swap(y1, y2);
    }
    if (x1 < x2){
        std::swap(x1, x2);
    }
    if (flag){
        for (int i = y1; i <= y2; i++) {
            for (int j = x2; j <= mid; j++){
                std::swap(rgb[i][j], rgb[i][x1 + x2 - j]);
            }
        }
    }
    else{
        for (int j = x2; j <= x1; j++){
            for (int i = y1; i <= mid; i++){
                std::swap(rgb[i][j], rgb[y1 + y2 - i][j]);
            }
        }
    }
    return 0;
}

int Image::draw_pentagramm(int x, int y, int r, QColor color_circle, QColor color_lines, int thick_circle, int thick_lines){
    unsigned char red = static_cast<unsigned char>(color_circle.red());
    unsigned char green = static_cast<unsigned char>(color_circle.green());
    unsigned char blue = static_cast<unsigned char>(color_circle.blue());
    int x_p[5];
    int y_p[5];
    int count = 0;
    double vertical_point = -1;
    y = bit_in_head.biHeight - y;
    int x1, y1;
    bool flag = 1;
    for (double a = 0; a < B * B; a++){
        for (double r1 = r; r1 >= r - thick_circle; r1--){
            x1 = static_cast<int>((r1*cos(a/r/r)));
            y1 = static_cast<int>((r1*sin(a/r/r)));
            if (!x1 && flag){
                vertical_point = a/r/r;
                flag = 0;
            }
            if (fabs((a / r / r) - (vertical_point + (count * 2 * M_PI / 5))) < E && count < 5){
                x_p[count] = x1 + x;
                y_p[count] = y1 + y;
                count++;
            }
            rgb[y + y1][x + x1].red = red;
            rgb[y + y1][x + x1].green = green;
            rgb[y + y1][x + x1].blue = blue;
        }
    }
    y_p[4] = y_p[1];
    for (int i = 0; i < count - 2; i++){
        for (int j = -thick_lines/2; j <= thick_lines/2; j++){
            draw_line(x_p[i] + j, y_p[i], x_p[i + 2] + j, y_p[i + 2], color_lines);
        }
    }
    for (int j = -thick_lines/2; j <= thick_circle/2; j++){
        draw_line(x_p[0] + j, y_p[0], x_p[3] + j, y_p[3], color_lines);
        draw_line(x_p[1], y_p[1] + j, x_p[4], y_p[4] + j, color_lines);
    }
    return 0;
}

int Image::draw_line(int y1, int x1, int y2, int x2, QColor color_lines){
    const int deltaX = abs(x2 - x1);
    const int deltaY = abs(y2 - y1);
    const int signX = x1 < x2 ? 1 : -1;
    const int signY = y1 < y2 ? 1 : -1;
    unsigned char red = static_cast<unsigned char>(color_lines.red());
    unsigned char green = static_cast<unsigned char>(color_lines.green());
    unsigned char blue = static_cast<unsigned char>(color_lines.blue());
    int error = deltaX - deltaY;
    rgb[x2][y2].red = red;
    rgb[x2][y2].green = green;
    rgb[x2][y2].blue = blue;
    while(x1 != x2 || y1 != y2)
    {
        rgb[x1][y1].red = red;
        rgb[x1][y1].green = green;
        rgb[x1][y1].blue = blue;
        const int error2 = error * 2;
        if (error2 > -deltaY)
        {
            error -= deltaY;
            x1 += signX;
        }
        if (error2 < deltaX)
        {
            error += deltaX;
            y1 += signY;
        }
    }
    return 0;
}

int Image::repaint(){
    int y = bit_in_head.biHeight;
    int x = bit_in_head.biWidth;
    int x_t, y_t;
    int temp;
    QColor color(255, 255, 255);

    for (int i = 0; i < y; i++){
        for (int j = 0;  j < x; j++) {
            if (rgb[i][j].red == 255 && rgb[i][j].green == 255 && rgb[i][j].blue == 255){
                temp = flood(j, i, &x_t, &y_t, 0, 0, 0);
                if (temp > max){
                    max = temp;
                    x1 = x_t;
                    y1 = y_t;
                }
            }
        }
    }

    std::cout<< max;

    for (int i = 0; i <  y; i++){
        for (int j = 0;  j < x; j++) {
            if (rgb[i][j].red == 0 && rgb[i][j].green == 0 && rgb[i][j].blue == 0){
                rgb[i][j].red = 255;
                rgb[i][j].green = 255;
                rgb[i][j].blue = 255;
            }
        }
    }
    QColor color2{255, 0, 0};
    flood(y1, x1, &x_t, &y_t, 255, 0, 0);

    return 0;
}


int Image::flood(int y, int x, int *x_t, int *y_t, unsigned char new_red, unsigned char new_green, unsigned char new_blue){
    int temp = 0;
    // Получим размеры изображения
    int imh = bit_in_head.biWidth;
    int imw = bit_in_head.biHeight;
    // Выделим памяти для складирования координат, которые еще предстоит залить
    unsigned int STACK_SIZE = static_cast<unsigned int>((imw + 2) * (imh + 2));
    int *floodfill_stackx = static_cast<int*>(malloc(STACK_SIZE * sizeof(int)));
    int *floodfill_stacky = static_cast<int*>(malloc(STACK_SIZE * sizeof(int)));
    if (floodfill_stacky == nullptr || floodfill_stackx == nullptr)
        return 0;
    unsigned int stack_head = 0;
    unsigned int stack_tail = 0;
    floodfill_stackx[stack_head] = x;
    floodfill_stacky[stack_head] = y;
    stack_head++;

    unsigned char red = rgb[x][y].red;
    unsigned char green = rgb[x][y].green;
    unsigned char blue = rgb[x][y].blue;


    // Пока не кончится память или точки на изображении

    while (stack_head < STACK_SIZE && stack_head > stack_tail) {
        x=floodfill_stackx[stack_tail];
        y=floodfill_stacky[stack_tail];
        stack_tail++;
        if (x >= 0 && y >= 0 && x < imw && y < imh) {
            // Проверим точку справа, если она не залита, то зальем
            if (x+1 == imw)
                continue;
            if (rgb[x+1][y].red == red && rgb[x+1][y].green == green && rgb[x+1][y].blue == blue) {
                floodfill_stackx[stack_head]=x+1;
                floodfill_stacky[stack_head]=y;
                rgb[x+1][y].red = new_red;
                rgb[x+1][y].green = new_green;
                rgb[x+1][y].blue = new_blue;
                temp++;
                *x_t = x;
                *y_t = y;
                stack_head++;
                }
            // Проверим точку слева, если она не залита, то зальем
            if (x-1 == -1)
                continue;
            if (rgb[x-1][y].red == red && rgb[x-1][y].green == green && rgb[x-1][y].blue == blue) {
                floodfill_stackx[stack_head]=x-1;
                floodfill_stacky[stack_head]=y;
                rgb[x-1][y].red = new_red;
                rgb[x-1][y].green = new_green;
                rgb[x-1][y].blue = new_blue;
                temp++;
                *x_t = x;
                *y_t = y;
                stack_head++;
                }
            // Проверим точку снизу, если она не залита, то зальем
            if (y-1 == -1)
                continue;
            if (rgb[x][y-1].red == red && rgb[x][y-1].green == green && rgb[x][y-1].blue == blue) {
                floodfill_stackx[stack_head]=x;
                floodfill_stacky[stack_head]=y-1;
                rgb[x][y-1].red = new_red;
                rgb[x][y-1].green = new_green;
                rgb[x][y-1].blue = new_blue;
                temp++;
                *x_t = x;
                *y_t = y;
                stack_head++;
                }
            // Проверим точку сверху, если она не залита, то зальем
            if (y+1 == imh)
                continue;
            if (rgb[x][y+1].red == red && rgb[x][y+1].green == green && rgb[x][y+1].blue == blue) {
                floodfill_stackx[stack_head]=x;
                floodfill_stacky[stack_head]=y+1;
                rgb[x][y+1].red = new_red;
                rgb[x][y+1].green = new_green;
                rgb[x][y+1].blue = new_blue;
                temp++;
                *x_t = x;
                *y_t = y;
                stack_head++;
                }
            }

    }
    // Освободим память
    free(floodfill_stacky);
    free(floodfill_stackx);
    return temp;

}

int Image::rotate(int angle){
    int xx = bit_in_head.biWidth;
    int yy = bit_in_head.biHeight;
    int new_x, new_y;
    int center_x, center_y;
    center_x = (xx - 1) / 2;
    center_y = (yy- 1) / 2;
    rgb_tripple **temp = static_cast<rgb_tripple**>(malloc(sizeof(rgb_tripple*) * static_cast<unsigned int>(yy)));
    for (int i = 0; i < yy; i++){
        temp[i] = static_cast<rgb_tripple*>(calloc(sizeof(rgb_tripple), static_cast<size_t>(xx)));
    }
    for (int x = 0; x < xx; x++){
        for (int y = 0; y < yy; y++){
            temp[y][x].red = rgb[y][x].red;
            temp[y][x].green = rgb[y][x].green;
            temp[y][x].blue = rgb[y][x].blue;

            rgb[y][x].red = 255;
            rgb[y][x].green = 255;
            rgb[y][x].blue = 255;

        }
    }
    double a_sin = sin(angle * M_PI / 180);
    double a_cos = cos(angle * M_PI / 180);
    for (int x = 0; x < xx; x++){
        for (int y = 0; y < yy; y++){
            new_x = static_cast<int>(round((x - center_x) * a_cos - (y - center_y) * a_sin) + center_x);
            new_y = static_cast<int>(round((x - center_x) * a_sin + (y - center_y) * a_cos) + center_y);
            if (new_x > 0 && new_y > 0 && new_x < xx && new_y < yy){
                rgb[new_y][new_x].red = temp[y][x].red;
                rgb[new_y][new_x].green= temp[y][x].green;
                rgb[new_y][new_x].blue = temp[y][x].blue;
            }
        }
    }
    return 0;
}



int Image::division(int n, int m){
    // m =~ x
    // n =~ y

    int mx = bit_in_head.biWidth;
    int my = bit_in_head.biHeight;
    int small_x = mx / m;
    int small_y = my / n;
    new_rgb = new rgb_tripple** [n * m];
    int g = 0;
    for (int q = 0; q < n; q++){
        for (int k = 0; k < m; k++){
            rgb_tripple** temp = new rgb_tripple* [small_y];
            for (int t = 0; t < small_y; t++){
                temp[t] = new rgb_tripple [small_x];
            }
            for (int i = 0; i < small_y; i++){
                for (int j = 0; j < small_x; j++){
                    temp[i][j] = rgb[i + q * small_y][j + k * small_x];
                }
            }
            new_rgb[g] = temp;
            g++;
        }
    }
    return 0;
}

int Image::save_parts(QString name, int n, int m, int a, int b, int k){
    const char *fname = (QString(name) + QString("_") + QString(char(a) + 49) +
                         QString("_") + QString(char(b) + 49) + QString(".bmp")).toLocal8Bit().constData();
    FILE *f = fopen(fname, "wb");
    if(!f)
        return -1;
    bit_map_file_header temp_bh = bit_head;
    temp_bh.bfSize /= (n * m);
    bit_map_file_in_fo_header temp_bih = bit_in_head;
    temp_bih.biHeight /= n;
    temp_bih.biWidth /= m;
    fwrite(&temp_bh, sizeof(temp_bh), 1, f);
    fwrite(&temp_bih, sizeof(temp_bih), 1, f);
    int mx = temp_bih.biWidth;
    int my = temp_bih.biHeight;
    size_t padding = 0;
    if ((mx*3) % 4)
        padding = 4 - (mx*3) % 4;
    fseek(f, long(bit_head.bfOffBits), SEEK_SET);
    for (int i = 0; i < my; i++){
        int j;
        for (j = 0; j < mx; j++){
            fwrite(&new_rgb[k][i][j], sizeof(rgb_tripple), 1, f);
        }
        if (padding)
            fwrite(&new_rgb[k][i][j], padding, 1, f);
    }
    fclose(f);
    return 0;
}


int Image::crop(int x1, int y1, int x2, int y2){
    std::swap(x2, x1);
    int dy = y2 - y1 + 1;
    int dx = x2 - x1 + 1;
    rgb_tripple** temp = new rgb_tripple* [dy];
    for (int i = 0; i < dy; i++){
        temp[i] = new rgb_tripple [dx];
    }
    int i, j, k, t;
    for (i = y1, k = 0; i <= y2; i++, k++){
        for (j = x1, t = 0; j <= x2; j++, t++){
            temp[k][t] = rgb[i][j];
        }
    }
    bit_in_head.biHeight = dy;
    bit_in_head.biWidth = dx;
    delete[] rgb;
    rgb = new rgb_tripple* [dy];
    rgb = temp;
    return 0;
}

int Image::draw_around(int x, int y, int thick, QColor color){
    unsigned char new_red = static_cast<unsigned char>(color.red());
    unsigned char new_green = static_cast<unsigned char>(color.green());
    unsigned char new_blue = static_cast<unsigned char>(color.blue());
    QColor current_color;
    QColor start_color;
    QColor stop_color;
    current_color.setRed(rgb[y][x].red);
    current_color.setGreen(rgb[y][x].green);
    current_color.setBlue(rgb[y][x].blue);
    start_color = current_color;
    // Получим размеры изображения
    int imh = bit_in_head.biWidth;
    int imw = bit_in_head.biHeight;

    while (compare(rgb[x][y], start_color)){
        x++;
    }
    std::cout << rgb[x][y].red << std::endl;
    std::cout << rgb[x][y].green << std::endl;
    std::cout << rgb[x][y].blue << std::endl;
    stop_color.setRed(rgb[x][y].red);
    stop_color.setGreen(rgb[x][y].green);
    stop_color.setBlue(rgb[x][y].blue);
    x--;
    std::cout << rgb[x][y].red << std::endl;
    std::cout << rgb[x][y].green << std::endl;
    std::cout << rgb[x][y].blue << std::endl;

    // Выделим памяти для складирования координат, которые еще предстоит залить
    unsigned int STACK_SIZE = static_cast<unsigned int>((imw + 2) * (imh + 2));
    int *floodfill_stackx = static_cast<int*>(malloc(STACK_SIZE * sizeof(int)));
    int *floodfill_stacky = static_cast<int*>(malloc(STACK_SIZE * sizeof(int)));
    if (floodfill_stacky == nullptr || floodfill_stackx == nullptr)
        return 0;
    unsigned int stack_head = 0;
    unsigned int stack_tail = 0;
    floodfill_stackx[stack_head] = x;
    floodfill_stacky[stack_head] = y;
    stack_head++;

    // Пока не кончится память или точки на изображении

    while (stack_head < STACK_SIZE && stack_head > stack_tail) {
        x=floodfill_stackx[stack_tail];
        y=floodfill_stacky[stack_tail];
        stack_tail++;
        if (x >= 0 && y >= 0 && x < imw && y < imh) {
            // Проверим точку справа, если она не залита, то зальем
            if (x+1 == imw)
                continue;
            if (compare_circle(x + 1, y, stop_color)) {
                floodfill_stackx[stack_head]=x+1;
                floodfill_stacky[stack_head]=y;
                rgb[x+1][y].red = new_red;
                rgb[x+1][y].green = new_green;
                rgb[x+1][y].blue = new_blue;

                stack_head++;
                }
            // Проверим точку слева, если она не залита, то зальем
            if (x-1 == -1)
                continue;
            if (compare_circle(x - 1, y, stop_color)) {
                floodfill_stackx[stack_head]=x-1;
                floodfill_stacky[stack_head]=y;
                rgb[x-1][y].red = new_red;
                rgb[x-1][y].green = new_green;
                rgb[x-1][y].blue = new_blue;
                stack_head++;
                }
            // Проверим точку снизу, если она не залита, то зальем
            if (y-1 == -1)
                continue;
            if (compare_circle(x, y - 1, stop_color)) {
                floodfill_stackx[stack_head]=x;
                floodfill_stacky[stack_head]=y-1;
                rgb[x][y-1].red = new_red;
                rgb[x][y-1].green = new_green;
                rgb[x][y-1].blue = new_blue;
                stack_head++;
                }
            // Проверим точку сверху, если она не залита, то зальем
            if (y+1 == imh)
                continue;
            if (compare_circle(x, y + 1, stop_color)) {
                floodfill_stackx[stack_head]=x;
                floodfill_stacky[stack_head]=y+1;
                rgb[x][y+1].red = new_red;
                rgb[x][y+1].green = new_green;
                rgb[x][y+1].blue = new_blue;

                stack_head++;
                }
            }

    }
    // Освободим память
    free(floodfill_stacky);
    free(floodfill_stackx);

    return 0;
}

int Image::compare(rgb_tripple rgb, QColor color){
    if (rgb.red == color.red() && rgb.green == color.green() && rgb.blue == color.blue()){
        return 1;
    }
    return 0;
}

int Image::compare_circle(int i, int j, QColor color){
    if (compare(rgb[i + 1][j], color)){
        return 1;
    }
    if (compare(rgb[i - 1][j], color)){
        return 1;
    }
    if (compare(rgb[i][j + 1], color)){
        return 1;
    }
    if (compare(rgb[i][j - 1], color)){
        return 1;
    }
    if (compare(rgb[i + 1][j + 1], color)){
        return 1;
    }
    if (compare(rgb[i - 1][j - 1], color)){
        return 1;
    }
    if (compare(rgb[i + 1][j - 1], color)){
        return 1;
    }
    if (compare(rgb[i - 1][j + 1], color)){
        return 1;
    }
    return 0;
}

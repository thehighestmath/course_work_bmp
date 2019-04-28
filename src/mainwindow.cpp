#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mygraphicview.h"
#include "information.h"
#include "image.h"
#include <QMessageBox>
#include <QColorDialog>
#include <cstring>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    img = new Image();
    ui->setupUi(this);
    picture = new MyGraphicView();
    information = new Information();
    reference = new Reference();
    connect(picture, SIGNAL(selection()), this, SLOT(selection()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selection(){
    int x1 = picture->coords.x1;
    int x2 = picture->coords.x2;
    int y1 = picture->coords.y1;
    int y2 = picture->coords.y2;

    if (button_pressed == RECT){
        if (y2 < y1){
            std::swap(y1, y2);
        }
        if (x1 < x2){
            std::swap(x1, x2);
        }
        if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0 ||
                x1 > (img->bit_in_head.biWidth - 1) || x2 > (img->bit_in_head.biWidth - 1) ||
                y1 > (img->bit_in_head.biHeight - 1) || y2 > (img->bit_in_head.biHeight - 1)){
            QMessageBox::warning(this, "Ошибка", "Выход за пределы границ рисунка. Выберите корректную область");
            button_pressed = 0;
            return;
        }
        img->draw_rect(x1, x2, y1, y2, ui->thick_count->value(), color_lines, color_flood, flag);
        picture->update(img->get_pixmap());
        button_pressed = 0;
    }
    else if (button_pressed == REFLECTION){
        if (y2 < y1){
            std::swap(y1, y2);
        }
        if (x1 < x2){
            std::swap(x1, x2);
        }
        if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0 ||
                x1 > (img->bit_in_head.biWidth - 1) || x2 > (img->bit_in_head.biWidth - 1) ||
                y1 > (img->bit_in_head.biHeight - 1) || y2 > (img->bit_in_head.biHeight - 1)){
            QMessageBox::warning(this, "Ошибка", "Выход за пределы границ рисунка. Выберите корректную область");
            button_pressed = 0;
            return;
        }
        img->reflection(x1, x2, y1, y2, flag);
        picture->update(img->get_pixmap());
        button_pressed = 0;
    }
    else if (button_pressed == PENTAGRAMM_CIRCLE){
        int r = static_cast<int>(sqrt(pow((x2-x1), 2) + pow((y2-y1), 2)));
        if (
                x1 < 0 || y1 < 0 ||
                x1 - r < 0 || y1 - r < 0 ||
                x1 > (img->bit_in_head.biWidth - 1) || y1 > (img->bit_in_head.biHeight - 1) ||
                x1 + r > (img->bit_in_head.biWidth - 1) || y1 + r > (img->bit_in_head.biHeight - 1)){
            QMessageBox::warning(this, "Ошибка", "Выход за пределы границ рисунка. Выберите корректный центр и радиус.");
            button_pressed = 0;
            return;
        }
        if (x1 == x2 && y1 == y2){
            QMessageBox::warning(this, "Ошибка", "Выберите корректную область");
            button_pressed = 0;
            return;
        }
        img->draw_pentagramm(x1, y1, r, color_circle, color_lines, thick_circle, thick_lines);
        picture->update(img->get_pixmap());
        button_pressed = 0;
    }
    else if (button_pressed == PENTAGRAMM_SQUARE){
        if (y2 < y1){
            std::swap(y1, y2);
        }
        if (x1 < x2){
            std::swap(x1, x2);
        }
        if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0 ||
                x1 > (img->bit_in_head.biWidth - 1) || x2 > (img->bit_in_head.biWidth - 1) ||
                y1 > (img->bit_in_head.biHeight - 1) || y2 > (img->bit_in_head.biHeight - 1)){
            QMessageBox::warning(this, "Ошибка", "Выход за пределы границ рисунка. Выберите корректную область");
            return;
        }
        if (x1 == x2 && y1 == y2){
            QMessageBox::warning(this, "Ошибка", "Выберите корректную область");
            button_pressed = 0;
            return;
        }
        int max = (abs(x1 - x2) > abs(y1 - y2)) ? abs(x1 - x2) : abs(y1 - y2);
        img->draw_pentagramm(x1 - max / 2, y1 + max / 2, max / 2, color_circle, color_lines, thick_circle, thick_lines);
        picture->update(img->get_pixmap());
        button_pressed = 0;
    }
    else if (button_pressed == CROP){
        if (y2 < y1){
            std::swap(y1, y2);
        }
        if (x1 < x2){
            std::swap(x1, x2);
        }
        img->crop(x1, y1, x2, y2);
        picture->update(img->get_pixmap());
        button_pressed = 0;
    }
}

void MainWindow::on_rect_clicked()
{
    if (!img->bit_head.bfSize){
        QMessageBox::critical(this, "Ошибка!", "Без картинки нарисовать прямоугольник невозможно.");
        return;
    }
    if (ui->flood->isChecked()){
        flag = 1;
    }
    else {
        flag = 0;
    }
    color_lines = QColorDialog::getColor(Qt::white, nullptr, "Выберите цвет линий прямогольника");
    if (color_lines == nullptr){
        return;
    }
    color_flood = nullptr;
    if (flag){
        color_flood = QColorDialog::getColor(Qt::white, nullptr, "Выберите цвет заливки прямогольника");
        if (color_flood == nullptr){
            return;
        }
    }
    QMessageBox::information(this, "Сообщение", "Выберите область");
    button_pressed = RECT;
    picture->button_pressed = RECT;
}

void MainWindow::on_compress_clicked()
{
    if (!img->bit_head.bfSize){
        QMessageBox::warning(this, "Ошибка!", "Без картинки сделать коллаж невозможно.");
        return;
    }
    if (flag_c <= 0){
        QMessageBox::warning(this, "Ошибка", "Программа не может сделать может сделать более 2 раз коллаж.");
        return;
    }
    img->compression(ui->repeat_x->value(), ui->repeat_y->value());
    flag_c--;
    picture->update(img->get_pixmap());
    ui->gridLayout->addWidget(picture);
}

void MainWindow::on_reflex_clicked()
{
    flag = -1; // if flag -> vertical!
    if (ui->vertical->isChecked())
        flag = 1;
    if (ui->horizontal->isChecked())
        flag = 0;
    if (!img->bit_head.bfSize){
        QMessageBox::warning(this, "Ошибка!", "Без картинки сделать отражение невозможно.");
        return;
    }
    if (flag == -1){
        QMessageBox::warning(this, "Ошибка!", "Выберите способ отражения");
        return;
    }
    QMessageBox::information(this, "Сообщение", "Выберите область");
    button_pressed = REFLECTION;
    picture->button_pressed = REFLECTION;

}

void MainWindow::on_pentagramm_clicked(){
    int flag_p = -1;
    if (ui->pentagramm_square->isChecked())
        flag_p = 1;
    if (ui->pentagramm_circle->isChecked())
        flag_p = 0;
    if (!img->bit_head.bfSize){
        QMessageBox::critical(this, "Ошибка!", "Без картинки нарисовать пентаграмму невозможно.");
        return;
    }
    if (flag_p == -1){
        QMessageBox::warning(this, "Ошибка!", "Выберите способ рисования пентаграммы");
        return;
    }
    color_circle = QColorDialog::getColor(Qt::white, nullptr, "Выберите цвет окружности");
    if (color_circle == nullptr){
        return;
    }
    color_lines = QColorDialog::getColor(Qt::white, nullptr, "Выберите цвет линий");
    if (color_lines == nullptr){
        return;
    }

    thick_lines = ui->thick_lines->value();
    thick_circle = ui->thick_circle->value();
    if (flag_p == 0){
        QMessageBox::information(this, "Сообщение", "Выберите центр окружности о оттащите мышку на желаемый радиусе");
        button_pressed = PENTAGRAMM_CIRCLE;
        picture->button_pressed = PENTAGRAMM_CIRCLE;
    }
    else if (flag_p){
        QMessageBox::information(this, "Сообщение", "Выберите квадрат, к который хотите вписать пентаграмму");
        button_pressed = PENTAGRAMM_SQUARE;
        picture->button_pressed = PENTAGRAMM_SQUARE;
    }

}

void MainWindow::on_menu_open_triggered()
{
    QString str = QFileDialog::getOpenFileName(nullptr,
                                               "Выберите файл для открытия",
                                               "/home/therealyou/course_work/",
                                               "*.bmp");
    if (str == nullptr)
        return;
    int err = img->load_bmp(str.toLocal8Bit());
    if (err == -3){
        QMessageBox::warning(this, "Ошибка", "Картинка не является 24-ой битной, дальнейшая работа программы с этой картинкой не возможна. Выберите другую картинку.");
        return;
    }
    if (err){
        QMessageBox::warning(this, "Error", "Something gone wrong");
        return;
    }
    flag_c = 2;
    picture->update(img->get_pixmap());
    ui->gridLayout->addWidget(picture);
    information->info_s.name = QFileInfo(str).baseName();
    information->info_s.path = QFileInfo(str).filePath();
    information->info_s.is_readable = QFileInfo(str).isReadable();
    information->info_s.is_writeable = QFileInfo(str).isWritable();
    information->info_s.version = img->bit_in_head.biSize;
    information->info_s.heigth = img->bit_in_head.biHeight;
    information->info_s.width = img->bit_in_head.biWidth;
    information->info_s.weight = img->bit_head.bfSize;
}

void MainWindow::on_menu_save_triggered()
{
    if (!img->bit_head.bfSize){
        QMessageBox::critical(this, "Ошибка!", "Невозможно сохранить то, чего нет.");
        return;
    }
    QString str = QFileDialog::getSaveFileName(nullptr,
                                               "Выберите файл для сохранения",
                                               "/home/therealyou/course_work/",
                                               ".bmp");
    if (str == nullptr)
        return;
    int err = img->save_bmp((str + ".bmp").toLocal8Bit().constData());
    if (err){
        QMessageBox::warning(this, "Ошибка", "Файл не может быть сохранен в этой папке");
        return;
    }
}


void MainWindow::on_menu_help_triggered()
{
    reference->exec();
}

void MainWindow::on_menu_info_triggered()
{
    if (!img->bit_head.bfSize){
        QMessageBox::warning(this, "Ошибка!", "Невозможно показать информацию о файле. Он не загружен");
        return;
    }
    information->setModal(true);
    information->write_information();
    information->exec();
}

void MainWindow::on_pushButton_clicked()
{
    img->repaint();
    picture->update(img->get_pixmap());
    ui->gridLayout->addWidget(picture);
}

void MainWindow::on_rotate_clicked()
{
    img->rotate(ui->rotape_angle->value());
    picture->update(img->get_pixmap());
    ui->gridLayout->addWidget(picture);
}

void MainWindow::on_save_parts_clicked()
{
    int n, m;
    n = 3; // OY
    m = 4; // OX
    img->division(n, m);
    int k = 0;

    for (int i = 0; i < n; i++){
        for (int j = 0; j < m; j++){
            img->save_parts("/home/therealyou/course_work_bmp/part", n, m, i, j, k);
            k++;
        }
    }

}

void MainWindow::on_crop_clicked()
{
    button_pressed = CROP;
    picture->button_pressed = CROP;
}

void MainWindow::on_draw_around_clicked()
{
    QColor color(255, 0, 0);
    img->draw_around(img->bit_in_head.biHeight - picture->coords.y1, picture->coords.x1, 1, color);
    picture->update(img->get_pixmap());
}

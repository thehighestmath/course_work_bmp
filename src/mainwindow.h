#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QImage>
#include <QColor>
#include <QFileDialog>
#include <QMouseEvent>
#include <QPoint>
#include <QMainWindow>
#include <QFileDialog>
#include <QWidget>
#include <QGridLayout>
#include "image.h"
#include "mygraphicview.h"
#include "information.h"
#include "reference.h"

#define RECT 1
#define REFLECTION 2
#define PENTAGRAMM_CIRCLE 3
#define PENTAGRAMM_SQUARE 4

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    MyGraphicView *picture;
    QColor color_lines;
    QColor color_flood;
    QColor color_circle;
    int thick_lines, thick_circle;
    int flag;
    int flag_c = 2;
    ~MainWindow();

private slots:
    void selection();

    void on_rect_clicked();

    void on_compress_clicked();

    void on_reflex_clicked();

    void on_pentagramm_clicked();

    void on_menu_open_triggered();

    void on_menu_save_triggered();

    void on_menu_help_triggered();

    void on_menu_info_triggered();

    void on_pushButton_clicked();

    void on_rotate_clicked();

private:
    Ui::MainWindow *ui;
    Image* img;
    Information* information;
    Reference* reference;
    int button_pressed = 0;
};

#endif // MAINWINDOW_H

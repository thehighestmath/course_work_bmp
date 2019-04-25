#ifndef MYGRAPHICVIEW_H
#define MYGRAPHICVIEW_H

#include <QWidget>
#include <QPointF>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QTimer>
#include <QMouseEvent>
#include <cmath>

#define RECT 1
#define REFLECTION 2
#define PENTAGRAMM_CIRCLE 3
#define PENTAGRAMM_SQUARE 4
#define N 10000

typedef struct Coords{
    int x1 = 0;
    int x2 = 0;
    int y1 = 0;
    int y2 = 0;
}Coords;

// Расширяем класс QGraphicsView
class MyGraphicView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MyGraphicView(QWidget *parent = nullptr);
    void update(QPixmap pixmap);
    void deleteItemsFromGroup(QGraphicsItemGroup *group);
    Coords coords;
    QGraphicsScene* scene;
    int button_pressed = 0;


signals:
    void selection();

private slots:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:
    QPixmap pixmap;

    QGraphicsItemGroup* group;
    bool mouse_pressed = false;
};

#endif // MYGRAPHICVIEW_H

#include "mygraphicview.h"
#include <iostream>
#include <QMessageBox>
#include <QString>
#define N 10000
#define B 1000
#define E 0.004
MyGraphicView::MyGraphicView(QWidget* parent):QGraphicsView (parent){
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    group = new QGraphicsItemGroup();
}

void MyGraphicView::update(QPixmap pixmap){
    scene = new QGraphicsScene();
    scene->addPixmap(pixmap);
    this->setScene(scene);
}

void MyGraphicView::mousePressEvent(QMouseEvent *event){
    QPoint position = mapToScene(event->pos()).toPoint();
    coords.x1 = position.x();
    coords.y1 = position.y();
    mouse_pressed = true;
}

void MyGraphicView::mouseReleaseEvent(QMouseEvent *event){
    QPoint position = mapToScene(event->pos()).toPoint();
    coords.x2 = position.x();
    coords.y2 = position.y();
    mouse_pressed = false;
    button_pressed = 0;
    this->deleteItemsFromGroup(group);
    emit selection();
}

void MyGraphicView::mouseMoveEvent(QMouseEvent* event){
    QPoint position = mapToScene(event->pos()).toPoint();
    int x = position.x();
    int y = position.y();
    this->deleteItemsFromGroup(group);
    group = new QGraphicsItemGroup();
    QPen penBlack(Qt::black);
    if (x < 0){
        x = 0;
    }
    if (y < 0){
        y = 0;
    }
    int x1 = coords.x1;
    int y1 = coords.y1;
    if (mouse_pressed && (button_pressed == RECT || button_pressed == REFLECTION || button_pressed == CROP)){
        group->addToGroup(scene->addLine(x1, y1, x, y1, penBlack));
        group->addToGroup(scene->addLine(x, y1, x, y, penBlack));
        group->addToGroup(scene->addLine(x, y, x1, y, penBlack));
        group->addToGroup(scene->addLine(x1, y, x1, y1, penBlack));
        scene->addItem(group);
    }
    else if (mouse_pressed && button_pressed == PENTAGRAMM_SQUARE){
        //int x2, y2;
        int a = (abs(x - x1) > abs(y - y1)) ? abs(x - x1) : abs(y - y1);
        if (x >= x1 && y >= y1){
            group->addToGroup(scene->addRect(x1, y1, a, a, penBlack));
            group->addToGroup(scene->addEllipse(x1, y1, a, a));
        }
        /*
        else if (x <= x1 && y <= y1){
            group->addToGroup(scene->addRect(x1 - a, y1 - a, a, a, penBlack));
            group->addToGroup(scene->addEllipse(x1 - a, y1 - a, a, a));
        }
        else if (x > x1 && y < y1){
            group->addToGroup(scene->addRect(x1, y1 - a, a, a, penBlack));
            group->addToGroup(scene->addEllipse(x1, y1 - a, a, a));
        }
        else if (x < x1 && y > y1){
            group->addToGroup(scene->addRect(x1 - a, y1, a, a, penBlack));
            group->addToGroup(scene->addEllipse(x1 - a, y1, a, a));
        }
        */
        scene->addItem(group);
    }
    else if (mouse_pressed && button_pressed == PENTAGRAMM_CIRCLE){
        int r = int(sqrt(pow((x-x1), 2) + pow((y-y1), 2)));
        int x_t, y_t;
        for (double a = 0; a < B * 4; a += 4){
            x_t = static_cast<int>((r * cos(a/r)));
            y_t = static_cast<int>((r * sin(a/r)));
            group->addToGroup(scene->addLine(x1 + x_t - 2, y1 + y_t - 2, x1 + x_t, y1 + y_t, penBlack));
        }
        scene->addItem(group);
    }
}

void MyGraphicView::deleteItemsFromGroup(QGraphicsItemGroup *group){
    foreach(QGraphicsItem *item, scene->items(group->boundingRect())) {
       if(item->group() == group ) {
          delete item;
       }
    }
}

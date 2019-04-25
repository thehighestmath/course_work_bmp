#ifndef REFERENCE_H
#define REFERENCE_H

#include <QDialog>
#include <QLabel>
namespace Ui {
class Reference;
}

class Reference : public QDialog
{
    Q_OBJECT

public:
    explicit Reference(QWidget *parent = nullptr);
    ~Reference();

private:
    Ui::Reference *ui;
};

#endif // REFERENCE_H

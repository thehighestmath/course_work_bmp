#ifndef INFORMATION_H
#define INFORMATION_H

#include <QDialog>

typedef struct Info_s{
  QString name;
  QString path;
  int version;
  int heigth;
  int width;
  int weight;
  bool is_readable;
  bool is_writeable;
}Info_s;

namespace Ui {
class Information;
}

class Information : public QDialog
{
    Q_OBJECT

public:
    explicit Information(QWidget *parent = nullptr);
    ~Information();
    Info_s info_s;
    void write_information();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Information *ui;
};

#endif // INFORMATION_H

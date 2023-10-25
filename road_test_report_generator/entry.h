#ifndef ENTRY_H
#define ENTRY_H

#include <QWidget>

namespace Ui {
class entry;
}

class entry : public QWidget
{
    Q_OBJECT

public:
    explicit entry(QWidget *parent = nullptr);
    ~entry();


private:
    Ui::entry *ui;
};

#endif // ENTRY_H

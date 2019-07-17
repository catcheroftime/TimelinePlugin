#ifndef ADDTIMECELL_H
#define ADDTIMECELL_H

#include <QDialog>

namespace Ui {
class AddTimeCell;
}

class AddTimeCell : public QDialog
{
    Q_OBJECT

public:
    explicit AddTimeCell(QWidget *parent = 0);
    ~AddTimeCell();


    QTime getStartTime();
    QTime getEndTime();
    QString getColor();

private slots:
    void on_ptn_sure_clicked();

private:
    Ui::AddTimeCell *ui;
};

#endif // ADDTIMECELL_H

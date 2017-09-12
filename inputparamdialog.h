#ifndef INPUTPARAMDIALOG_H
#define INPUTPARAMDIALOG_H

#include <QDialog>
#include <QPoint>


namespace Ui {
class InputParamDialog;
}

class InputParamDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InputParamDialog(QWidget *parent = 0);
    ~InputParamDialog();

private:
    Ui::InputParamDialog *ui;
    std::vector<QPoint> src_point_vector;
    std::vector<QPoint> target_point_vector;
};

#endif // INPUTPARAMDIALOG_H

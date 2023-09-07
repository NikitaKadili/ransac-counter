#include "addpointdialog.h"
#include "ui_addpointdialog.h"

#include <QIntValidator>

AddPointDialog::AddPointDialog(QWidget* parent, int* x, int* y)
    : QDialog(parent)
    , ui_(new Ui::AddPointDialog)
    , x_(x)
    , y_(y)
{
    ui_->setupUi(this);
    this->setWindowTitle("Добавить точку");

    SetValidator();
}

AddPointDialog::~AddPointDialog() {
    delete ui_;
}

void AddPointDialog::accept() {
    // Обновляем значения X и Y
    *x_ = ui_->x_edit->text().toInt();
    *y_ = ui_->y_edit->text().toInt();

    // Возвращаем соответствующий код завершения
    this->done(this->Accepted);
}

void AddPointDialog::SetValidator() {
    QIntValidator* int_val = new QIntValidator(this);
    ui_->x_edit->setValidator(int_val);
    ui_->y_edit->setValidator(int_val);
}

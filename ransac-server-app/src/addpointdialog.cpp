#include "addpointdialog.h"
#include "ui_addpointdialog.h"

#include <QIntValidator>

AddPointDialog::AddPointDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::AddPointDialog)
{
    ui_->setupUi(this);
    SetValidators();
}

AddPointDialog::~AddPointDialog() {
    delete ui_;
}

void AddPointDialog::accept() {
    int x = ui_->x_edit->text().toInt();
    int y = ui_->y_edit->text().toInt();

    emit signalAddPoint(x, y);
    close();
}

void AddPointDialog::SetValidators() {
    QIntValidator* int_val = new QIntValidator(this);
    ui_->x_edit->setValidator(int_val);
    ui_->y_edit->setValidator(int_val);
}

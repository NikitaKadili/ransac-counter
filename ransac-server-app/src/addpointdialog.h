#pragma once

#include <QDialog>

namespace Ui {
class AddPointDialog;
}

class AddPointDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddPointDialog(QWidget* parent = nullptr, int* = nullptr, int* = nullptr);

    ~AddPointDialog();

private slots:
    // Слот вызывается, когда нажата кнопка "OK"
    void accept();

private:
    Ui::AddPointDialog *ui_;

    int* x_; // Указатель на переменную значения X
    int* y_; // Указатель на переменную значения Y

    // Метод устанавливает необходимый валидатор на текстовые поля
    void SetValidator();
};

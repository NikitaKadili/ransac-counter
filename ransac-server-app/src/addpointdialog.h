#pragma once

#include <QDialog>

namespace Ui {
class AddPointDialog;
}

class AddPointDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddPointDialog(QWidget *parent = nullptr);

    ~AddPointDialog();

signals:
    // Сигнал отправляет координаты X и Y новой точки,
    // внесенные через форму класса
    void signalAddPoint(int, int);

public slots:
    // Слот вызывается, когда нажата кнопка "OK"
    void accept();

private:
    Ui::AddPointDialog *ui_;

    // Метод устанавливает необходимые валидаторы на текстовые поля
    void SetValidators();
};

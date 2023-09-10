#pragma once

#include <cstdint>

#include <QDialog>

namespace Ui {
class EditPortDialog;
}

class EditPortDialog : public QDialog {
    Q_OBJECT
public:
    // Конструктор принимает указатель на объект родителя,
    // указатель на объект со значение текущего порта
    explicit EditPortDialog(QWidget* parent = nullptr, uint16_t* port = nullptr);

    ~EditPortDialog();

private slots:
    // Слот проверяет корректность ввода данных. Если все корректно -
    // обновляет соответствующие поля класса, иначе - выводит сообщение
    // об ошибке, выделяет ошибочное поле
    void slotValueChanged(const QString&);

    // Слот срабатывает, если нажата клавиша "OK"
    void accept();

private:
    Ui::EditPortDialog* ui_;

    uint16_t* port_;  // Указатель на переменную порта
    bool is_correct_; // Сигнализирует о том, корретны введенные данные или нет

    // Метод соединяет сигналы и соответствующие им слоты
    void ConnectSignalsAndSlots();
};

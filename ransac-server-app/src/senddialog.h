#pragma once

#include <QDialog>

#include "sender.h"

namespace Ui {
class SendDialog;
}

class SendDialog : public QDialog {
    Q_OBJECT
public:
    explicit SendDialog(QWidget* parent = nullptr, ransac::SenderSettings* = nullptr);

    ~SendDialog();

private slots:
    // Слот проверяет правильность введенных данных, заносит их,
    // либо выводит сообщение об ошибке и выделяет соответствующую графу
    void slotValueChanged(const QString&);

    // Слот вызывается при нажатии клавиши "OK". Если все данные введены
    // корректно - успешно завершает выполнение с кодом Accepted
    void accept();

private:
    Ui::SendDialog *ui_;

    ransac::SenderSettings* settings_; // Указатель на настройки отправщика
    bool is_data_correct_ = false;     // Верны или нет данные, введенные пользоваталем

    // Метод соединяет сигнали и соответствующие им слоты
    void ConnectSignalsAndSlots();

    // Метод задает полям значения из переданного конструктору набора настроек
    void UploadFromSettings();
};

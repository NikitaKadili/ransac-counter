#pragma once

#include <QDialog>

#include "ransac.h"

namespace Ui {
class CounterSettingsDialog;
}

class CounterSettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit CounterSettingsDialog(QWidget* parent = nullptr,
                                   ransac::Settings* settings = nullptr,
                                   size_t total_points_num = 0);

    ~CounterSettingsDialog();

private slots:
    // Слот реагирует на изменения величин слайдеров,
    // обновляет информацию в соответствующих лэйблах
    void slotValueChanged(int);

    // Слот обновляет доступность параметров в зависимости от того,
    // выбраны соответствующие флаги "Автоматически" или нет
    void slotUpdateAvaliability(int);

    // Слот возвращает код Accepted при нажатии на клавишу "OK"
    void accept();

private:
    Ui::CounterSettingsDialog* ui_;

    ransac::Settings* settings_; // Указатель на настройки RANSAC-алгоритма

    // Метод соединяет сигналы с соответствующими слотами
    void ConnectSlotsAndSignals();

    // Метод считывает данные из settings, передает их на форму
    void UploadFromSettings(size_t);
};

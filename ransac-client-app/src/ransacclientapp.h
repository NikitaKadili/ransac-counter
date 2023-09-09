#pragma once

#include <cstdint>

#include <QMainWindow>
#include <QUdpSocket>

#include "client.h"
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RansacClientApp; }
QT_END_NAMESPACE

class RansacClientApp : public QMainWindow {
    Q_OBJECT
public:
    RansacClientApp(QWidget* parent = nullptr);

    ~RansacClientApp();

private slots:
    // Слот принимает данные, распарсенные клиентом,
    // переносит их на форму
    void slotRecieveData(ransac::DataToRecieve);

    // Слот вызывает окно изменения порта. Если введено корректное значение -
    // вносит его в поле класса, меняет порт у объекта client_
    void slotPortButtonPressed();

private:
    Ui::RansacClientApp* ui_;
    ransac::Client* client_;

    ransac::LineFormula formula_; // Функция прямой
    int32_t min_x_ = 0;           // Минимальное значение X
    int32_t max_x_ = 0;           // Максимальное значение X

    // Метод соединяет сигналы и соответствующие слоты
    void ConnectSignalsAndSlots();

    // Метод отрисовывает функцию прямой на графике
    void DrawGraphic();

    // Метод возвращает строку с формулой
    QString GetFormulaString() const;
};

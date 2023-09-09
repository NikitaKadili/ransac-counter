#pragma once

#include <cstdint>

#include <QObject>
#include <QUdpSocket>

namespace ransac {

// Порт по умолчанию
const uint16_t DEFAULT_PORT = 1212;

// Структура представляет линейную формулу типа: y(x) = ax + b
struct LineFormula {
    double a = 0;
    double b = 0;

    // Метод возвращает значение y(x)
    double GetY(int) const;
};
// Структура представляет пакет данных, получаемых от сервера,
// содержащий функцию прямой и диапозон по оси X
struct DataToRecieve {
    LineFormula formula; // Функция результирующей прямой

    int32_t min_x; // Минимальное значение X
    int32_t max_x; // Максимальное значение X
};

class Client : public QObject {
    Q_OBJECT
public:
    Client(QObject* parent = nullptr);

    // Метод меняет просматриваемый порт на переданный
    void ChangePort(uint16_t);

    // Метод возвращает значение текущего порта
    uint32_t GetCurrentPort() const;

signals:
    // Сигнал передает принятую формулу прямой
    void signalDataProceed(ransac::DataToRecieve);

private slots:
    // Слот обрабатывает
    void slotDataIncoming();

private:
    QUdpSocket* udp_socket_; // Указатель на сокет

    uint16_t port_; // Просматриваемый порт

    // Метод объединяет сигналы с соответствующими им слотами
    void ConnectSignalsAndSlots();
};

} // namespace ransac

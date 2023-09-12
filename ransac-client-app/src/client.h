#pragma once

#include <cstdint>
#include <string>

#include <QObject>
#include <QUdpSocket>

namespace ransac {

// Порт по умолчанию
const uint16_t DEFAULT_PORT = 1212;

// Структура представляет линейную формулу типа: y(x) = ax + b
struct LineFormula {
    double a = 0.0;
    double b = 0.0;

    // Метод возвращает значение y(x)
    double GetY(int) const;
};

// Структура представляет пакет данных, получаемых от сервера,
// содержащий функцию прямой и диапозон по оси X
struct DataToRecieve {
    LineFormula formula; // Функция результирующей прямой

    int32_t min_x = 0; // Минимальное значение X
    int32_t max_x = 0; // Максимальное значение X
};

class Client : public QObject {
    Q_OBJECT
public:
    Client(QObject* parent = nullptr);

    // Метод меняет текущий просматриваемый порт на новый
    void ChangePort(uint16_t);

    // Метод возвращает значение текущего порта
    uint32_t GetCurrentPort() const;

signals:
    // Сигнал передает принятую формулу прямой
    void signalDataProceed(ransac::DataToRecieve);

private slots:
    // Слот принимает и парсит входящие данные
    void slotDataIncoming();

private:
    QUdpSocket* udp_socket_; // Указатель на сокет
    uint16_t port_;          // Просматриваемый порт

    // Метод объединяет сигналы с соответствующими им слотами
    void ConnectSignalsAndSlots();

    // Метод десериализует данные из proto в структуру DataToRecieve
    DataToRecieve DeserializeProto(const std::string& ser_str) const;
};

} // namespace ransac

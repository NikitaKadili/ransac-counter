#pragma once

#include <cstdint>

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

#include <data_message.pb.h>

#include "ransac.h"

namespace ransac {

// Структура представляет пакет с необходимыми для клиента данными,
// которые будут сериализованы и отправлены
struct DataToSend {
    LineFormula formula; // Функция результирующей прямой

    int32_t min_x = 0; // Минимальное значение X
    int32_t max_x = 0; // Максимальное значение X
};

// Структура представляет набор настроек для отправления
// результата вычислений RANSAC-алгоритма
struct SenderSettings {
    QHostAddress address = QHostAddress("127.0.0.1"); // Адрес клиента
    uint16_t port = 1212;                             // Порт клиента
};

class Sender : public QObject {
    Q_OBJECT
public:
    Sender(QObject* parent = nullptr);

    // Метод отправляет датаграмму со значениями результирующей
    // модели и диапозоном по оси X на переданный адрес клиента
    void SendData(const SenderSettings&, const DataToSend&) const;

private:
    QUdpSocket* udp_socket_; // Указатель на UDP-сокет

    // Метод сериализует данные из структуры DataToSend в сообщение,
    // описанное в data_message.proto
    ransac_data::Data *SerializeDataToProto(const DataToSend&) const;
};

} // namespace ransac

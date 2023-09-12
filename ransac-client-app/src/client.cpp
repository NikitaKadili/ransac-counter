#include "client.h"

#include <QByteArray>

#include <data_message.pb.h>

namespace ransac {

Client::Client(QObject* parent)
    : QObject(parent)
    , udp_socket_(new QUdpSocket(this))
    , port_(DEFAULT_PORT)
{
    udp_socket_->bind(port_);
    ConnectSignalsAndSlots();
}

void Client::ChangePort(uint16_t new_port) {
    port_ = new_port;

    udp_socket_->abort();
    udp_socket_->bind(port_);
}

uint32_t Client::GetCurrentPort() const { return port_; }

void Client::slotDataIncoming() {
    QByteArray data;

    do {
        data.resize(udp_socket_->pendingDatagramSize());
        udp_socket_->readDatagram(data.data(), data.size());
    } while (udp_socket_->hasPendingDatagrams());

    std::string ser_str(data.constData(), data.size());
    emit(signalDataProceed(DeserializeProto(ser_str)));
}

void Client::ConnectSignalsAndSlots() {
    // Получение датаграммы сокетом
    connect(udp_socket_, SIGNAL(readyRead()),
            SLOT(slotDataIncoming()));
}

DataToRecieve Client::DeserializeProto(const std::string &ser_str) const {
    ransac_data::Data data;

    // Если не удалось распарсить - возвращаем пустую структуру
    if (!data.ParseFromString(ser_str)) {
        return {};
    }

    // Иначе - возвращаем полученные данные
    return {
        { data.formula().a(), data.formula().b() },
        data.min_x(),
        data.max_x()
    };
}

double LineFormula::GetY(int x) const {
    return x * static_cast<double>(a) + b;
}

} // namespace ransac

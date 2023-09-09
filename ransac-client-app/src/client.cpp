#include "client.h"

#include <QByteArray>
#include <QDataStream>

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

    QDataStream in(&data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_15);

    do {
        data.resize(udp_socket_->pendingDatagramSize());
        udp_socket_->readDatagram(data.data(), data.size());
    } while (udp_socket_->hasPendingDatagrams());

    DataToRecieve recieved_data;
    in >> recieved_data.formula.a
            >> recieved_data.formula.b
            >> recieved_data.min_x
            >> recieved_data.max_x;

    emit(signalDataProceed(recieved_data));
}

void Client::ConnectSignalsAndSlots() {
    // Получение датаграммы сокетом
    connect(udp_socket_, SIGNAL(readyRead()),
            SLOT(slotDataIncoming()));
}

double LineFormula::GetY(int x) const {
    return x * static_cast<double>(a) + b;
}

} // namespace ransac

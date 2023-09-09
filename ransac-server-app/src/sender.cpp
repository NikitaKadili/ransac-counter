#include "sender.h"

#include <QDataStream>

namespace ransac {

Sender::Sender(QObject* parent)
    : QObject(parent)
    , udp_socket_(new QUdpSocket(this))
{ /* do nothing */ }

void Sender::SendData(const SenderSettings& settings, const DataToSend& data_pack) const {
    QByteArray data;

    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);

    out << data_pack.formula.a << data_pack.formula.b << data_pack.min_x << data_pack.max_x;
    udp_socket_->writeDatagram(data, settings.address, settings.port);
}

} // namespace ransac

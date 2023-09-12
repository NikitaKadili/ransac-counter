#include "sender.h"

namespace ransac {

Sender::Sender(QObject* parent)
    : QObject(parent)
    , udp_socket_(new QUdpSocket(this))
{ /* do nothing */ }

void Sender::SendData(const SenderSettings& settings, const DataToSend& data_pack) const {
    QByteArray data;

    // Сериализуем данные
    auto* serialized_data = SerializeDataToProto(data_pack);
    // Записываем сериализованные данные в std::string,
    // чтобы перенести их в объект типа QByteArray
    std::string temp_str = serialized_data->SerializeAsString();
    data = QByteArray(temp_str.c_str(), temp_str.size());

    udp_socket_->writeDatagram(data, settings.address, settings.port);

    delete serialized_data;
}

ransac_data::Data* Sender::SerializeDataToProto(const DataToSend& data_pack) const {
    ransac_data::Data* result_msg = new ransac_data::Data();

    // Сериализуем формулу
    ransac_data::Formula* formula_msg = result_msg->mutable_formula();
    formula_msg->set_a(data_pack.formula.a);
    formula_msg->set_b(data_pack.formula.b);

    // Сериализуем минимальное и максимальное значения X
    result_msg->set_min_x(data_pack.min_x);
    result_msg->set_max_x(data_pack.max_x);

    return result_msg;
}

} // namespace ransac

#include "senddialog.h"
#include "ui_senddialog.h"

#include <limits>

SendDialog::SendDialog(QWidget* parent, ransac::SenderSettings* settings)
    : QDialog(parent)
    , ui_(new Ui::SendDialog)
    , settings_(settings)
{
    ui_->setupUi(this);

    // Вносим на форму данные из settings_
    UploadFromSettings();
    // Проверяем их корректность
    slotValueChanged("");

    ConnectSignalsAndSlots();
}

SendDialog::~SendDialog() {
    delete ui_;
}

void SendDialog::slotValueChanged(const QString&) {
    // Очищаем предшествующие предупреждения
    ui_->warning_lbl->clear();

    QString warning_style = "border: 1px solid red;"
                "padding-top: 2px;"
                "padding-bottom: 2px;"
                "border-radius: 2px";

    is_data_correct_ = true;
    // Проверяем правильность ввода IP-адреса.
    // Если введен верно - обновляем его в settings_,
    // иначе - выделаем поле, выводим сообщение об ошибке
    QHostAddress new_ip;
    if (new_ip.setAddress(ui_->ip_edit->text())) {
        ui_->ip_edit->setStyleSheet("");
        settings_->address = std::move(new_ip);
    }
    else {
        ui_->ip_edit->setStyleSheet(warning_style);
        ui_->warning_lbl->setText("Некорректно указан IP-адрес");

        is_data_correct_ = false;
    }

    // Проверяем корректность ввода порта.
    // Если введен верно - обновляем его в settings_,
    // иначе - выделаем поле, выводим сообщение об ошибке
    uint new_port = ui_->port_edit->text().toUInt();
    if (new_port <= std::numeric_limits<uint16_t>::max()) {
        ui_->port_edit->setStyleSheet("");
        settings_->port = new_port;
    }
    else {
        ui_->port_edit->setStyleSheet(warning_style);

        // Если предупреждение уже имеется - ставим точку и пробел между ними
        if (!ui_->warning_lbl->text().isEmpty()) {
            ui_->warning_lbl->setText(ui_->warning_lbl->text() + ". ");
        }
        ui_->warning_lbl->setText(ui_->warning_lbl->text() + "Некорректно указан порт");

        is_data_correct_ = false;
    }
}

void SendDialog::accept() {
    // Если введены некорректные данные - выходим из слота
    if (!is_data_correct_) {
        return;
    }

    this->done(this->Accepted);
}

void SendDialog::ConnectSignalsAndSlots() {
    // Изменение данных
    connect(ui_->ip_edit, SIGNAL(textChanged(const QString&)),
            SLOT(slotValueChanged(const QString&)));
    connect(ui_->port_edit, SIGNAL(textChanged(const QString&)),
            SLOT(slotValueChanged(const QString&)));
}

void SendDialog::UploadFromSettings() {
    ui_->ip_edit->setText(settings_->address.toString());
    ui_->port_edit->setText(QString::number(settings_->port));
}

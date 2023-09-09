#include "editportdialog.h"
#include "ui_editportdialog.h"

#include <limits>

EditPortDialog::EditPortDialog(QWidget *parent, uint16_t* port)
    : QDialog(parent)
    , ui_(new Ui::EditPortDialog)
    , port_(port)
{
    ui_->setupUi(this);

    // Вносим текущее значение в текстовое поле
    ui_->port_edit->setText(QString::number(*port_));
    slotValueChanged("");

    ConnectSignalsAndSlots();
}

EditPortDialog::~EditPortDialog() {
    delete ui_;
}

void EditPortDialog::slotValueChanged(const QString&) {
    QString warning_style = "border: 1px solid red;"
                "padding-top: 2px;"
                "padding-bottom: 2px;"
                "border-radius: 2px";

    ui_->warning_lbl->clear();
    is_correct_ = true;

    // Считываем новое значение
    uint new_port = ui_->port_edit->text().toUInt();
    // Если значение некорректное - выделяем поле красным,
    // выводим соответствующее сообщение
    if (new_port <= std::numeric_limits<uint16_t>::max()) {
        ui_->port_edit->setStyleSheet("");
        *port_ = new_port;
    }
    // Иначе - очищаем стиль, обновляем значение по указателю point_
    else {
        ui_->port_edit->setStyleSheet(warning_style);
        ui_->warning_lbl->setText("Некорректное значение порта");

        is_correct_ = false;
    }
}

void EditPortDialog::accept() {
    if (!is_correct_) {
        return;
    }

    this->done(this->Accepted);
}

void EditPortDialog::ConnectSignalsAndSlots() {
    // Изменение данных в поле ввода
    connect(ui_->port_edit, SIGNAL(textChanged(const QString&)),
            SLOT(slotValueChanged(const QString&)));
}

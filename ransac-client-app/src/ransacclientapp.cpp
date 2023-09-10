#include "ransacclientapp.h"
#include "./ui_ransacclientapp.h"

#include "editportdialog.h"

#include <QDataStream>
#include <QDebug>
#include <QGraphicsView>

RansacClientApp::RansacClientApp(QWidget* parent)
    : QMainWindow(parent)
    , ui_(new Ui::RansacClientApp)
    , client_(new ransac::Client(this))
{
    ui_->setupUi(this);

    // Добавим возможность увеличивать/уменьшать график, перемещать его
    ui_->graph_wgt->setInteraction(QCP::iRangeZoom, true);
    ui_->graph_wgt->setInteraction(QCP::iRangeDrag, true);

    // Задаем диапозон видимости от -1500 до 1500 для осей X и Y
    ui_->graph_wgt->xAxis->setRange(-1500, 1500);
    ui_->graph_wgt->yAxis->setRange(-1500, 1500);

    // Выводим порт по умолчанию на форму
    ui_->port_lbl->setText("Текущий порт: " +
                           QString::number(client_->GetCurrentPort()));

    ConnectSignalsAndSlots();
}

RansacClientApp::~RansacClientApp() {
    delete ui_;
}

void RansacClientApp::slotRecieveData(ransac::DataToRecieve data) {
    // Вносим результат в поля класса
    formula_ = std::move(data.formula);
    min_x_ = data.min_x;
    max_x_ = data.max_x;

    // Вносим функцию в текстовое поле
    ui_->formula_edit->setText(GetFormulaString());
    // Отрисовываем прямую на графике
    DrawGraphic();
}

void RansacClientApp::slotPortButtonPressed() {
    uint16_t new_port = client_->GetCurrentPort();

    // Создаем окно изменения порта
    EditPortDialog* edit_dialog = new EditPortDialog(this, &new_port);
    // Если окно было закрыто без нажатия "OK" - выходим из слота
    if (edit_dialog->exec() == edit_dialog->Rejected) {
        return;
    }

    // Обновляем порт
    client_->ChangePort(new_port);
    // Вносим текст в соответствующее поле формы
    ui_->port_lbl->setText("Текуший порт: " + QString::number(new_port));

    delete edit_dialog;
}

void RansacClientApp::ConnectSignalsAndSlots() {
    // Сигнал получения данных
    connect(client_, SIGNAL(signalDataProceed(ransac::DataToRecieve)),
            SLOT(slotRecieveData(ransac::DataToRecieve)));

    // Сигнал нажатия кнопки "Изменить порт"
    connect(ui_->port_btn, SIGNAL(pressed()),
            SLOT(slotPortButtonPressed()));
}

void RansacClientApp::DrawGraphic() {
    // Создаем график, если он не был создан
    if (ui_->graph_wgt->graphCount() == 0) {
        // График 0 - для прямой
        ui_->graph_wgt->addGraph();

        ui_->graph_wgt->graph(0)->setPen(QColor(150, 150, 250));
        ui_->graph_wgt->graph(0)->pen().setWidth(3);
        ui_->graph_wgt->graph(0)->setLineStyle(QCPGraph::lsLine);
    }
    // Очищаем график
    ui_->graph_wgt->graph(0)->data().data()->clear();

    // Вносим точки прямой
    ui_->graph_wgt->graph(0)->addData(min_x_, formula_.GetY(min_x_));
    ui_->graph_wgt->graph(0)->addData(max_x_, formula_.GetY(max_x_));

    // Обновляем холст
    ui_->graph_wgt->replot();
}

QString RansacClientApp::GetFormulaString() const {
    QString formula_str = QString::number(formula_.a) + "* a ";
    if (formula_.b < 0) {
        formula_str += '-';
    }
    else {
        formula_str += '+';
    }

    return formula_str + QString::number(formula_.b);
}

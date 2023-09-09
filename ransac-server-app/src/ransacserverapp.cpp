#include "ransacserverapp.h"
#include "./ui_ransacserverapp.h"

#include "addpointdialog.h"
#include "countersettingsdialog.h"
#include "senddialog.h"

#include <chrono>
#include <regex>
#include <string>

#include <QFileDialog>
#include <QGraphicsView>
#include <QSharedPointer>

RansacServerApp::RansacServerApp(QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::RansacServerApp)
{
    ui_->setupUi(this);

    // Добавим возможность увеличивать/уменьшать график, перемещать его
    ui_->graph_wgt->setInteraction(QCP::iRangeZoom, true);
    ui_->graph_wgt->setInteraction(QCP::iRangeDrag, true);

    // Задаем диапозон видимости от -1500 до 1500 для осей X и Y
    ui_->graph_wgt->xAxis->setRange(-1500, 1500);
    ui_->graph_wgt->yAxis->setRange(-1500, 1500);

    ConnectClotsAndSignals();
}

RansacServerApp::~RansacServerApp() {
    delete ui_;
}

void RansacServerApp::slotAddPointCalled() {
    int x = 0;
    int y = 0;

    // Создаем объект класса окна для добавления точки (AddPointDialog),
    // передаем указатели на x и y, открываем окно
    AddPointDialog* add_point_window = new AddPointDialog(this, &x, &y);

    // Если окно было закрыто (не нажата кнопка "ОК") - выходим из метода
    if (add_point_window->exec() == add_point_window->Rejected) {
        return;
    }

    // Добавляем полученные координаты
    AddPoint({ x, y });

    // Обновляем холст
    ui_->graph_wgt->replot();
    // Обновляем кнопки
    UpdateButtonsEnability();

    delete add_point_window;
}

void RansacServerApp::slotUploadFromFile() {
    // Получаем имя необходимого файла
    QString file_name = QFileDialog::getOpenFileName(this,
                                                     tr("Открыть файл"),
                                                     "./",
                                                     "Текстовый файл (*.txt)");

    // Пробуем открыть файл
    std::ifstream input(file_name.toStdString());

    // Если возникла ошибка при открытии файла -
    // уведомим пользователя в statusBar, выйдем из метода
    if (!input) {
        this->statusBar()->showMessage("Возникла ошибка при открытии файла");
        return;
    }

    // Удаляем ранее добавленные точки
    points_.clear();
    ui_->points_list->clear();
    // Если на холсте существует график - очищаем его
    if (ui_->graph_wgt->graphCount() != 0) {
        ui_->graph_wgt->graph(0)->data().data()->clear();
    }

    // В случае удачи - итерируемся по распарсенным координатам, добавляем их
    for (ransac::Point& p : GetPointsFromFile(input)) {
        AddPoint(std::move(p));
    }
    input.close();

    // Обновляем холст
    ui_->graph_wgt->replot();
    // Обновляем кнопки
    UpdateButtonsEnability();

    // Выведем в statusBar соответствующее сообщение
    this->statusBar()->showMessage(QString::number(points_.size())
                                   + " точек были загружено из файла");
}

void RansacServerApp::slotUnlockButtons(QListWidgetItem*) {
    UpdateButtonsEnability();
}

void RansacServerApp::slotCountRansacModel() {
    // Создаем объект класса окна указания преднастроек,
    // передаем указатель на настройки
    CounterSettingsDialog* counter_settings =
            new CounterSettingsDialog(this, &ransac_settings_, points_.size());

    // Если ввод данных был прерван - ничего не делаем
    if (counter_settings->exec() == counter_settings->Rejected) {
        return;
    }

    // Создаем объект класса RansacCounter, передаем набор точек и ссылку на указатель
    ransac::RansacCounter* counter = new ransac::RansacCounter(points_, ransac_settings_);

    // Производим вычисления в соответствии с выбранным пользователем
    // режимом (асинхронный, обычный), замеряем потраченное на это время
    using namespace std::chrono;
    time_point start_time = steady_clock::now();
    ransac::RansacResult ransac_result;
    if (ransac_settings_.is_async_required) {
        ransac_result = counter->AsyncCount();
    }
    else {
        ransac_result = counter->Count();
    }
    duration spent_time = steady_clock::now() - start_time;

    // Выводим в statusBar формулу результирующей прямой и затраченное время
    this->statusBar()->showMessage(
                "Формула: "
               + QString::number(ransac_result.first.a) + "*x + "
               + QString::number(ransac_result.first.b) + ". "
               "Потрачено времени: "
               + QString::number(duration_cast<milliseconds>(spent_time).count())
               + " ms");

    // Включаем режим подсчета, обновляем доступность кнопок
    is_countmode_on_ = true;
    UpdateButtonsEnability();

    // Отрисовываем получившуюся прямую
    DrawRansacResults(ransac_result, counter->GetMinX(), counter->GetMaxX());

    // Вносим данные в соответствующее поле класса
    // для будущей отправки по UDP-протоколу
    result_data_ = {
        std::move(ransac_result.first), counter->GetMinX(), counter->GetMaxX()
    };

    delete counter_settings;
    delete counter;
}

void RansacServerApp::slotResetButtonPressed() {
    // Отключаем режим подсчета
    is_countmode_on_ = false;

    // Очищаем графики с результатами подсчетов
    ui_->graph_wgt->graph(1)->data().data()->clear();
    ui_->graph_wgt->graph(2)->data().data()->clear();
    ui_->graph_wgt->graph(3)->data().data()->clear();

    // Делаем видимым основной график
    ui_->graph_wgt->graph(0)->setVisible(true);

    // Обновляем холст
    ui_->graph_wgt->replot();

    // Обновляем доступность кнопок
    UpdateButtonsEnability();
}

void RansacServerApp::slotSendButtonPressed() {
    // Создаем и открываем окно настроек для отправки данных
    SendDialog* send_dial = new SendDialog(this, &sender_settings_);
    // Если окно завершилось с неверным кодом - выходим из слота
    if (send_dial->exec() == send_dial->Rejected) {
        return;
    }

    // Создаем отправщик, передаем данные для отправки датаграммы
    ransac::Sender* sender = new ransac::Sender(this);
    sender->SendData(sender_settings_, result_data_);

    delete sender;
    delete send_dial;
}

void RansacServerApp::slotSaveToFile() {
    // Если множество координат пусто - выходим из метода
    if (points_.empty()) {
        return;
    }

    // Получаем адрес папки, создаем соответствующий поток вывода
    QString dir_to_save = QFileDialog::getExistingDirectory(this,
                                            tr("Выбрать папку"),
                                            "./",
                                            QFileDialog::ShowDirsOnly
                                                            | QFileDialog::DontResolveSymlinks);
    std::ofstream output(dir_to_save.toStdString() + "/points.txt", std::ios::trunc);

    // Если не получилось создать/открыть файл для записи -
    // сообщаем об ошибке в statusBar, выходим из метода
    if (!output) {
        this->statusBar()->showMessage("Ошибка создания/открытия файла для записи");
        return;
    }

    // Записываем координаты в поток вывода
    WritePointsToFile(output);

    output.close();
    this->statusBar()->showMessage("Координаты были сохранены");
}

void RansacServerApp::slotRemovePoint() {
    int index_to_remove = ui_->points_list->currentIndex().row();

    // Если ничего не выбрано - выходим метода
    if (index_to_remove < 0) {
        return;
    }

    // Удаляем точку из всех списков
    RemovePoint(index_to_remove);

    // Перерисовываем актуальные точки
    RedrawPoints();
    // Обновляем кнопки
    UpdateButtonsEnability();
}

void RansacServerApp::ConnectClotsAndSignals() {
    // Сигнал нажатия кнопки "Загрузить из файла"
    connect(ui_->fromfile_btn, SIGNAL(pressed()),
            SLOT(slotUploadFromFile()));
    // Сигнал нажатия на кнопку "Сохранить в файл"
    connect(ui_->save_btn, SIGNAL(pressed()),
            SLOT(slotSaveToFile()));

    // Сигнал нажатия на элемент списка с точками
    connect(ui_->points_list, SIGNAL(itemPressed(QListWidgetItem*)),
            SLOT(slotUnlockButtons(QListWidgetItem*)));

    // Cигнал нажатия кнопки "Добавить"
    connect(ui_->add_btn, SIGNAL(pressed()),
            SLOT(slotAddPointCalled()));
    // Сигнал нажатия на кнопку "Удалить"
    connect(ui_->remove_btn, SIGNAL(pressed()),
            SLOT(slotRemovePoint()));

    // Сигнал нажатия на кнопку "Рассчитать"
    connect(ui_->count_btn, SIGNAL(pressed()),
            SLOT(slotCountRansacModel()));

    // Сигнал нажатия на кнопку "Сбросить вычисления"
    connect(ui_->reset_btn, SIGNAL(pressed()),
            SLOT(slotResetButtonPressed()));

    // Сигнал нажатия на кнопку "Отправить"
    connect(ui_->send_btn, SIGNAL(pressed()),
            SLOT(slotSendButtonPressed()));
}

void RansacServerApp::AddPoint(ransac::Point point) {
    // Если на холсте отсутствует график - создаем его
    if (ui_->graph_wgt->graphCount() == 0) {
        // График 0 - основной график точек
        ui_->graph_wgt->addGraph();

        ui_->graph_wgt->graph(0)->setPen(QColor(100, 100, 100));
        ui_->graph_wgt->graph(0)->setBrush(QColor(100, 100, 100));
        ui_->graph_wgt->graph(0)->setLineStyle(QCPGraph::lsNone);

        ui_->graph_wgt->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
    }

    // Если точка уже существует - ничего не делаем
    if (points_.find(point) != points_.end()) {
        return;
    }

    QString point_str = QString::number(point.x) + ", " + QString::number(point.y);

    // Добавляем точку в список
    ui_->points_list->addItem(point_str);
    // Очищаем выбор в списке
    ui_->points_list->clearSelection();
    ui_->points_list->selectionModel()->clear();

    // Отрисовываем новую точку
    ui_->graph_wgt->graph(0)->addData(point.x, point.y);

    // Добавляем точку в множество unique_points_
    points_.insert(std::move(point));

    // Выводим соответствующее сообщение в statusBar
    this->statusBar()->showMessage("Точка (" + point_str + ") была добавлена");
}

std::vector<ransac::Point> RansacServerApp::GetPointsFromFile(std::ifstream& input) const {
    std::vector<ransac::Point> result;
    std::regex point_reg("\\(([+-]?[0-9]+), ([+-]?[0-9]+)\\)[\\s]*[\\S]*");
    std::smatch sm;

    // Итерируемся по строкам файла, парсим при помощи регулярного выражения.
    // В случае удачи добавляем пару координат в результат
    for (std::string line; std::getline(input, line);) {
        if (!std::regex_match(line, sm, point_reg)) {
            continue;
        }

        result.emplace_back(stoi(sm[1].str()), stoi(sm[2].str()));
    }

    return result;
}

void RansacServerApp::WritePointsToFile(std::ofstream& output) const {
    for (const ransac::Point& p : points_) {
        output << '(' << p.x << ", " << p.y << ")\n";
    }
}

void RansacServerApp::RemovePoint(int index) {
    // Считываем значение выделенной точки,
    // приводим к типу ransac::Point
    std::string point_str = ui_->points_list->item(index)->text().toStdString();
    ransac::Point point_to_rm = {
        stoi(point_str.substr(0, point_str.find(','))),
        stoi(point_str.substr(point_str.find(' ') + 1, point_str.size()))
    };

    // Удаляем точку из множества
    points_.erase(std::move(point_to_rm));

    // Удаляем элемент из списка
    delete ui_->points_list->takeItem(index);
    // Очищаем выбор в списке
    ui_->points_list->clearSelection();
    ui_->points_list->selectionModel()->clear();

    // Выводим соответствующее сообщение в statusBar
    this->statusBar()->showMessage("Точка (" + QString::fromStdString(point_str)
                                   + ") была удалена");
}

void RansacServerApp::UpdateButtonsEnability() {
    // Делаем все кнопки недоступными, кроме "Отправить" и "Сбросить",
    // если включен режим подсчета
    if (is_countmode_on_) {
        ui_->remove_btn->setEnabled(false);
        ui_->count_btn->setEnabled(false);
        ui_->save_btn->setEnabled(false);
        ui_->fromfile_btn->setEnabled(false);
        ui_->add_btn->setEnabled(false);

        ui_->send_btn->setEnabled(true);
        ui_->reset_btn->setEnabled(true);

        return;
    }
    // По умолчанию кнопки "Отправить" и "Сбросить" должны быть заблокированы,
    // а кнопки "Загрузить" и "Добавить" - разблокированы
    ui_->send_btn->setEnabled(false);
    ui_->reset_btn->setEnabled(false);
    ui_->fromfile_btn->setEnabled(true);
    ui_->add_btn->setEnabled(true);

    // Делаем доступной кнопку "Сохранить в файл", если в списке есть точки,
    // иначе блокируем ее
    if (ui_->points_list->count() > 0) {
        ui_->save_btn->setEnabled(true);
    }
    else {
        ui_->save_btn->setEnabled(false);
    }

    // Делаем доступным кнопку "Удалить", если какой-либо элемент списка выбран,
    // иначе блокируем ее
    if (ui_->points_list->currentItem()) {
        ui_->remove_btn->setEnabled(true);
    }
    else {
        ui_->remove_btn->setEnabled(false);
    }

    // Делаем доступной кнопку "Рассчитать", если в программу
    // внесены 5 и более точек, иначе блокируем ее
    if (points_.size() >= 5) {
        ui_->count_btn->setEnabled(true);
    }
    else {
        ui_->count_btn->setEnabled(false);
    }
}

void RansacServerApp::RedrawPoints() {
    // Если график не был создан ранее - создаем новый
    if (ui_->graph_wgt->graphCount() == 0) {
        ui_->graph_wgt->addGraph();
    }

    // Очищаем график
    ui_->graph_wgt->graph(0)->data().data()->clear();

    // Итерируемся по точкам, отображаем их на графике
    for (const ransac::Point& p : points_) {
        ui_->graph_wgt->graph(0)->addData(p.x, p.y);
    }

    // Обновляем холст
    ui_->graph_wgt->replot();
}

void RansacServerApp::DrawRansacResults(const ransac::RansacResult& ransac_result,
                                        int min_x, int max_x)
{
    // Если графики для отображения результата расчетов
    // не былы добавлены ранее - добавляем их
    if (ui_->graph_wgt->graphCount() < 4) {
        // График 1 - для линии
        ui_->graph_wgt->addGraph();
        ui_->graph_wgt->graph(1)->setPen(QColor(150, 150, 250));
        ui_->graph_wgt->graph(1)->pen().setWidth(3);
        ui_->graph_wgt->graph(1)->setLineStyle(QCPGraph::lsLine);

        // График 2 - для входящих точек
        ui_->graph_wgt->addGraph();
        ui_->graph_wgt->graph(2)->setPen(QColor(100, 100, 245));
        ui_->graph_wgt->graph(2)->setBrush(QColor(100, 100, 245));
        ui_->graph_wgt->graph(2)->setLineStyle(QCPGraph::lsNone);
        ui_->graph_wgt->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));

        // График 3 - для невходящих точек
        ui_->graph_wgt->addGraph();
        ui_->graph_wgt->graph(3)->setPen(QColor(215, 47, 47));
        ui_->graph_wgt->graph(3)->setBrush(QColor(215, 47, 47));
        ui_->graph_wgt->graph(3)->setLineStyle(QCPGraph::lsNone);
        ui_->graph_wgt->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
    }
    // Делаем невидимым основной график
    ui_->graph_wgt->graph(0)->setVisible(false);

    // Наносим на график вошедшие и не вошедшие точки
    for (const ransac::Point& p : points_) {
        if (ransac_result.second.find(p) != ransac_result.second.end()) {
            ui_->graph_wgt->graph(2)->addData(p.x, p.y);
        }
        else {
            ui_->graph_wgt->graph(3)->addData(p.x, p.y);
        }
    }

    // Наносим прямую на график
    ui_->graph_wgt->graph(1)->addData(
                min_x,
                ransac_result.first.GetY(min_x));
    ui_->graph_wgt->graph(1)->addData(
                max_x,
                ransac_result.first.GetY(max_x));

    // Обновляем холст
    ui_->graph_wgt->replot();
}

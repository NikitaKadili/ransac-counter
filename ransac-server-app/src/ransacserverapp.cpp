#include "ransacserverapp.h"
#include "./ui_ransacserverapp.h"
#include "addpointdialog.h"

#include <regex>
#include <string>
#include <QDebug> // DEBUG
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

    // Задаем диапозон видимости от -1500 до 1500 для осей
    ui_->graph_wgt->xAxis->setRange(-1500, 1500);
    ui_->graph_wgt->yAxis->setRange(-1500, 1500);

    ConnectClotsAndSignals();
}

RansacServerApp::~RansacServerApp() {
    delete ui_;
}

void RansacServerApp::slotAddPoint(int x, int y) {
    AddPoint(x, y);

    // Обновляем холст
    ui_->graph_wgt->replot();
    // Обновляем кнопки
    UpdateButtonsEnability();
}

void RansacServerApp::slotCallAddWindow() {
    // Создаем объект класса окна для добавления точки (AddPointDialog),
    // соединяем необходимые сигналы со слотами
    AddPointDialog* add_point_window = new AddPointDialog(this);
    connect(add_point_window, SIGNAL(signalAddPoint(int, int)),
            SLOT(slotAddPoint(int, int)));

    add_point_window->exec();
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
    // Если существует график - очищаем его
    if (ui_->graph_wgt->graphCount() != 0) {
        ui_->graph_wgt->graph(0)->data().data()->clear();
    }

    // В случае удачи - итерируемся по распарсенным координатам, добавляем их
    for (auto& [x, y] : GetPointsFromFile(input)) {
        AddPoint(x, y);
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
            SLOT(slotCallAddWindow()));
    // Сигнал нажатия на кнопку "Удалить"
    connect(ui_->remove_btn, SIGNAL(pressed()),
            SLOT(slotRemovePoint()));
}

void RansacServerApp::AddPoint(int x, int y) {
    // Если на холсте отсутствует график - создаем его
    if (ui_->graph_wgt->graphCount() == 0) {
        ui_->graph_wgt->addGraph();

        ui_->graph_wgt->graph(0)->setPen(QColor(100, 100, 100));
        ui_->graph_wgt->graph(0)->setBrush(QColor(100, 100, 100));
        ui_->graph_wgt->graph(0)->setLineStyle(QCPGraph::lsNone);

        ui_->graph_wgt->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 7));
    }

    ransac::Point new_point(x, y);
    // Если точка уже существует - ничего не делаем
    if (points_.find(new_point) != points_.end()) {
        return;
    }

    // Добавляем точку в множество unique_points_
    points_.insert(std::move(new_point));

    QString point_str = QString::number(x) + ", " + QString::number(y);
    // Добавляем точку в список
    ui_->points_list->addItem(point_str);
    // Очищаем выбор в списке
    ui_->points_list->clearSelection();
    ui_->points_list->selectionModel()->clear();

    // Отрисовываем новую точку
    ui_->graph_wgt->graph(0)->addData(x, y);

    // Выводим соответствующее сообщение в statusBar
    this->statusBar()->showMessage("Точка (" + point_str + ") была добавлена");
}

std::vector<std::pair<int, int>> RansacServerApp::GetPointsFromFile(std::ifstream& input) const {
    std::vector<std::pair<int, int>> result;
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
    // Делаем доступной кнопку "Сохранить в файл", если в списке есть точки
    if (ui_->points_list->count() > 0) {
        ui_->save_btn->setEnabled(true);
    }
    else {
        ui_->save_btn->setEnabled(false);
    }

    // Делаем доступными кнопки "Изменить" и "Удалить",
    // если какой-либо элемент списка выбран
    if (ui_->points_list->currentItem()) {
        ui_->change_btn->setEnabled(true);
        ui_->remove_btn->setEnabled(true);
    }
    else {
        ui_->change_btn->setEnabled(false);
        ui_->remove_btn->setEnabled(false);
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

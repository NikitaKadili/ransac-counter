#pragma once

#include <fstream>
#include <limits>
#include <set>
#include <unordered_set>
#include <vector>

#include <QtWidgets>
#include <QMainWindow>
#include <QGraphicsScene>

#include "ransac.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RansacServerApp; }
QT_END_NAMESPACE

class RansacServerApp : public QMainWindow {
    Q_OBJECT
public:
    RansacServerApp(QWidget *parent = nullptr);

    ~RansacServerApp();

public slots:
    // Слот вызывает метод AddPointToGraph, обновляет холст и кнопки
    void slotAddPoint(int, int);
    // Слот проверяет, выбран ли элемент в списке.
    // Если выбран - вызывает метод RemovePoint с его индексом,
    // обновляет холст и кнопки
    void slotRemovePoint();

    // Слот создает и показывает объект класса AddPointDialog
    // для ручного добавления новой точки
    void slotCallAddWindow();

    // Слот загружает точки из выбранного пользователем файла,
    // инорирует точки, не соответствующие шаблону: (XX, YY)
    void slotUploadFromFile();
    // Слот сохраняет точки из множества points_ в указанный пользователем файл
    void slotSaveToFile();

    // Слот обновляет доступность кнопок
    // при нажатии на элемент списка с точками
    void slotUnlockButtons(QListWidgetItem*);

    // Метод запускает процесс вычисления уравнения прямой алгоритмом RANSAC
    void slotCountRansacModel();

private:
    Ui::RansacServerApp* ui_;

    std::unordered_set<ransac::Point, ransac::PointHasher> points_; // Множество точек

    // Метод соединяет слоты с соответствующими им сигналами
    void ConnectClotsAndSignals();

    // Метод добавляет точку в необходимые поля класса
    void AddPoint(int, int);
    // Метод удаляет точку из всех полей класса
    void RemovePoint(int);

    // Метод возвращает вектор пар координат из распарсенного файлового потока
    std::vector<std::pair<int, int>> GetPointsFromFile(std::ifstream&) const;
    // Метод записывает координаты из points_ в файловый поток
    void WritePointsToFile(std::ofstream&) const;

    // Метод обновляет доступность кнопок в зависимости от состояния программы
    void UpdateButtonsEnability();
    // Метод перерисовывает актуальные точки из множества points_ на холст,
    // удаляет все, что было нарисовано ранее
    void RedrawPoints();
};

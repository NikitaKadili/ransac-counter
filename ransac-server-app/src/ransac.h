#pragma once

#include <limits>
#include <tuple>
#include <vector>
#include <unordered_set>

namespace ransac {

// Структура представляет набор координат точки
struct Point {
    int x = 0;
    int y = 0;

    Point(int x, int y);
};
bool operator<(const Point&, const Point&);
bool operator==(const Point&, const Point&);
// Класс представляет хэшер для структуры Point
class PointHasher {
public:
    std::size_t operator()(const Point&) const;
};

// Структура представляет линейную формулу типа: y(x) = ax + b
struct LineFormula {
    double a = 0;
    double b = 0;

    // Метод возвращает значение y(x)
    double GetY(int) const;
};

// Структура представляет перечень настроек,
// необходимых для работы RANSAC-алгоритма
struct Settings {
    double max_y_diff = 1.0;  // Максимальное отклонение точек от аппроксимирующей прямой
    int iterations_num = 200; // Количество итерация
    int inliers_size = 3;     // Размер случайных множеств

    bool auto_max_y_diff = false;   // Автоматически рассчитать максимальную разницу
    bool auto_inliers_size = false; // Автоматически рассчитать размер
};

// Представление результата вычислений RANSAC-алгоритма -
// линейная формула и вектор входязих точек
using RansacResult = std::pair<LineFormula, std::vector<Point>>;

// Класс представляет объект, расчитывающий линейную формулу для
// некоторого набора точек по алгоритму RANSAC
class RansacCounter final {
public:
    RansacCounter(const std::unordered_set<Point, PointHasher>&, Settings&);

    // Метод возвращает результат работы RANSAC-алгоритма в виде структуры RansacResult
    RansacResult Count() const;

    // Метод возвращает минимальное значение X из текущего заданного набора точек
    int GetMinX() const;
    // Метод возвращает максимальное значение X из текущего заданного набора точек
    int GetMaxX() const;

private:
    std::vector<Point> points_; // Набор точек
    Settings& settings_; // Ссылка на набор настроек для RANSAC-алгоритма

    int max_x_ = std::numeric_limits<int>::min(); // Максимальное значение X
    int min_x_ = std::numeric_limits<int>::max(); // Минимальное значение X
    int max_y_ = std::numeric_limits<int>::min(); // Максимальное значение Y
    int min_y_ = std::numeric_limits<int>::max(); // Минимальное значение Y

    // Метод возвращает случаную величину в диапозоне [0, max_y)
    int GetRandomNum(int) const;

    // Метод возвращает уравнение аппроксимирующей прямой для набора переданных точек
    LineFormula GetApprox(const std::unordered_set<int>&) const;

    // Метод копирует копирует набор точек из unordered_set в points_,
    // параллельно находит минимумы и максимумы X и Y
    void CopyPointsFromUnorderedSet(const std::unordered_set<Point, PointHasher>&);
};

} // namespace ransac

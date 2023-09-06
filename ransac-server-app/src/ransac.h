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

// Представление результата вычислений RANSAC-алгоритма -
// линейная формула и вектор входязих точек
using RansacResult = std::pair<LineFormula, std::vector<Point>>;

// Класс представляет объект, расчитывающий линейную формулу для
// некоторого набора точек по алгоритму RANSAC
class RansacCounter final {
public:
    RansacCounter(const std::unordered_set<Point, PointHasher>&, double max_y_diff_ = 0.0);

    // Метод задает количество необходимых итераций
    RansacCounter& SetInterations(int);
    // Метод вносит точку с координатами (x; y) в набор точек
    void AddPoint(int, int);
    // Метод очищает набор точек
    void ClearPoints();

    // Метод возвращает результат работы RANSAC-алгоритма в виде структуры RansacResult
//    LineFormula Count() const;
    RansacResult Count() const;

    // Метод возвращает минимальное значение X из текущего заданного набора точек
    int GetMinX() const;
    // Метод возвращает максимальное значение X из текущего заданного набора точек
    int GetMaxX() const;

private:
    std::vector<Point> points_; // Набор точек
    int iterations_ = 100;      // Количество итераций (по умолчанию 100)

    int max_x_ = std::numeric_limits<int>::min(); // Максимальное значение X
    int min_x_ = std::numeric_limits<int>::max(); // Минимальное значение X
    int max_y_ = std::numeric_limits<int>::min(); // Максимальное значение Y
    int min_y_ = std::numeric_limits<int>::max(); // Минимальное значение Y

    double max_y_diff_ = 0.0; // Максимально-допустимое отклонение точек от прямой по оси Y

    // Метод возвращает случаную величину в диапозоне [0, max_y)
    int GetRandomNum(int) const;

    // Метод возвращает уравнение аппроксимирующей прямой для набора переданных точек
    LineFormula GetApprox(const std::unordered_set<int>&) const;
};

} // namespace ransac

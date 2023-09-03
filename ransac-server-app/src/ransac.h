#pragma once

#include <limits>
#include <vector>
#include <unordered_set>

namespace ransac {

// Структура представляет набор координат точки
struct Point {
    int x = 0;
    int y = 0;

    Point(int x, int y);
};
// Структура представляет линейную формулу типа: y(x) = ax + b
struct LineFormula {
    double a = 0;
    double b = 0;

    // Метод возвращает значение y(x)
    double GetY(int) const;
};

// Класс представляет объект, расчитывающий линейную формулу для
// некоторого набора точек по алгоритму RANSAC
class RansacCounter final {
public:
    RansacCounter() = default;
    RansacCounter(std::vector<Point>);

    // Метод задает количество необходимых итераций
    RansacCounter& SetInterations(int);
    // Метод вносит точку с координатами (x; y) в набор точек
    void AddPoint(int, int);
    // Метод очищает набор точек
    void ClearPoints();

    // Метод рассчитывает линейную формулу, возвращает
    // ее в виду структуры LineFormula
    LineFormula Count() const;

private:
    std::vector<Point> points_; // Набор точек
    int iterations_ = 100;      // Количество итераций (по умолчанию 100)

    // Метод возвращает случаную величину в диапозоне [0, max_y)
    int GetRandomNum(int) const;

    // Метод возвращает уравнение аппроксимирующей прямой для набора переданных точек
    LineFormula GetApprox(const std::unordered_set<int>&) const;

    // Метод возвращает максимальное и минимальное значения Y в векторе points_
    std::pair<int, int> GetMinMaxY() const;
};

} // namespace ransac

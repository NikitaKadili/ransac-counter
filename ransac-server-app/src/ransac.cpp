#include "ransac.h"

#include <algorithm>
#include <random>
#include <unordered_set>

ransac::RansacCounter::RansacCounter(std::vector<Point> points)
    : points_(std::move(points))
{ /* do nothing */ }

ransac::RansacCounter& ransac::RansacCounter::SetInterations(int iterations) {
    iterations_ = iterations;
    return *this;
}

void ransac::RansacCounter::AddPoint(int x, int y) {
    points_.emplace_back(x, y);
}

void ransac::RansacCounter::ClearPoints() { points_.clear(); }

[[nodiscard]] ransac::LineFormula ransac::RansacCounter::Count() const {
    LineFormula best_model = { -1, -1 }; // Наиболее подходящая линейная модель

    if (points_.size() < 3) {
        return best_model;
    }

    int max_inliers = 0; // Максимально количество входящих точек
    // Минимальное количество невходящих точек
    int min_outliers = std::numeric_limits<int>::max();
    // Размер случайных множеств
    int temp_inliers_size
            = std::max(2, static_cast<int>(points_.size()) / 10);

    // Рассчитываем максимально-допустимую разницу между Y
    // для разделения точек на входищие и не входящие
    auto [max_y, min_y] = GetMinMaxY();
    double max_y_diff = (max_y - min_y) / 2.0;

    for (int i = 0; i < iterations_; ++i) {
        std::unordered_set<int> temp_set; // Множество случайных точек
        temp_set.reserve(temp_inliers_size);

        // Набираем temp_inliers_size случайных точек
        for (int j = 0; j < temp_inliers_size; ++j) {
            int random_id = GetRandomNum(static_cast<int>(points_.size()));

            if (temp_set.find(random_id) != temp_set.end()) {
                --j;
            }
            else {
                temp_set.insert(random_id);
            }
        }

        // Находим модель для текущего набора случайных точек
        LineFormula temp_model = GetApprox(temp_set);

        int current_inliers = 0;     // Количество входящих точек для временной модели
        int current_outliers = 0; // // Количество не входящих точек для временной модели

        // Находим все входящие и не входящие точки
        for (const Point& p : points_) {
            if (std::abs(static_cast<double>(p.y) - temp_model.GetY(p.x)) <= max_y_diff) {
                ++current_inliers;
            }
            else {
                ++current_outliers;
            }
        }

        // Если текущая модель имеет лучшие характеристики - копируем ее в best_model
        if (max_inliers < current_inliers
                && min_outliers > current_outliers)
        {
            best_model = temp_model;
            max_inliers = current_inliers;
            min_outliers = current_outliers;
        }
    }

    return best_model;
}

int ransac::RansacCounter::GetRandomNum(int max) const {
    static std::random_device rd;
    static std::mt19937 g(rd());
    std::uniform_int_distribution<int> dist(0, max - 1);

    return dist(g);
}

ransac::LineFormula ransac::RansacCounter::GetApprox(const std::unordered_set<int>& points) const {
    double sum_x = 0.0;
    double sum_y = 0.0;
    double sum_x2 = 0.0;
    double sum_xy = 0.0;
    int n = static_cast<int>(points.size());

    for (int p_id : points) {
        sum_x += points_[p_id].x;
        sum_y += points_[p_id].y;
        sum_x2 += points_[p_id].x * points_[p_id].x;
        sum_xy += points_[p_id].x * points_[p_id].y;
    }

    double a = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);
    double b = (sum_y - a * sum_x) / n;

    return { a, b };
}

std::pair<int, int> ransac::RansacCounter::GetMinMaxY() const {
    int max_y = std::numeric_limits<int>::min(); // Максимальное значение Y
    int min_y = std::numeric_limits<int>::max(); // Минимальное значение Y

    for (const Point& p : points_) {
        max_y = std::max(max_y, p.y);
        min_y = std::min(min_y, p.y);
    }

    return { max_y, min_y };
}

ransac::Point::Point(int x, int y) : x(x), y(y) { /* do nothing */ }

double ransac::LineFormula::GetY(int x) const {
    return a * static_cast<double>(x) + b;
}

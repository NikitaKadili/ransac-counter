#include "ransac.h"

#include <algorithm>
#include <random>

ransac::RansacCounter::RansacCounter(
        const std::unordered_set<Point, PointHasher>& points,
        Settings& settings)
    : settings_(settings)
{
    CopyPointsFromUnorderedSet(points);

    // Рассчитываем размер случайных множеств, если необходимо
    if (settings_.auto_inliers_size) {
        settings_.inliers_size = std::max(2, static_cast<int>(points_.size() / 100));
    }
    // Рассчитываем максимально-допустимую разницу, если необходимо
    if (settings_.auto_max_y_diff) {
        settings_.max_y_diff = (max_y_ - min_y_) / 3.0;
    }
}

[[nodiscard]] ransac::RansacResult ransac::RansacCounter::Count() const {
    LineFormula best_model = { 0, 0 }; // Наиболее подходящая линейная модель
    std::vector<Point> best_inliers;

    if (points_.size() < 3) {
        return { best_model, best_inliers };
    }

    int max_inliers = 0; // Максимально количество входящих точек
    // Минимальное количество невходящих точек
    int min_outliers = std::numeric_limits<int>::max();

    for (int i = 0; i < settings_.iterations_num; ++i) {
        std::unordered_set<int> temp_set; // Множество случайных точек
        temp_set.reserve(settings_.inliers_size);

        // Набираем temp_inliers_size случайных точек
        for (int j = 0; j < settings_.inliers_size; ++j) {
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

        int current_inliers = 0;  // Количество входящих точек для временной модели
        int current_outliers = 0; // Количество не входящих точек для временной модели

        std::vector<Point> temp_inliers; // Вектор текущих входящих точек
        temp_set.reserve(settings_.inliers_size);

        // Находим все входящие и не входящие точки
        for (const Point& p : points_) {
            if (std::abs(static_cast<double>(p.y) - temp_model.GetY(p.x))
                    <= settings_.max_y_diff)
            {
                ++current_inliers;
                temp_inliers.push_back(std::move(p));
            }
            else {
                ++current_outliers;
            }
        }

        // Если текущая модель имеет лучшие характеристики - делаем ее лучшей
        if (max_inliers < current_inliers
                && min_outliers > current_outliers)
        {
            best_model = temp_model;

            max_inliers = current_inliers;
            min_outliers = current_outliers;

            std::swap(best_inliers, temp_inliers);
        }
    }

    return { best_model, best_inliers };
}

int ransac::RansacCounter::GetMinX() const { return min_x_; }
int ransac::RansacCounter::GetMaxX() const { return max_x_; }

int ransac::RansacCounter::GetRandomNum(int max) const {
    static std::random_device rd;
    static std::mt19937 g(rd());
    std::uniform_int_distribution<int> dist(0, max - 1);

    return dist(g);
}

ransac::LineFormula ransac::RansacCounter::GetApprox(
        const std::unordered_set<int>& points) const
{
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

void ransac::RansacCounter::CopyPointsFromUnorderedSet(
        const std::unordered_set<Point, PointHasher>& points)
{
    // Итерируемся по точкам, переносим их в вектор,
    // параллельно находим минимумы и максимумы X и Y
    for (Point p : points) {
        max_x_ = std::max(max_x_, p.x);
        min_x_ = std::min(min_x_, p.x);

        max_y_ = std::max(max_y_, p.y);
        min_y_ = std::min(min_y_, p.y);

        points_.push_back(std::move(p));
    }
}

ransac::Point::Point(int x, int y) : x(x), y(y) { /* do nothing */ }

double ransac::LineFormula::GetY(int x) const {
    return a * static_cast<double>(x) + b;
}

bool ransac::operator<(const Point& lhs, const Point& rhs) {
    return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
}
bool ransac::operator==(const Point& lhs, const Point& rhs) {
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}
std::size_t ransac::PointHasher::operator()(const Point& p) const {
    return p.x + p.y * 27;
}

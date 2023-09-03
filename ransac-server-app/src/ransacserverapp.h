#pragma once

#include <QMainWindow>
#include "ransac.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RansacServerApp; }
QT_END_NAMESPACE

class RansacServerApp : public QMainWindow {
    Q_OBJECT
public:
    RansacServerApp(QWidget *parent = nullptr);
    ~RansacServerApp();

private:
    Ui::RansacServerApp *ui;
    ransac::RansacCounter* ransac_counter_;
};

#include "ransacserverapp.h"
#include "./ui_ransacserverapp.h"

#include <QDebug>

RansacServerApp::RansacServerApp(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RansacServerApp)
    , ransac_counter_(new ransac::RansacCounter)
{
    ui->setupUi(this);
}

RansacServerApp::~RansacServerApp() {
    delete ui;
}


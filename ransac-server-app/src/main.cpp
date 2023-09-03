#include <QApplication>
#include "ransacserverapp.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    RansacServerApp w;
    w.show();

    return a.exec();
}

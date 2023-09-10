#include <QApplication>

#include "ransacserverapp.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    RansacServerApp ransac_server_window;
    ransac_server_window.show();

    return app.exec();
}

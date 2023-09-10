#include "ransacclientapp.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    RansacClientApp client_app_window;
    client_app_window.show();

    return app.exec();
}

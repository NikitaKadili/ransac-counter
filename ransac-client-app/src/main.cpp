#include "ransacclientapp.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    RansacClientApp client_app;
    client_app.show();

    return app.exec();
}

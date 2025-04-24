#include "AutoMagik.h"
#include <QtWidgets/QApplication>
#include "firebase.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AutoMagik w;
    w.show();
    return a.exec();
}

#include <QFontDatabase> //czcionka
#include "AutoMagik.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AutoMagik w;
    w.show();
    return a.exec();


int fontId = QFontDatabase::addApplicationFont(":/fonts/tektur.ttf"); //czcionka
if (fontId != -1) {
    QString family = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont font(family);
    QApplication::setFont(font);  // lub użyj tylko lokalnie
}

}

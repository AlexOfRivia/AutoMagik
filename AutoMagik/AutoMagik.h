#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_AutoMagik.h"

class AutoMagik : public QMainWindow
{
    Q_OBJECT

public:
    AutoMagik(QWidget *parent = nullptr);
    ~AutoMagik();

private:
    Ui::AutoMagikClass ui;
};

#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TransMaster.h"

class TransMaster : public QMainWindow
{
    Q_OBJECT

public:
    TransMaster(QWidget *parent = nullptr);
    ~TransMaster();

private:
    Ui::TransMasterClass ui;
};

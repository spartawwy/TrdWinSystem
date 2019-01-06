#ifndef AUTO_FORCAST_SERVER_H
#define AUTO_FORCAST_SERVER_H

#include <QtWidgets/QWidget>
#include "ui_auto_forcast_server.h"

class auto_forcast_server : public QWidget
{
    Q_OBJECT

public:
    auto_forcast_server(QWidget *parent = 0);
    ~auto_forcast_server();

private:
    Ui::auto_forcast_serverClass ui;
};

#endif // AUTO_FORCAST_SERVER_H

/********************************************************************************
** Form generated from reading UI file 'auto_forcast_server.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUTO_FORCAST_SERVER_H
#define UI_AUTO_FORCAST_SERVER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_auto_forcast_serverClass
{
public:

    void setupUi(QWidget *auto_forcast_serverClass)
    {
        if (auto_forcast_serverClass->objectName().isEmpty())
            auto_forcast_serverClass->setObjectName(QStringLiteral("auto_forcast_serverClass"));
        auto_forcast_serverClass->resize(600, 400);

        retranslateUi(auto_forcast_serverClass);

        QMetaObject::connectSlotsByName(auto_forcast_serverClass);
    } // setupUi

    void retranslateUi(QWidget *auto_forcast_serverClass)
    {
        auto_forcast_serverClass->setWindowTitle(QApplication::translate("auto_forcast_serverClass", "auto_forcast_server", 0));
    } // retranslateUi

};

namespace Ui {
    class auto_forcast_serverClass: public Ui_auto_forcast_serverClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTO_FORCAST_SERVER_H

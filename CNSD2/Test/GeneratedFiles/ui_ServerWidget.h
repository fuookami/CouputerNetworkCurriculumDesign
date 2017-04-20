/********************************************************************************
** Form generated from reading UI file 'ServerWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVERWIDGET_H
#define UI_SERVERWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ServerWidget
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QRadioButton *LocalHostRadios;
    QRadioButton *IPAddressRadio;
    QLineEdit *IPP1;
    QLabel *label_2;
    QLineEdit *IPP2;
    QLabel *label_3;
    QLineEdit *IPP3;
    QLabel *label_4;
    QLineEdit *IPP4;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_5;
    QLineEdit *Port;
    QSpacerItem *horizontalSpacer;
    QTextBrowser *Log;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *Close;
    QPushButton *Listen;
    QPushButton *StopListen;

    void setupUi(QWidget *ServerWidget)
    {
        if (ServerWidget->objectName().isEmpty())
            ServerWidget->setObjectName(QStringLiteral("ServerWidget"));
        ServerWidget->resize(450, 720);
        ServerWidget->setMinimumSize(QSize(450, 720));
        ServerWidget->setMaximumSize(QSize(450, 720));
        verticalLayoutWidget = new QWidget(ServerWidget);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 10, 426, 701));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QStringLiteral("label"));
        QFont font;
        font.setPointSize(12);
        label->setFont(font);

        horizontalLayout->addWidget(label);

        LocalHostRadios = new QRadioButton(verticalLayoutWidget);
        LocalHostRadios->setObjectName(QStringLiteral("LocalHostRadios"));

        horizontalLayout->addWidget(LocalHostRadios);

        IPAddressRadio = new QRadioButton(verticalLayoutWidget);
        IPAddressRadio->setObjectName(QStringLiteral("IPAddressRadio"));

        horizontalLayout->addWidget(IPAddressRadio);

        IPP1 = new QLineEdit(verticalLayoutWidget);
        IPP1->setObjectName(QStringLiteral("IPP1"));
        IPP1->setMaximumSize(QSize(40, 16777215));
        IPP1->setMaxLength(3);
        IPP1->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(IPP1);

        label_2 = new QLabel(verticalLayoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout->addWidget(label_2);

        IPP2 = new QLineEdit(verticalLayoutWidget);
        IPP2->setObjectName(QStringLiteral("IPP2"));
        IPP2->setMaximumSize(QSize(40, 16777215));
        IPP2->setMaxLength(3);
        IPP2->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(IPP2);

        label_3 = new QLabel(verticalLayoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout->addWidget(label_3);

        IPP3 = new QLineEdit(verticalLayoutWidget);
        IPP3->setObjectName(QStringLiteral("IPP3"));
        IPP3->setMaximumSize(QSize(40, 16777215));
        IPP3->setMaxLength(3);
        IPP3->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(IPP3);

        label_4 = new QLabel(verticalLayoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout->addWidget(label_4);

        IPP4 = new QLineEdit(verticalLayoutWidget);
        IPP4->setObjectName(QStringLiteral("IPP4"));
        IPP4->setMaximumSize(QSize(40, 16777215));
        IPP4->setMaxLength(3);
        IPP4->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(IPP4);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_5 = new QLabel(verticalLayoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setFont(font);

        horizontalLayout_2->addWidget(label_5);

        Port = new QLineEdit(verticalLayoutWidget);
        Port->setObjectName(QStringLiteral("Port"));
        Port->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_2->addWidget(Port);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_2);

        Log = new QTextBrowser(verticalLayoutWidget);
        Log->setObjectName(QStringLiteral("Log"));

        verticalLayout->addWidget(Log);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        Close = new QPushButton(verticalLayoutWidget);
        Close->setObjectName(QStringLiteral("Close"));

        horizontalLayout_3->addWidget(Close);

        Listen = new QPushButton(verticalLayoutWidget);
        Listen->setObjectName(QStringLiteral("Listen"));

        horizontalLayout_3->addWidget(Listen);

        StopListen = new QPushButton(verticalLayoutWidget);
        StopListen->setObjectName(QStringLiteral("StopListen"));

        horizontalLayout_3->addWidget(StopListen);


        verticalLayout->addLayout(horizontalLayout_3);


        retranslateUi(ServerWidget);

        QMetaObject::connectSlotsByName(ServerWidget);
    } // setupUi

    void retranslateUi(QWidget *ServerWidget)
    {
        ServerWidget->setWindowTitle(QApplication::translate("ServerWidget", "ServerWidget", Q_NULLPTR));
        label->setText(QApplication::translate("ServerWidget", "IP\345\234\260\345\235\200\357\274\232", Q_NULLPTR));
        LocalHostRadios->setText(QApplication::translate("ServerWidget", "\346\234\254\345\234\260", Q_NULLPTR));
        IPAddressRadio->setText(QApplication::translate("ServerWidget", "\346\214\207\345\256\232IP\345\234\260\345\235\200", Q_NULLPTR));
        label_2->setText(QApplication::translate("ServerWidget", ".", Q_NULLPTR));
        label_3->setText(QApplication::translate("ServerWidget", ".", Q_NULLPTR));
        label_4->setText(QApplication::translate("ServerWidget", ".", Q_NULLPTR));
        label_5->setText(QApplication::translate("ServerWidget", "\347\253\257\345\217\243\345\234\260\345\235\200\357\274\232", Q_NULLPTR));
        Close->setText(QApplication::translate("ServerWidget", "\345\205\263\351\227\255", Q_NULLPTR));
        Listen->setText(QApplication::translate("ServerWidget", "\345\274\200\345\247\213\347\233\221\345\220\254", Q_NULLPTR));
        StopListen->setText(QApplication::translate("ServerWidget", "\345\201\234\346\255\242\347\233\221\345\220\254", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ServerWidget: public Ui_ServerWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVERWIDGET_H

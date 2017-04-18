/********************************************************************************
** Form generated from reading UI file 'CilentWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CILENTWIDGET_H
#define UI_CILENTWIDGET_H

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
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CilentWidget
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
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_7;
    QSpinBox *DataSize;
    QLabel *label_6;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_8;
    QLineEdit *Message;
    QTextBrowser *Log;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *Close;
    QPushButton *SendRandomData;
    QPushButton *SendMessage;
    QPushButton *Connect;
    QPushButton *Disconnect;

    void setupUi(QWidget *CilentWidget)
    {
        if (CilentWidget->objectName().isEmpty())
            CilentWidget->setObjectName(QStringLiteral("CilentWidget"));
        CilentWidget->resize(480, 350);
        CilentWidget->setMinimumSize(QSize(480, 350));
        CilentWidget->setMaximumSize(QSize(480, 350));
        verticalLayoutWidget = new QWidget(CilentWidget);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 10, 461, 331));
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

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_7 = new QLabel(verticalLayoutWidget);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setFont(font);

        horizontalLayout_4->addWidget(label_7);

        DataSize = new QSpinBox(verticalLayoutWidget);
        DataSize->setObjectName(QStringLiteral("DataSize"));
        DataSize->setMinimum(1);
        DataSize->setMaximum(100);

        horizontalLayout_4->addWidget(DataSize);

        label_6 = new QLabel(verticalLayoutWidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        horizontalLayout_4->addWidget(label_6);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_8 = new QLabel(verticalLayoutWidget);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setFont(font);

        horizontalLayout_5->addWidget(label_8);

        Message = new QLineEdit(verticalLayoutWidget);
        Message->setObjectName(QStringLiteral("Message"));

        horizontalLayout_5->addWidget(Message);


        verticalLayout->addLayout(horizontalLayout_5);

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

        SendRandomData = new QPushButton(verticalLayoutWidget);
        SendRandomData->setObjectName(QStringLiteral("SendRandomData"));

        horizontalLayout_3->addWidget(SendRandomData);

        SendMessage = new QPushButton(verticalLayoutWidget);
        SendMessage->setObjectName(QStringLiteral("SendMessage"));

        horizontalLayout_3->addWidget(SendMessage);

        Connect = new QPushButton(verticalLayoutWidget);
        Connect->setObjectName(QStringLiteral("Connect"));

        horizontalLayout_3->addWidget(Connect);

        Disconnect = new QPushButton(verticalLayoutWidget);
        Disconnect->setObjectName(QStringLiteral("Disconnect"));

        horizontalLayout_3->addWidget(Disconnect);


        verticalLayout->addLayout(horizontalLayout_3);


        retranslateUi(CilentWidget);

        QMetaObject::connectSlotsByName(CilentWidget);
    } // setupUi

    void retranslateUi(QWidget *CilentWidget)
    {
        CilentWidget->setWindowTitle(QApplication::translate("CilentWidget", "Cilent", Q_NULLPTR));
        label->setText(QApplication::translate("CilentWidget", "IP\345\234\260\345\235\200\357\274\232", Q_NULLPTR));
        LocalHostRadios->setText(QApplication::translate("CilentWidget", "\346\234\254\345\234\260", Q_NULLPTR));
        IPAddressRadio->setText(QApplication::translate("CilentWidget", "\346\214\207\345\256\232IP\345\234\260\345\235\200", Q_NULLPTR));
        label_2->setText(QApplication::translate("CilentWidget", ".", Q_NULLPTR));
        label_3->setText(QApplication::translate("CilentWidget", ".", Q_NULLPTR));
        label_4->setText(QApplication::translate("CilentWidget", ".", Q_NULLPTR));
        label_5->setText(QApplication::translate("CilentWidget", "\347\253\257\345\217\243\345\234\260\345\235\200\357\274\232", Q_NULLPTR));
        label_7->setText(QApplication::translate("CilentWidget", "\351\232\217\346\234\272\346\225\260\346\215\256\351\225\277\345\272\246\357\274\232", Q_NULLPTR));
        label_6->setText(QApplication::translate("CilentWidget", "MB", Q_NULLPTR));
        label_8->setText(QApplication::translate("CilentWidget", "\344\277\241\346\201\257\357\274\232", Q_NULLPTR));
        Close->setText(QApplication::translate("CilentWidget", "\345\205\263\351\227\255", Q_NULLPTR));
        SendRandomData->setText(QApplication::translate("CilentWidget", "\347\224\237\346\210\220\351\232\217\346\234\272\346\225\260\346\215\256\345\271\266\345\217\221\351\200\201", Q_NULLPTR));
        SendMessage->setText(QApplication::translate("CilentWidget", "\345\217\221\351\200\201\344\277\241\346\201\257", Q_NULLPTR));
        Connect->setText(QApplication::translate("CilentWidget", "\350\277\236\346\216\245", Q_NULLPTR));
        Disconnect->setText(QApplication::translate("CilentWidget", "\346\226\255\345\274\200\350\277\236\346\216\245", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CilentWidget: public Ui_CilentWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CILENTWIDGET_H

#include "ServerWindow.h"

ServerWindow::ServerWindow(QWidget * parent)
	: QMainWindow(parent)
{
	ui = new Ui::ServerWidget();
	ui->setupUi(this);

	ui->LocalHostRadios->setChecked(true);
	ui->Port->setText(QString::fromLocal8Bit("22884"));

	connect(ui->Close, SIGNAL(click()), this, SLOT(closeClickSlot()));
	connect(ui->Listen, SIGNAL(click()), this, SLOT(listenClickSlot()));
	connect(ui->StopListen, SIGNAL(click()), this, SLOT(stopListenClickSlot()));
	// msg
}

void ServerWindow::closeClickSlot()
{

}

void ServerWindow::listenClickSlot()
{
	if (checkIsValid())
	{

	}
}

void ServerWindow::stopListenClickSlot()
{

}

void ServerWindow::showMSG(const QString &msg)
{
	ui->Log->toPlainText().append(msg);
}
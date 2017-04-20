#include "ServerWindow.h"

ServerWindow::ServerWindow(QWidget * parent)
	: QMainWindow(parent), beClosed(false)
{
	ui = new Ui::ServerWidget();
	ui->setupUi(this);

	ui->LocalHostRadios->setChecked(true);
	ui->Port->setText(QString::fromLocal8Bit("22884"));

	server = new Server();

	connect(ui->Close, SIGNAL(clicked()), this, SLOT(closeClickSlot()));
	connect(ui->Listen, SIGNAL(clicked()), this, SLOT(listenClickSlot()));
	connect(ui->StopListen, SIGNAL(clicked()), this, SLOT(stopListenClickSlot()));
	connect(server, SIGNAL(closed()), this, SLOT(serverClosed()));
	connect(server, SIGNAL(pushMsg(const QString)), this, SLOT(showMsg(const QString)));
	// msg
}

void ServerWindow::closeClickSlot()
{
	beClosed = true;
	server->close();
}

void ServerWindow::listenClickSlot()
{
	if (checkIsValid())
	{
		QHostAddress host;
		quint16 port(ui->Port->text().toUInt());

		if (ui->LocalHostRadios->isChecked())
			host = QHostAddress::LocalHost;
		else
		{
			std::ostringstream sout;
			sout << ui->IPP1 << "." << ui->IPP2 << "." << ui->IPP3 << "." << ui->IPP4;
			host = QString::fromLocal8Bit(sout.str().c_str());
		}

		server->listen(host, port);
	}
}

void ServerWindow::stopListenClickSlot()
{
	server->close();
}

void ServerWindow::serverClosed(void)
{
	if (beClosed)
		this->close();
}

void ServerWindow::showMsg(const QString msg)
{
	ui->Log->insertPlainText(msg);
	qDebug() << QString::fromLocal8Bit("��������") << msg;
}

bool ServerWindow::checkIsValid(void)
{
	static const auto isValidIPPart([](const QLineEdit *pLineEdit)->bool
	{
		return pLineEdit->text().toUInt() < 256;
	});

	static const auto isValidPort([](const QLineEdit *pLineEdit)->bool
	{
		return pLineEdit->text().toUInt() < 0x00010000;
	});

	if (ui->LocalHostRadios->isChecked())
		return isValidPort(ui->Port);
	else
		return isValidIPPart(ui->IPP1) && isValidIPPart(ui->IPP2)
		&& isValidIPPart(ui->IPP3) && isValidIPPart(ui->IPP4) && isValidPort(ui->Port);
}
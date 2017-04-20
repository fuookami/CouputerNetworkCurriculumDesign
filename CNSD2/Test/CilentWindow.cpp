#include "CilentWindow.h"
#include <random>
#include <ctime>

CilentWindow::CilentWindow(QWidget * parent)
	: QMainWindow(parent), beClosed(false)
{
	m_pUi = new Ui::CilentWidget();
	m_pUi->setupUi(this);

	m_pUi->LocalHostRadios->setChecked(true);
	m_pUi->Port->setText(QString::fromLocal8Bit("22884"));

	m_pUi->Disconnect->setEnabled(false);
	m_pUi->SendRandomData->setEnabled(false);
	m_pUi->SendMessage->setEnabled(false);

	m_pCilent = new Cilent();

	connect(m_pCilent, SIGNAL(pushMsg(const QString)), this, SLOT(showMsg(const QString)));
	connect(m_pUi->Connect, SIGNAL(clicked()), this, SLOT(connectToHost()));
	connect(m_pUi->Close, SIGNAL(clicked()), this, SLOT(closeSlot()));

	connect(m_pUi->Disconnect, SIGNAL(clicked()), this, SLOT(disconnectFromHost()));
	connect(m_pUi->SendMessage, SIGNAL(clicked()), this, SLOT(sendMsg()));
	connect(m_pUi->SendRandomData, SIGNAL(clicked()), this, SLOT(sendRandomData()));
}

void CilentWindow::showMsg(const QString msg)
{
	m_pUi->Log->insertPlainText(msg);
	qDebug() << QString::fromLocal8Bit("¿Í»§¶Ë£º") << msg;
}

void CilentWindow::sendRandomData(void)
{
	static std::mt19937_64 engine(time(nullptr));
	Public::DataType data(m_pUi->DataSize->value() * 1024, '0');
	for (unsigned char & b : data)
		b = engine() % 256;

	m_pCilent->wirteToHost(data);
}

void CilentWindow::sendMsg(void)
{
	m_pCilent->wirteToHost(std::string(m_pUi->Message->text().toLocal8Bit()));
}

void CilentWindow::connectToHost(void)
{
	if (checkIsValid())
	{
		m_pUi->Connect->setEnabled(false);
		m_pUi->Close->setEnabled(false);

		QHostAddress host;
		quint16 port(m_pUi->Port->text().toUInt());

		if (m_pUi->LocalHostRadios->isChecked())
			host = QHostAddress::LocalHost;
		else
		{
			std::ostringstream sout;
			sout << m_pUi->IPP1 << "." << m_pUi->IPP2 << "." << m_pUi->IPP3 << "." << m_pUi->IPP4;
			host = QString::fromLocal8Bit(sout.str().c_str());
		}

		connect(m_pCilent, SIGNAL(cilentConnected()), this, SLOT(connectSucceed()));
		connect(m_pCilent, SIGNAL(serverLose()), this, SLOT(connectFail()));
		m_pCilent->connectToHost(host, port);
	}
}

void CilentWindow::connectSucceed(void)
{
	disconnect(m_pCilent, SIGNAL(cilentConnected()));
	disconnect(m_pCilent, SIGNAL(serverLose()));
	m_pUi->Close->setEnabled(true);
	m_pUi->SendMessage->setEnabled(true);
	m_pUi->SendRandomData->setEnabled(true);
	m_pUi->Disconnect->setEnabled(true);
}

void CilentWindow::connectFail(void)
{
	disconnect(m_pCilent, SIGNAL(cilentConnected()));
	disconnect(m_pCilent, SIGNAL(serverLose()));
	m_pUi->Connect->setEnabled(true);
	m_pUi->Close->setEnabled(true);
}

void CilentWindow::disconnectFromHost(void)
{
	m_pUi->Disconnect->setEnabled(false);
	m_pUi->SendRandomData->setEnabled(false);
	m_pUi->SendMessage->setEnabled(false);
	m_pUi->Close->setEnabled(false);
	connect(m_pCilent, SIGNAL(cilentDisconnected()), this, SLOT(disconnectSucceed()));

	m_pCilent->disconnectFromHost();
}

void CilentWindow::disconnectSucceed()
{
	if (beClosed)
	{
		this->close();
	}

	disconnect(m_pCilent, SIGNAL(cilentDisconnected()));
	m_pUi->Connect->setEnabled(true);
	m_pUi->Close->setEnabled(true);
}

void CilentWindow::closeSlot(void)
{
	if (m_pCilent->state() == QAbstractSocket::UnconnectedState)
	{
		this->close();
	}
	else 
	{
		m_pUi->Disconnect->setEnabled(false);
		m_pUi->SendRandomData->setEnabled(false);
		m_pUi->SendMessage->setEnabled(false);
		m_pUi->Close->setEnabled(false);
		beClosed = true;
		connect(m_pCilent, SIGNAL(cilentDisconnected()), this, SLOT(disconnectSucceed()));
		m_pCilent->disconnectFromHost();
	}
}

bool CilentWindow::checkIsValid(void)
{
	static const auto isValidIPPart([](const QLineEdit *pLineEdit)->bool
	{
		return pLineEdit->text().toUInt() < 256;
	});

	static const auto isValidPort([](const QLineEdit *pLineEdit)->bool
	{
		return pLineEdit->text().toUInt() < 0x00010000;
	});

	if (m_pUi->LocalHostRadios->isChecked())
		return isValidPort(m_pUi->Port);
	else
		return isValidIPPart(m_pUi->IPP1) && isValidIPPart(m_pUi->IPP2)
		&& isValidIPPart(m_pUi->IPP3) && isValidIPPart(m_pUi->IPP4) && isValidPort(m_pUi->Port);
}
#include "CilentWindow.h"

CilentWindow::CilentWindow(QWidget * parent)
	: QMainWindow(parent), beClosed(false)
{
	m_pUi = new Ui::CilentWidget();
	m_pUi->setupUi(this);

	m_pCilent = new Cilent();

	m_pUi->Disconnect->setEnabled(false);
	m_pUi->SendRandomData->setEnabled(false);
	m_pUi->SendMessage->setEnabled(false);

	connect(m_pCilent, SIGNAL(pushMsg(const QString)), this, SLOT(showMsg(const QString)));
	connect(m_pUi->Connect, SIGNAL(clicked()), this, SLOT(connectToHost()));
	connect(m_pUi->Close, SIGNAL(clicked()), this, SLOT(close()));

	connect(m_pUi->Disconnect, SIGNAL(clicked()), this, SLOT(disconnectFromHost()));
	connect(m_pUi->SendMessage, SIGNAL(clicked()), this, SLOT(sendMsg()));
	connect(m_pUi->SendRandomData, SIGNAL(clicked()), this, SLOT(sendRandomData()));
}

void CilentWindow::showMsg(const QString msg)
{
	m_pUi->Log->insertPlainText(msg);
}

void CilentWindow::connectToHost(void)
{


	m_pUi->Connect->setEnabled(false);
	m_pUi->Close->setEnabled(false);

	
}

void CilentWindow::connected(void)
{
	m_pUi->Close->setEnabled(true);
	m_pUi->SendMessage->setEnabled(true);
	m_pUi->SendRandomData->setEnabled(true);
	m_pUi->Disconnect->setEnabled(true);
}

void CilentWindow::disconnectFromHost(void)
{
	m_pUi->Disconnect->setEnabled(false);
	m_pUi->SendRandomData->setEnabled(false);
	m_pUi->SendMessage->setEnabled(false);
	m_pUi->Close->setEnabled(false);
	connect(m_pCilent, SIGNAL(disconnected()), this, SLOT(disconnected()));
	m_pCilent->disconnectFromHost();
}

void CilentWindow::disconnected()
{
	if (beClosed)
	{
		this->close();
	}

	disconnect(m_pCilent, SIGNAL(disconnected()));
	m_pUi->Connect->setEnabled(true);
	m_pUi->Close->setEnabled(true);
}

void CilentWindow::close(void)
{
	if (m_pCilent->state() == QAbstractSocket::UnconnectedState)
	{
		this->close();
	}

	m_pUi->Disconnect->setEnabled(false);
	m_pUi->SendRandomData->setEnabled(false);
	m_pUi->SendMessage->setEnabled(false);
	m_pUi->Close->setEnabled(false);
	beClosed = true;
	connect(m_pCilent, SIGNAL(disconnected()), this, SLOT(disconnected()));
	m_pCilent->disconnectFromHost();
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
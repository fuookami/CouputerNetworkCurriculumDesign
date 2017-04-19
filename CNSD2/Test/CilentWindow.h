#pragma once

#include "ui_CilentWidget.h"
#include "Cilent.h"
#include <QtWidgets/QMainWindow>

class CilentWindow : public QMainWindow
{
	Q_OBJECT;

public:
	CilentWindow(QWidget *parent = nullptr);

private slots:
	void showMsg(const QString);
	void sendRandomData(void);
	void sendMsg(void);
	void connectToHost(void);
	void connectSucceed(void);
	void disconnectFromHost(void);
	void disconnectSucceed(void);
	void close(void);

private:
	bool checkIsValid(void);

private:
	Ui::CilentWidget *m_pUi;
	Cilent *m_pCilent;
	bool beClosed;
};
	
#pragma once

#include "ui_ServerWidget.h"
#include "Server.h"
#include <array>
#include <QtWidgets/QMainWindow>

class ServerWindow : public QMainWindow
{
	Q_OBJECT;

public:
	ServerWindow(QWidget *parent = nullptr);

private slots:
	void closeClickSlot(void);
	void listenClickSlot(void);
	void stopListenClickSlot(void);
	void serverClosed(void);
	void showMsg(const QString msg);

private:
	bool checkIsValid(void);
	
private:
	Ui::ServerWidget *ui;
	Server *server;
	bool beClosed;
};
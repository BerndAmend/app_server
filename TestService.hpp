#pragma once

#include "Service.hpp"
#include <QObject>
#include <QTimer>

class TestService : public QObject, public Service
{
	Q_OBJECT
public:
	TestService();

	// Service interface
protected:
	void disconnected(int client) override;

public slots:
	void doSomething();

private:
	int _counter = 0;
	int _notifyAdress = 0;
	QTimer *_timer = new QTimer(this);
};

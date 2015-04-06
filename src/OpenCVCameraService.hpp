#pragma once

#include "Service.hpp"
#include <QObject>
#include <QTimer>
#include <future>
#include <map>
#include <set>

class OpenCVCameraService : public QObject, public Service
{
	Q_OBJECT
public:
	OpenCVCameraService();

	// Service interface
protected:
	void disconnected(int client) override;

	void addClient(int id, int camera);
	void removeClient(int id);
	void removeClient(int id, int camera);
	std::set<int> getClients(int camera);

private:
	int _counter = 0;
	int _notifyAdress = 0;
	std::map<int, std::future<void>> _cameraThreads;
	std::mutex _receiverClientsMutex;
	std::map<int, std::set<int>> _receiverClients;
};

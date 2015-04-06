#include "OpenCVCameraService.hpp"
#include <iostream>
#include <QJsonDocument>
#include <opencv2/opencv.hpp>
#include <QVariant>

OpenCVCameraService::OpenCVCameraService()
	: Service("OpenCVCamera")
{
	registerCommand("open", [this](int client, const QJsonObject &args, QJsonObject &result) {
		std::lock_guard<std::mutex> lock(_receiverClientsMutex);
		// id, width, height, fps
		int id = args["id"].toInt();
		int width = args["width"].toInt();
		int height = args["height"].toInt();
		int fps = args["fps"].toInt();

		addClient(client, id);

		if(_cameraThreads.count(id) == 0) {
			// open camera
			auto cap = std::make_shared<cv::VideoCapture>(id); // open the default camera
			if(!cap->isOpened()) { // check if we succeeded
				result["error"] = 10;
				return;
			}

			if(!cap->set(CV_CAP_PROP_FRAME_WIDTH, width))
				std::cerr << "Couldn't set width\n";

			if(!cap->set(CV_CAP_PROP_FRAME_HEIGHT, height))
				std::cerr << "Couldn't set height\n";

			if(!cap->set(CV_CAP_PROP_FPS, fps))
				std::cerr << "Couldn't set framerate\n";
			_cameraThreads[id] = std::async(std::launch::async, [this, id, cap]() {
				cv::Mat frame;
				QJsonObject msg;
				QByteArray data;
				for(;;)
				{
					(*cap) >> frame; // get a new frame from camera
					msg["width"] = frame.cols;
					msg["height"] = frame.rows;
					data = QByteArray::fromRawData((const char*)frame.data, frame.step[0] * frame.rows);
					data = data.toBase64();
					msg["data"] = QString::fromUtf8(data);

					auto clients = getClients(id);
					for(auto client : clients) {
						notifyFromOtherThread(client, "image", msg);
					}
				}
			});
		}
	});

	registerCommand("close", [this](int client, const QJsonObject &args, QJsonObject &result) {
		int id = args["id"].toInt();
		std::lock_guard<std::mutex> lock(_receiverClientsMutex);
		removeClient(client, id);
	});
}

void OpenCVCameraService::disconnected(int client)
{
	std::lock_guard<std::mutex> lock(_receiverClientsMutex);
	removeClient(client);
}

void OpenCVCameraService::addClient(int id, int camera)
{

	_receiverClients[camera].insert(id);
}

void OpenCVCameraService::removeClient(int id)
{
	for(auto &cam : _receiverClients)
		cam.second.erase(id);
}

void OpenCVCameraService::removeClient(int id, int camera)
{
	_receiverClients[camera].erase(id);
}

std::set<int> OpenCVCameraService::getClients(int camera)
{
	std::lock_guard<std::mutex> lock(_receiverClientsMutex);
	return _receiverClients[camera];
}

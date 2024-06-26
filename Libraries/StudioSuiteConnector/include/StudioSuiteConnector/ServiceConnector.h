#pragma once

#include <string>

#include "OTCore/JSON.h"

#include <qobject.h>

class ServiceConnector
{
public:
    static ServiceConnector& getInstance()
    {
        static ServiceConnector instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    void setServiceURL(const std::string &url) { serviceURL = url; }
    void setMainObject(QObject* main) { mainObject = main; }

    void sendExecuteRequest(ot::JsonDocument& doc);

private:
    ServiceConnector() : mainObject(nullptr) {};
    ~ServiceConnector() {};

    std::string serviceURL;
    QObject* mainObject;
};

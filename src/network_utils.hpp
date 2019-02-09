#ifndef NETWORK_SQUEEZE
#define NETWORK_SQUEEZE

#include <iostream>
#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "../lib/rapidjson/include/rapidjson/document.h"
#include "../lib/rapidjson/include/rapidjson/stringbuffer.h"
#include "../lib/rapidjson/include/rapidjson/writer.h"
#define URL "http://179.106.230.91:8080"

typedef struct
{
    std::string deviceMac;
    std::string deviceName;
} Device;

typedef struct
{
    std::string lastName;
    std::string firstName;
    bool auth;
} Employee;

class NetworkUtils
{
    private:
    RestClient::Connection *_conn;
    std::string _token;
    std::string _url;
    Device _device;

    public:
    NetworkUtils ();
    NetworkUtils (std::string, Device);
    NetworkUtils (std::string, std::string);

    int getAuthToken (std::string &);
    int checkEmployee (Employee &);
    std::string getUrl ();
};

#endif
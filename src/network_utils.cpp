#include "network_utils.hpp"

using namespace std;
using namespace rapidjson;


NetworkUtils::NetworkUtils (string url, Device device)
{
    RestClient::init ();
    _url = url;
    _device = device;
}

NetworkUtils::NetworkUtils ()
{
}
NetworkUtils::NetworkUtils (string url, string token)
{
    _url = url;
    _token = token;
}

int NetworkUtils::getAuthToken (std::string &token)
{
    string jsonDevice = "{ \"data\":{\"deviceName\":\"" + _device.deviceName +
                        "\",\"deviceMac\":\"" + _device.deviceMac + "\"} }";
#ifdef EBUG
    cout << jsonDevice << endl;
#endif
    _conn = new RestClient::Connection (_url);
    RestClient::HeaderFields headers;
    headers["Content-Type"] = "application/json";
    _conn->SetHeaders (headers);
    RestClient::Response r = _conn->post ("/device/authenticate", jsonDevice);
    Document document;
    document.Parse (r.body.c_str ());
#ifdef EBUG
    cout << r.body << endl;
#endif
    if (!document.HasMember ("data"))
    {
        cout << "Parsing json problem" << endl;
        return 500;
    }

    const Value &data = document["data"];

    if (data.HasMember ("token"))
    {
        _token = data["token"].GetString ();
        return r.code;
    }
    return 403;
}

int NetworkUtils::checkEmployee (Employee &employee)
{
    string jsonEmployee = "{ \"data\":{\"firstName\":\"" + employee.firstName +
                          "\",\"lastName\":\"" + employee.lastName + "\"} }";
#ifdef EBUG
    cout << jsonEmployee << endl;
#endif
    _conn = new RestClient::Connection (_url);
    RestClient::HeaderFields headers;
    headers["Content-Type"] = "application/json";
    headers["Authorization"] = "Bearer " + _token;
    _conn->SetHeaders (headers);

    RestClient::Response r = _conn->post ("/employees/face", jsonEmployee);
    if (r.code == 200) employee.auth = true;
    return r.code;
}

string NetworkUtils::getUrl ()
{
    return _url;
}

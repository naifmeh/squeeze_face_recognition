#ifndef VIDEO_KAFKA_CONSUMER
#define VIDEO_KAFKA_CONSUMER

#include <chrono>
#include <ctime>
#include <iostream>

#include <cppkafka/cppkafka.h>
#include <opencv2/opencv.hpp>

#include "face_extraction.hpp"
#include "face_tracking.hpp"
#include "network_utils.hpp"
#include "utils.hpp"

#include "../lib/rapidjson/include/rapidjson/document.h"
#include "../lib/rapidjson/include/rapidjson/stringbuffer.h"
#include "../lib/rapidjson/include/rapidjson/writer.h"
#include "base64.h"

class VideoConsumer
{

    private:
    std::string _cameraId = "";

    std::string _topic;
    std::string _brokers;
    std::string _groupid;
    std::string _token;
    NetworkUtils _network;
    bool shouldUseNetwork = false;
    cppkafka::Consumer *_consumer = NULL;
    cppkafka::Producer *_producer = NULL;
    cppkafka::Configuration _configuration;
    cppkafka::Configuration _configProd;
    cppkafka::MessageBuilder *_messageBuilder;

    public:
    VideoConsumer (std::string brokers, std::string topic, std::string groupid);
    VideoConsumer (std::string brokers, std::string topic, std::string groupid, std::string cameraid);
    ~VideoConsumer ();
    void setConsumer ();
    void setConsumer (std::string);
    void setProducer ();
    void pollConsumer ();
    void getVideoFrame ();
};

#endif
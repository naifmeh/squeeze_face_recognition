#include "video_consumer.hpp"

using namespace std;
using namespace cppkafka;
using namespace rapidjson;

#define UNKNOWN_PATH_FOLDER "./unknown_faces/"
#define THRESHOLD_DETECT 0.8

cv::Mat mat;
cv::Mat mat2;
Utils resultActionUtils;

/* Util function to infer the face using the python classifier */
void inferFaceStream (string pathToFrame, string &name)
{
#ifdef VERBOSE
    cout << "Inferring video faces..." << endl;
#endif
    string infercommand = "python classifier.py  infer ./generated-embeddings/classifier.pkl ";
    string pathToComm = pathToFrame;
    infercommand += pathToComm;
    char *command = new char[infercommand.length () + 1];
    std::strcpy (command, infercommand.c_str ());
    FILE *pip = popen (command, "r");
    if (!pip)
    {
        cerr << "Could not start command" << endl;
    }

    std::array<char, 128> buffer;
    string resultat;
    while (fgets (buffer.data (), 128, pip) != NULL)
    {
        resultat += buffer.data ();
    }
    Utils parser;
    std::vector<string> parserResult = parser.splitText (resultat, ' ');
    float accuracy = strtof (parserResult[1].c_str (), 0);

    /*Comparing to a threshold */
    if (accuracy <= THRESHOLD_DETECT)
    {
        cout << "Result not accurate enough" << endl;
        cout << "******** Detected " << parserResult[0] << " with " << accuracy * 100 << " accuracy." << endl;
        name = "unknown";
        cv::Mat unknownFace = cv::imread (pathToFrame);
        std::time_t timestamp = std::time (nullptr);
        cv::imwrite (UNKNOWN_PATH_FOLDER + std::to_string (timestamp) + ".jpg", unknownFace);
        resultActionUtils.recognitionBadResultAction(nullptr);
    }
    else
    {
        cout << "Detected " << parserResult[0] << " with " << accuracy * 100 << " % accuracy." << endl;
        name = parserResult[0];
        resultActionUtils.recognitionOkResultAction(nullptr);
    }
    cout << endl;
    cout << endl;
}

VideoConsumer::VideoConsumer (std::string brokers, std::string topic, std::string groupid)
{
    _brokers = brokers;
    _topic = topic;
    _groupid = groupid;
    _configuration = { { "metadata.broker.list", _brokers },
                       { "max.partition.fetch.bytes", "2097152" },
                       { "group.id", _groupid } };
}

VideoConsumer::VideoConsumer (std::string brokers, std::string topic, std::string groupid, std::string cameraid)
{
    _brokers = brokers;
    _topic = topic;
    _groupid = groupid;
    _cameraId = cameraid;
    _configuration = { { "metadata.broker.list", _brokers },
                       { "max.partition.fetch.bytes", "2097152" },
                       { "group.id", _groupid } }; //Kafka consumer configuration
}

/* Setting consumer without REST queries */
void VideoConsumer::setConsumer ()
{
#ifdef EBUG
    cout << "Setting up consumer without REST queries..." << endl;
#endif
    _consumer = (new Consumer (_configuration));
    _consumer->subscribe ({ _topic });
}

/* Setting consumer with REST queries */
void VideoConsumer::setConsumer (string token)
{
#ifdef EBUG
    cout << "Setting up consumer with REST queries..." << endl;
#endif
    _consumer = (new Consumer (_configuration));
    _consumer->subscribe ({ _topic });
    _token = token;
    _network = *new NetworkUtils ("localhost:8080", _token);
    shouldUseNetwork = true;
}

/*Setting the result producer */
void VideoConsumer::setProducer ()
{
#ifdef EBUG
    cout << "Setting up result producer..." << endl;
#endif
    string topic = "video-result-topic";
    _configProd = { { "metadata.broker.list", _brokers } };
    _producer = new Producer (_configProd);
    _messageBuilder = new MessageBuilder (topic);
}

/* Polling the consumer and deserializing the result */
void VideoConsumer::pollConsumer ()
{
    Message msg = _consumer->poll ();
    if (!msg)
    {
        cerr << "No message received for camera " << _cameraId << endl;
        return;
    }
    if (msg.get_error ())
    {
        if (!msg.is_eof ())
        {
            cerr << "[+] Received error notification: " << msg.get_error () << endl;
        }
        return;
    }

    Document document;
    string jsonPayload = "";
    for (auto i = msg.get_payload ().begin (); i != msg.get_payload ().end (); i++)
    {
        jsonPayload += *i;
    }


    document.Parse (jsonPayload.c_str ());
    if (document.HasMember ("rows") && document.HasMember ("cols") && document.HasMember ("data"))
    {
        int rows = document["rows"].GetInt ();
        int cols = document["cols"].GetInt ();
        int type = document["type"].GetInt ();
        string cameraId = document["cameraId"].GetString ();
        if (_cameraId != cameraId && cameraId != "")
        {
            return;
        }

        int64 timestamp = document["timestamp"].GetInt64 ();
        std::time_t result = std::time (nullptr);
        if (timestamp / 1000 < result - 1)
        {
            cout << "HUGE LATENCY -- WARNING > 1 seconds" << endl;
            cout << "Rebalancing..." << endl;
            while (timestamp / 1000 < result)
            {
                msg = _consumer->poll ();
                jsonPayload = "";
                for (auto i = msg.get_payload ().begin (); i != msg.get_payload ().end (); i++)
                {
                    jsonPayload += *i;
                }
                document.Parse (jsonPayload.c_str ());
                rows = document["rows"].GetInt ();
                cols = document["cols"].GetInt ();
                type = document["type"].GetInt ();
                timestamp = document["timestamp"].GetInt64 ();
            }
            cout << "Rebalanced stream" << endl;
        }
        string data = document["data"].GetString ();
        std::vector<BYTE> decodedBytes = base64_decode (data);

        stringstream ss;
        for (int i = 0; i < decodedBytes.size (); i++)
        {
            ss << decodedBytes[i];
        }
        string decoded_data = ss.str ();
        mat.release ();
        mat = *new cv::Mat (rows, cols, type, (void *)decoded_data.data ());
        mat2 = mat.clone ();
    }
    else
    {
        return;
    }
}


void VideoConsumer::getVideoFrame ()
{

    this->pollConsumer ();
    if (mat2.empty () || mat2.rows == 0 || mat2.cols == 0) return;
#ifdef ISPLAY_STREAM
    cv::imshow ("Recognition", mat2);
    cv::waitKey (1);
#endif

    FaceExtracted faceModule = FaceExtracted (mat2, "./outputFrame/frame.jpg");
    std::chrono::high_resolution_clock::time_point t1Face = std::chrono::high_resolution_clock::now ();
    faceModule.detectFaces ();
    std::chrono::high_resolution_clock::time_point t2Face = std::chrono::high_resolution_clock::now ();
    auto durationFace = std::chrono::duration_cast<std::chrono::milliseconds> (t2Face - t1Face).count ();
#ifdef VERBOSE
    cout << "Faces detection took " << (float)(durationFace / 1000.f) << " seconds" << endl;
#endif
    std::vector<cv::Rect> facesRect;
    faceModule.getFacesRectangle (facesRect);
    if (facesRect.size () > 0)
    {
        faceModule.generateLandmark ();
        faceModule.getRotatedFaces ();
        faceModule.generateThumbnails (96);
        std::vector<FaceTracking> trackers;
        std::vector<string> outputs;
        faceModule.getOutputVector (outputs);
        std::vector<string> names;
        string name;
        for (int i = 0; i < facesRect.size (); i++)
        {
            if (i == 0)
                trackers.push_back (FaceTracking (mat2, facesRect[i]));
            else
                trackers.push_back (FaceTracking (trackers[i - 1].getFrame (), facesRect[i]));
            trackers[i].initTracker ();
        }
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now ();
#pragma omp parallel for // num_threads(NUM_THREADS)
        for (int i = 0; i < facesRect.size (); i++)
        {
            inferFaceStream (outputs[i], name);
            names.push_back (name);
        }
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now ();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (t2 - t1).count ();
#ifdef VERBOSE
        cout << "Faces recognition took " << (float)(duration / 1000.f) << " seconds" << endl;
#endif

        if (shouldUseNetwork)
        {
            for (int i = 0; i < names.size (); i++)
            {
                if (names[i] != "unknown")
                {
                    Utils parser;
                    std::vector<string> parserResult = parser.splitText (name, '-');
                    Employee employee = { parserResult[1], parserResult[0], false };
                    int code = _network.checkEmployee (employee);
                    string msg = names[i] + " recognized by camera : " + _cameraId;
                    std::string timestamp = std::to_string (std::time (nullptr));

                    _messageBuilder->key (timestamp);
                    _messageBuilder->payload (msg);

                    _producer->produce (*_messageBuilder);
                    if (code == 200)
                    {
                        cout << names[i] << " authorized." << endl;
                        resultActionUtils.authOkResultAction(nullptr);
                    } else {
                        cout << names[i] << " not authorized." << endl;
                        resultActionUtils.authBadResultAction(nullptr);
                    }
                }
                else
                {
                    string msg = "Unauthorized person detected by camera : " + _cameraId;
                    std::string timestamp = std::to_string (std::time (nullptr));

                    _messageBuilder->key (timestamp);
                    _messageBuilder->payload (msg);
                    _producer->produce (*_messageBuilder);
                }
            }
        }
        bool isTrackerOk = true;
#ifdef VERBOSE
        cout << "Tracking..." << endl;
#endif
        while (isTrackerOk)
        {
            this->pollConsumer ();
            if (mat2.empty () || mat2.rows == 0 || mat2.cols == 0) break;
            for (int i = 0; i < trackers.size (); i++)
            {
                if (i == 0)
                    trackers[0].setFrame (mat2);
                else
                    trackers[i].setFrame (trackers[i - 1].getFrame ());
                trackers[i].updateTracker ();
                if (names.size () > 0) trackers[i].setName (names[i]);

                isTrackerOk = trackers[i].isTrackingOk ();
                if (!isTrackerOk) break;
            }
#ifdef ISPLAY_STREAM
            cv::imshow ("Video", trackers[trackers.size () - 1].getFrame ());
            cv::waitKey (1);
#endif
        }
    }
    //--------
}

VideoConsumer::~VideoConsumer ()
{
    RestClient::disable ();
}
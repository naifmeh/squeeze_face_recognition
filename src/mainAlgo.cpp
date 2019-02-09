#include <chrono>
#include <cstdlib> /* For system calls */
#include <iostream>
#include <thread>

#include <opencv2/videoio.hpp>

#include "face_extraction.hpp"
#include "face_tracking.hpp"
#include "network_utils.hpp"
#include "training_generator.hpp"
#include "utils.hpp"
#include "video_consumer.hpp"

#define NUM_THREADS 4
#define THRESHOLD 0.8
#define URL "http://179.106.230.91:8080"

using namespace cv;
using namespace std;

Device device = { "A8:B6:EA:DF:6E:44", "Raspberry" };
NetworkUtils network = NetworkUtils (URL, device);
std::vector<string> _listCameras;
string token;
string groupid;
string brokers;
Utils resultAction;

void inferFace (string pathToFrame, string &name)
{
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

#ifdef VERBOSE
    cout << "For path : " << pathToComm << endl;
#endif
    /*Comparing to a threshold */
    if (accuracy <= THRESHOLD)
    {
        cout << "Result not accurate enough" << endl;
        cout << "******** Detected " << parserResult[0] << " with " << accuracy * 100 << " accuracy." << endl;
        name = "unknown";
        resultAction.recognitionBadResultAction(nullptr);
    }
    else
    {
        cout << "Detected " << parserResult[0] << " with " << accuracy * 100 << " % accuracy." << endl;
        name = parserResult[0];
        resultAction.recognitionOkResultAction(nullptr);
    }
    cout << endl;
    cout << endl;
}

void getAuthToken ()
{

    int rep = network.getAuthToken (token);
    if (rep != 200)
    {
        cout << "[getAuthToken] : error while generating token" << endl;
        perror ("Token problem");
        exit (EXIT_FAILURE);
    }
}

void *streamLogic (void *cameraId)
{
    string cameraid = *(string *)cameraId;
    VideoConsumer consumer (brokers, "video-stream-topic", groupid, cameraid);
    consumer.setConsumer (token);
    consumer.setProducer ();
    while (1)
    {
        consumer.getVideoFrame ();
    }
}

bool authorizeFace (string name)
{
    Utils parser;
    std::vector<string> parserResult = parser.splitText (name, '-');
    Employee employee = { parserResult[1], parserResult[0], false };

    int reps = network.checkEmployee (employee);
    if (reps == 200)
    {
        cout << name << " authorized." << endl;
        return true;
    }
    cout << name << " not authorized" << endl;
    return false;
}

int main (int argc, char **argv)
{
    CommandLineParser parser (argc, argv,
                              "{help h||}"
                              "{infer | false | Infer the input}"
                              "{video | | Video stream }"
                              "{stream | | Live remote video stream}"
                              "{train | false | Train the classifier}"
                              "{align | false | Align images}"
                              "{webcam | | Webcam}"
                              "{align_folder_in |./training-images/ | Folder containing images to align }"
                              "{align_folder_out | ./aligned-images | Folder to contain aligned images }"
                              "{image  || image to process}"
                              "{brokers | 179.106.230.91:9092 | Broker IP }"
                              "{groupid | testId | Group consumer Id}");

    parser.about ("SqueezeCNN v0.0.1");
    if (parser.has ("help"))
    {
        parser.printMessage ();
    }

    _listCameras.push_back ("testCam");
    _listCameras.push_back ("cam2");

    string image = parser.get<string> ("image");
    string video = parser.get<string> ("video");
    bool infer = parser.get<bool> ("infer");
    bool train = parser.get<bool> ("train");
    bool align = parser.get<bool> ("align");
    groupid = parser.get<string>("groupid");
    brokers = parser.get<string>("brokers");
    string align_folder_in = parser.get<String> ("align_folder_in");
    string align_folder_out = parser.get<String> ("align_folder_out");

    std::vector<string> outputAlignement;
    if (align)
    {
#ifdef VERBOSE
        cout << "Aliging files..." << endl;
#endif
        boost::filesystem::path dirpath (align_folder_in);
        /* Recuperating list of files into a map */
        Utils utils;
        utils.listSubPath (dirpath);
        utils.generateOutputPath (utils.getFileNames (), align_folder_out);
#ifdef EBUG
        cout << " ----------- Displaying input folder tree ------------ " << endl;
        utils.displayMap (utils.getFileNames ());
        cout << " ----------- Displaying predicted output tree ------------ " << endl;
        utils.displayMap (utils.getOutputPath ());
#endif
        std::map<string, std::vector<string>> inputs = utils.getFileNames ();

/* Generating aligned faces in output folder */
#ifdef VERBOSE
        cout << "Aligment started using " << NUM_THREADS << " threads" << endl;
#endif

#pragma omp parallel num_threads(NUM_THREADS)
        {
#pragma omp single
            for (auto const &x : inputs)
            {
                string output;
                for (auto i = x.second.begin (); i != x.second.end (); i++)
                {
                    output = "";
                    utils.generateOutputPath (x.first, align_folder_out, output);
                    string input = x.first + '/' + *i;
                    output = output + '/' + *i;
                    outputAlignement.push_back (output);
#ifdef EBUG
                    cout << "Input ----- " << input << "-------- Output ->" << output << endl;
#endif
                    FaceExtracted faceModule = FaceExtracted (input, output);
                    faceModule.detectFaces ();
                    faceModule.generateLandmark ();
                    faceModule.getRotatedFaces ();
#ifdef ISPLAY
                    faceModule.displayResult (2);
#endif
                    faceModule.generateThumbnails (96);
                }
            }
        }
    }

    /* Step 2 : case 1: Training the classifier */
    if (train)
    {
#ifdef VERBOSE
        cout << "Training option selected, starting..." << endl;
#endif

        /* Step2.1 : Data augmentation */
        string alignedPath = "./";
        alignedPath += align_folder_out;
        alignedPath += '/';
        boost::filesystem::path alignedFolderPath (alignedPath);
        Utils dataAugUtil;
        dataAugUtil.listSubPath (alignedFolderPath);
        std::map<string, std::vector<string>> dataAugInputs = dataAugUtil.getFileNames ();

#pragma omp parallel num_threads(NUM_THREADS)
        {
#pragma omp single
            for (auto const &x : dataAugInputs)
            {
                string output;

                for (auto i = x.second.begin (); i != x.second.end (); i++)
                {
                    output = x.first + '/' + *i;
                    cout << output << endl;
                    TrainingGenerator generator = TrainingGenerator (output, output);
                    // generator.rotateImage(50,1);
                    generator.flipImageHorizontally ();
                    generator.generateGaussianNoise (10);
                }
            }
        }

#ifdef VERBOSE
        cout << "Aligment done." << endl;
        cout << "Generating embeddings in folder generated-embeddings..." << endl;
#endif
        /* Generating embeddings */
        int repsSystem = system ("./openface/batch-represent/main.lua -outDir "
                                 "./generated-embeddings/ -data ./aligned-images/");
#ifdef VERBOSE
        cout << "Command done. Check previous log if errors occured" << endl;
#endif

        /* Training the network */
        repsSystem = system ("./openface/demos/classifier.py train ./generated-embeddings/");


        repsSystem = system("rm -rf ./aligned-images/*");
    }


    if (parser.has ("video"))
    {
        getAuthToken ();

        VideoCapture cap;
        if (parser.has ("webcam"))
        {
            cap.open (0);
            cap.set (CAP_PROP_FRAME_WIDTH, 1280);
            cap.set (CAP_PROP_FRAME_HEIGHT, 720);
        }
        else
            cap.open (video);
        if (!cap.isOpened ())
        {
            cerr << "Error opening video stream" << endl;
        }
        int repsSystem = system ("mkdir outputFrame");

        while (1)
        {
            Mat frame;
            cap >> frame;
            if (frame.empty ()) break;

            FaceExtracted faceModule = FaceExtracted (frame, "./outputFrame/frame.jpg");
            faceModule.detectFaces ();
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
                        trackers.push_back (FaceTracking (frame, facesRect[i]));
                    else
                        trackers.push_back (FaceTracking (trackers[i - 1].getFrame (), facesRect[i]));

                    trackers[i].initTracker ();
                }
                std::chrono::high_resolution_clock::time_point t1 =
                std::chrono::high_resolution_clock::now ();
#pragma omp parallel for // num_threads(NUM_THREADS)
                for (int i = 0; i < facesRect.size (); i++)
                {
                    inferFace (outputs[i], name);
                    names.push_back (name);
                }
                std::chrono::high_resolution_clock::time_point t2 =
                std::chrono::high_resolution_clock::now ();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (t2 - t1).count ();
                cout << "Faces recognition took " << (float)(duration / 1000.f) << " seconds" << endl;
                ;

                for (int i = 0; i < names.size (); i++)
                {
                    if (names[i] != "unknown") authorizeFace (names[i]);
                }

                bool isTrackerOk = true;
#ifdef VERBOSE
                cout << "Tracking..." << endl;
#endif
                while (isTrackerOk)
                {
                    cap >> frame;
                    if (frame.empty ()) break;
                    for (int i = 0; i < trackers.size (); i++)
                    {
                        if (i == 0)
                            trackers[0].setFrame (frame);
                        else
                            trackers[i].setFrame (trackers[i - 1].getFrame ());
                        trackers[i].updateTracker ();
                        if (names.size () > 0) trackers[i].setName (names[i]);

                        isTrackerOk = trackers[i].isTrackingOk ();
                        if (!isTrackerOk) break;
                    }
                    imshow ("Video", trackers[trackers.size () - 1].getFrame ());
                    waitKey (1);
                }
            }
            else
            {
                imshow ("Video", frame);
                waitKey (2);
            }

            if (frame.empty ()) break;
        }
        repsSystem = system ("rm ./outputFrame/*");
        cap.release ();
        destroyAllWindows ();
    }

    if (parser.has ("stream"))
    {
        getAuthToken ();
        #ifdef MULTITHREAD
        pthread_t tids[_listCameras.size ()];
        for (int i = 0; i < _listCameras.size (); i++)
        {
            pthread_create (&tids[i], NULL, streamLogic, (void *)&_listCameras[i]);
        }
        for (int i = 0; i < _listCameras.size (); i++)
        {
            pthread_join (tids[i], NULL);
        }
        #else
        VideoConsumer consumer (brokers, "video-stream-topic", groupid, "testCam");
        consumer.setConsumer (token);
        consumer.setProducer ();
        while (1)
        {
            consumer.getVideoFrame ();
        }
        #endif

    }

    return 0;
}

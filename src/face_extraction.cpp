#include "face_extraction.hpp"


using namespace std;
using namespace cv;
using namespace cv::face;

#define PI 3.1415926535
#define COLOR_LANDMARK Scalar (255, 100, 100)
#define RESIZE_SCALE 1 /* Resizing scale */

// TODO: generate folder if not found

FaceExtracted::FaceExtracted (Mat frame)
{
    motherFrame = frame;
    face_cascade_name =
    "/home/naif/Documents/opencv/data/haarcascades/haarcascade_frontalface_alt.xml";
    window_name = "Face extractor";
}

FaceExtracted::FaceExtracted (std::string input, std::string output)
{
    face_cascade_name =
    "/home/naif/Documents/opencv/data/haarcascades/haarcascade_frontalface_alt.xml";
    window_name = "Face extractor";
    _inputPath = input;
    motherFrame = imread (_inputPath);
    if (motherFrame.empty ())
    {
        cout << "Empty frame" << endl;
    }
    _outputPath = output;
}

FaceExtracted::FaceExtracted (cv::Mat frame, std::string output)
{
    face_cascade_name =
    "/home/naif/Documents/opencv/data/haarcascades/haarcascade_frontalface_alt.xml";
    window_name = "Face extractor";
    motherFrame = frame;
    _outputPath = output;
}

void FaceExtracted::detectFaces ()
{
#ifdef EBUG
    cout << "[FaceExtracted]::DetectFaces" << endl;
#endif
    Mat frame_gray;
    cvtColor (motherFrame, frame_gray, COLOR_BGR2GRAY);
    equalizeHist (frame_gray, frame_gray);

    if (!_faceCascade.load (face_cascade_name)) return;

    _faceCascade.detectMultiScale (frame_gray, _faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size (60, 60));
    Mat faceROI;

    for (size_t i = 0; i < _faces.size (); i++)
    {
        _faces[i].width = _faces[i].width + PADDING;
        _faces[i].height = _faces[i].height + PADDING;
        faceROI = motherFrame (_faces[i]);
        facesROI.push_back (faceROI);
    }
}

std::vector<cv::Rect> FaceExtracted::detectSingleFace (cv::Mat aligned_face)
{
    Mat frame_gray;
    cvtColor (aligned_face, frame_gray, COLOR_BGR2GRAY);
    equalizeHist (frame_gray, frame_gray);

    _faceCascade.load (face_cascade_name);
    std::vector<cv::Rect> detectedFaces;
    _faceCascade.detectMultiScale (frame_gray, detectedFaces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size (60, 60));
    return detectedFaces;
}

void FaceExtracted::saveCroppedFaces (string path)
{
    for (size_t i = 0; i < facesROI.size (); i++)
    {
        stringstream ssfn;
        ssfn << path << '/' << i << ".jpg";
        string filename = ssfn.str ();
        imwrite (filename, facesROI[i]);
    }
}

void FaceExtracted::saveAlignedFace (Mat face, string path, int uuid)
{
    // Pb multiple faces
    if (uuid > 0)
    {
        stringstream ssfn;
        ssfn << path << uuid << ".jpg";
        string alterPath = ssfn.str ();
        _outputsVector.push_back (alterPath);
        cout << alterPath << endl;
        imwrite (alterPath, face);
    }
    else
    {
        imwrite (path, face);
        _outputsVector.push_back (path);
    }
}

void FaceExtracted::getOutputVector (std::vector<string> &outputVect)
{
    outputVect = _outputsVector;
}

void FaceExtracted::generateThumbnails (int size = 96)
{
    for (size_t i = 0; i < _aligned_cropped_faces.size (); i++)
    {
        Mat resized;
        resize (_aligned_cropped_faces[i], resized, Size (size, size));
        saveAlignedFace (resized, _outputPath, i);
    }
}

void FaceExtracted::drawPolyline (const int index, const int start, const int end, bool isClosed = false)
{
    vector<Point> points;
    for (int i = start; i <= end; i++)
    {
        points.push_back (Point ((_landmarks[index])[i].x, (_landmarks[index])[i].y));
    }
    polylines (motherFrame, points, isClosed, COLOR_LANDMARK, 2, 16);
}

int FaceExtracted::generateLandmark ()
{
    Ptr<Facemark> facemark = FacemarkLBF::create ();
    facemark->loadModel ("models/lbfmodel.yaml");

    bool success = facemark->fit (motherFrame, _faces, _landmarks);

    if (success)
        return 0;
    else
        return -1;
}


cv::Mat FaceExtracted::getMotherFrame ()
{
    return motherFrame;
}
void FaceExtracted::generateFaceLine ()
{
    for (int i = 0; i < _landmarks.size (); i++)
        line (motherFrame, _landmarks[i][45], _landmarks[i][36], COLOR_LANDMARK, 4);
}

void FaceExtracted::getRotationMatrix ()
{
#ifdef EBUG
    cout << "[FaceExtracted]::getRotationMatrix" << endl;
#endif
    _rotation_mat.reserve (facesROI.size ());
    for (int i = 0; i < facesROI.size (); i++)
    {
        double dY = _landmarks[i][42].y - _landmarks[i][36].y;
        double dX = _landmarks[i][42].x - _landmarks[i][36].x;
        double eye_angle = atan2 (dY, dX) * (180 / PI);
        Point2f center (facesROI[i].cols / 2., facesROI[i].rows / 2.);
        _rotation_mat.push_back (getRotationMatrix2D (center, eye_angle, RESIZE_SCALE));
#ifdef EBUG
        cout << _rotation_mat[i] << endl;
#endif
    }
}

void FaceExtracted::getRotatedFaces ()
{
#ifdef EBUG
    cout << "[FaceExtracted]::getRotatedFaces" << endl;
#endif
    this->getRotationMatrix ();
    _aligned_faces.reserve (facesROI.size ());
    for (int i = 0; i < facesROI.size (); i++)
    {
        _aligned_faces.push_back (Mat::zeros (facesROI[i].rows, facesROI[i].cols, facesROI[i].type ()));
        warpAffine (facesROI[i], _aligned_faces[i], _rotation_mat[i],
                    Size (facesROI[i].cols, facesROI[i].rows));
        // std::vector<cv::Rect> faces_aligned = this->detectSingleFace(_aligned_faces[i]);
        // if(faces_aligned.size() > 0) {
        //  Mat croppedFace = _aligned_faces[i](faces_aligned[0]);
        _aligned_cropped_faces.push_back (_aligned_faces[i]);
        //}
    }
}

void FaceExtracted::getFacesRectangle (std::vector<cv::Rect> &rects)
{
    rects = _faces;
}

void FaceExtracted::displayResult (cv::Mat matrix)
{
    while (1)
    {
        if ((waitKey (10) == 27)) break;
        imshow (window_name, matrix);
    }
}
void FaceExtracted::displayResult (int action)
{
    switch (action)
    {
    case 0:
        while (1)
        {
            if (waitKey (10) == 27) break;
            for (size_t i = 0; i < _faces.size (); i++)
            {
                Point top (_faces[i].x - PADDING, _faces[i].y - PADDING);
                Point bottom (_faces[i].x + _faces[i].width + PADDING, _faces[i].y + _faces[i].height + PADDING);
                rectangle (motherFrame, top, bottom, Scalar (255, 255, 0), 3, 8, 0);
                imshow (window_name, motherFrame);
            }
        }
        break;

    case 2: // Drawing the _landmarks
        for (int i = 0; i < _landmarks.size (); i++)
        {
            cout << "Displaying landmarks" << endl;
            // i loop through the faces, j through the _landmarks
            if (_landmarks[i].size () == 68)
            {
                cout << "Landmark size is 68" << endl;
                drawPolyline (i, 0, 16); // Jaw line
                drawPolyline (i, 17, 21); // Left eyebrow
                drawPolyline (i, 22, 26); // Right eyebrow
                drawPolyline (i, 27, 30); // Nose bridge
                drawPolyline (i, 30, 35, true); // Lower nose
                drawPolyline (i, 36, 41, true); // Left eye
                drawPolyline (i, 42, 47, true); // Right Eye
                drawPolyline (i, 48, 59, true); // Outer lip
                drawPolyline (i, 60, 67, true); // Inner lip
            }
            else
            {
                for (int j = 0; j < _landmarks[i].size (); j++)
                {
                    circle (motherFrame, _landmarks[i][j], 3, COLOR_LANDMARK, FILLED);
                }
            }
        }
        while (1)
        {
            if (waitKey (10) == 27) break;
            imshow (window_name, motherFrame);
        }

    default:
        return;
    }
}

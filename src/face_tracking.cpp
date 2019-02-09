#include "face_tracking.hpp"

using namespace std;
using namespace cv;

FaceTracking::FaceTracking (cv::Mat frame, cv::Rect face)
{
    _frame = frame;
    _face = face;
}

cv::Rect FaceTracking::getFace ()
{
    return _face;
}

cv::Mat FaceTracking::getFrame ()
{
    return _frame;
}

void FaceTracking::setFace (cv::Rect face)
{
    _face = face;
}

void FaceTracking::setFrame (cv::Mat frame)
{
    _frame = frame;
}

void FaceTracking::setName (std::string name)
{
    putText (_frame, name, Point (_face.x, _face.y - 10), FONT_HERSHEY_SIMPLEX, 0.75, Scalar (0, 0, 255), 2);
}

bool FaceTracking::isTrackingOk ()
{
    return _isTrackingOk;
}

void FaceTracking::initTracker ()
{
    _tracker = TrackerKCF::create ();


    _tracker->init (_frame, _face);
}

void FaceTracking::updateTracker ()
{
    Rect2d rect = Rect2d (_face);
    _isTrackingOk = _tracker->update (_frame, rect);
    _face = rect;
    if (_isTrackingOk)
    {
        rectangle (_frame, rect, Scalar (255, 0, 0), 2, 1);
    }
    else
    {
        cout << "Lost tracking" << endl;
    }
}
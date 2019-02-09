#ifndef FACE_TRACKING_SQUEEZE
#define FACE_TRACKING_SQUEEZE

#include <iostream>
/* Opencv */
#include <opencv2/core/ocl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/tracking/tldDataset.hpp>
#include <opencv2/tracking/tracker.hpp>


class FaceTracking
{
    private:
    cv::Mat _frame;
    cv::Rect2d _face;
    cv::Ptr<cv::Tracker> _tracker;
    bool _isTrackingOk;

    public:
    FaceTracking (cv::Mat, cv::Rect);
    cv::Mat getFrame ();
    cv::Rect getFace ();
    void setFace (cv::Rect);
    void setFrame (cv::Mat);
    void setName (std::string);

    /* Methods */
    bool isTrackingOk ();
    void initTracker ();
    void updateTracker ();
};

#endif // FACE_TRACKING_SQUEEZE
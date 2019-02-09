#ifndef FACE_EXTRACTION_SQUEEZE
#define FACE_EXTRACTION_SQUEEZE

#include <cmath>
#include <iostream>
/* Opencv */
#include <opencv2/core/ocl.hpp>
#include <opencv2/face.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/tracking/tldDataset.hpp>
#include <opencv2/tracking/tracker.hpp>


#define PADDING 0 /* 15 Extra pixels to be sure to get the face */


class FaceExtracted
{
    private:
    cv::CascadeClassifier _faceCascade;
    std::string face_cascade_name;
    std::string window_name;
    std::vector<cv::Mat> facesROI;
    cv::Mat motherFrame;
    std::vector<cv::Rect> _faces;
    /* Landmarks */
    std::vector<std::vector<cv::Point2f>> _landmarks;
    /* Rotation matrix */
    std::vector<cv::Mat> _rotation_mat;
    /* ALigned faces */
    std::vector<cv::Mat> _aligned_faces;
    std::vector<cv::Mat> _aligned_cropped_faces;
    std::vector<cv::Mat> _detected_aligned_faces;
    /* Path */
    std::string _inputPath;
    std::string _outputPath;
    std::vector<std::string> _outputsVector;

    void getNumberOfAlignedFaces ();
    void getRotationMatrix ();
    void drawPolyline (const int, const int, const int, bool);
    void saveAlignedFace (cv::Mat, std::string, int);

    public:
    FaceExtracted (cv::Mat);
    FaceExtracted (std::string, std::string);
    FaceExtracted (cv::Mat, std::string);

    void detectFaces ();
    std::vector<cv::Rect> detectSingleFace (cv::Mat);
    void saveCroppedFaces (std::string path);
    int generateLandmark ();
    void generateFaceLine ();
    void generateThumbnails (int);
    void getRotatedFaces ();
    cv::Mat getMotherFrame ();
    void getFacesRectangle (std::vector<cv::Rect> &);
    void getOutputVector (std::vector<std::string> &);
    bool isTrackingOk ();
    void displayResult (int);
    void displayResult (cv::Mat);
};

#endif // FACE_EXTRACTION_SQUEEZE

/* Face tracing :

Frame : detect faces - if no face detected - skip frame
if face detected - save the rectangle - init tracker
while tracker ok, keep tracking
else return to step 1 */
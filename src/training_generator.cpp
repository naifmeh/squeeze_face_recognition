#include "training_generator.hpp"

using namespace std;
using namespace cv;

static int imageValue = 0;

TrainingGenerator::TrainingGenerator (string pathToFile, string output)
{
    _originalFrame = imread (pathToFile);
    _outputPath = output;
    if (_originalFrame.empty ())
    {
        printf ("No frame");
    }
}

TrainingGenerator::TrainingGenerator (string output, Mat frame)
{
    _outputPath = output;
    _originalFrame = frame;
}

TrainingGenerator::~TrainingGenerator ()
{
}


void TrainingGenerator::saveMatrix ()
{
    stringstream ssfn;
    ssfn << _outputPath << ++imageValue << ".jpg";
    string filePath = ssfn.str ();
    imwrite (filePath, _alteredFrame);
}

void TrainingGenerator::saveMatrix (std::string path)
{
    imwrite (path, _alteredFrame);
}

void TrainingGenerator::rotateImage (double angle, int mode)
{
    Point2f center (_originalFrame.cols / 2.0f, _originalFrame.rows / 2.f);
    Mat rot_mat;
    switch (mode)
    {
    case 1:
        while (angle < 360)
        {
            rot_mat = getRotationMatrix2D (center, angle, 1.0);
            warpAffine (_originalFrame, _alteredFrame, rot_mat, _originalFrame.size ());
            saveMatrix ();
            angle += 15;
        }
        break;

    case 0:
        rot_mat = getRotationMatrix2D (center, angle, 1.0);
        warpAffine (_originalFrame, _alteredFrame, rot_mat, _originalFrame.size ());
        saveMatrix ();
        break;

    default:
        break;
    }
}

void TrainingGenerator::displayResult ()
{
    while (1)
    {
        if (waitKey (10) == 27) break;
        imshow ("Data augmentation display", _alteredFrame);
    }
}

void TrainingGenerator::flipImageHorizontally ()
{
    flip (_originalFrame, _alteredFrame, 1);
    saveMatrix ();
}

void TrainingGenerator::generateGaussianNoise (int h)
{
    fastNlMeansDenoisingColored (_originalFrame, _alteredFrame, h);
    saveMatrix ();
}

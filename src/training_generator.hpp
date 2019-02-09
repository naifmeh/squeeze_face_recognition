#ifndef TRAINING_GENERATOR_SQUEEZE
#define TRAINING_GENERATOR_SQUEEZE

#include <iostream>
#include <opencv2/opencv.hpp>

/*
 * Class allowing to perform data augmentation in order to multiply
 * our original set of images.
 */
class TrainingGenerator
{
    private:
    cv::Mat _originalFrame;
    cv::Mat _alteredFrame;
    std::string _outputPath;
    std::vector<cv::Mat> listOfAlteredFrames;

    public:
    TrainingGenerator (std::string, std::string);
    TrainingGenerator (std::string, cv::Mat);
    ~TrainingGenerator ();

    void rotateImage (double, int);
    void flipImageHorizontally ();
    void generateGaussianNoise (int);
    void displayResult ();
    void saveMatrix ();
    void saveMatrix (std::string);
};

#endif // TRAINING_GENERATOR_SQUEEZE

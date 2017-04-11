#include <opencv2/core/core.hpp>
#include <opencv2/core/cuda.hpp>
#include <vector>

#include "constants.h"
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <chrono>

#define curIMAGE curImageUMat

class Processing : public QObject
{
    Q_OBJECT

public slots:
    void ProcessFrame();
    void CopyFrame(cv::Mat *Frame);
    void TriggerRecording(int size, char *data);

signals:
    void SendAngle(double Angle, double Speed);

public:
    Processing();
    ~Processing();

	cv::UMat curImageUMat;
	cv::Mat curImageMat;
	cv::Mat drawing;
    cv::Mat histogram;
private:
	double ProcessMethodOne();
	double ProcessMethodTwo();
//    double AnglePrediction(float angle);

	bool LastMeasurementFailed = 0;
	int CurrentFrame = 0;
	cv::Mat intrinsic_Matrix;
	cv::Mat distortion_coeffs;
	const int nPoints = 4;
    int ThreshConst = 80;
    const int ApproxConst = 10;
	int hist_mean;
    float last_angle = 0, last_pred_angle = 0, predictedAngle = 0, predictedSpeed = 0, motorSpeed = 0, motorPower = 0, motorPos = 0, usedAngle = 0, usedSpeed = 0, den = 0;
    int nmsmts=0;
    double prev_angles[NUMBER_PRED_ANGLES]={0};
    std::vector<int> iterj;
    std::vector<float> prang;
    QFile *file, *file2;
    std::chrono::high_resolution_clock::time_point processorstart, start, end;
    bool balancing=0;
};

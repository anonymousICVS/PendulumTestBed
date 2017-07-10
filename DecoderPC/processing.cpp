#include "processing.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/cuda.hpp>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <chrono>
#include <numeric>
#include <cmath>
#include <chrono>
#include <sstream>
#include <iomanip>
//#include "measuretime.h"
//#include "robotcontrol.h"

#define PI 3.14159265

//extern MeasureTime mytime;
//extern RobotControl BotControl;
extern bool CameraRunning;
extern std::condition_variable newFrameSignal;
extern std::condition_variable SendSignal;
std::mutex g_lock;

using namespace cv;



Processing::Processing()
{
    processorstart = std::chrono::high_resolution_clock::now();
    //std::fill(FrameBuffer, FrameBuffer + FRAME_COUNT, Mat(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC1));
    curImageMat = Mat(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC1);
    curImageUMat = UMat(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC1);
    drawing = Mat(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3);

    // 600x400 DATA
    intrinsic_Matrix = (Mat_<double>(3, 3) << 1538.80, 0, 311.49, 0, 1542.45, 188.30, 0, 0, 1);
    distortion_coeffs = (Mat_<double>(4, 1) << -0.224078, 0.744011, -0.001172, 0.002631);
    //    mytime.StartMeasure(12);

    file = new QFile("log_angles_coded.csv");
    if(!(file->open(QFile::WriteOnly))){
        std::cout << "Error opening the Log file!" << std::endl;
    }else{
        QTextStream stream(file);
        stream << "Time" << "," << "Measured Angle" << "," << "Measured Angle Speed" << "," << "Predicted Angle" << "," << "Predicted Angle Speed" << "," <<  "Motor Speed" << "," <<  "Motor Position" << "," << "Motor Power" << ", " << "Used Angle" << ", " << "Used Speed" << ", " << "Denominator" << "\n";
    }
}


Processing::~Processing()
{
    file->close();
    printf("Destroyed me \n");
}

void Processing::TriggerRecording(int size, char* data)
{
    int j = 0;
    j = (int) *data;
    //    std::cout << "Size: " << size << std::endl;
    if(size==1){
        balancing = j-48;
    }else{
        try {
        std::string fs(data);
        //Extracting the predicted angle and speed
        std::stringstream ss(fs);
        std::string item;
        std::getline(ss, item, ' ');
        predictedAngle = std::stof(item)/10000;
        std::getline(ss, item, ' ');
        predictedSpeed = std::stof(item)/10000;
        std::getline(ss, item, ' ');
        motorSpeed = std::stof(item)/10000;
        std::getline(ss, item, ' ');
        motorPos = std::stof(item)/10000;
        std::getline(ss, item, ' ');
        motorPower = std::stof(item)/10000;
        std::getline(ss, item, ' ');
        usedAngle = std::stof(item)/10000;
//        std::string item2 = "0.024";
//        float test = std::stof(item2);
//        std::cout << "'" << item2 << "'" << std::setprecision(2) << test << std::endl;
//        std::cout << "'" << item << "'" << std::setprecision(2) << usedAngle << std::endl;
        std::getline(ss, item, ' ');
        usedSpeed = std::stof(item)/10000;
        std::getline(ss, item, ' ');
        den = std::stof(item)/10000;
        } catch ( std::invalid_argument &e){
            std::cout << e.what() << std::endl;
        }

//        std::cout << "Predicted Angle from Robot: " << predictedAngle << ", predicted speed: " << predictedSpeed << std::endl;
    }
    //    if(balancing){
    //        std::cout << "Balancing started." << std::endl;
    //    }else{
    //        std::cout << "Balancing ended." << std::endl;
    //    }

}


void Processing::ProcessFrame()
{
    float angle = 0;
    float speed = 0;//, filteredspeed=0;
    //    long long proc_time;
    long long inter_time;
    //printf("Frame %4d: Start Processing.\n",FrameCounter);
    //    mytime.StartMeasure(1);
//    std::chrono::high_resolution_clock::time_point p1 = std::chrono::high_resolution_clock::now();
    angle = ProcessMethodOne();
//    std::chrono::high_resolution_clock::time_point p2 = std::chrono::high_resolution_clock::now();
//    float period;
//    period = std::chrono::duration_cast<std::chrono::microseconds>(p2-p1).count();
    //    proc_time = mytime.StopMeasure(1);
    //angle = round(angle*20)/20;
    if (LastMeasurementFailed)
    {
        angle = last_angle;
    }
    // Inter frame measure and speed calculations
    end = std::chrono::high_resolution_clock::now();
    inter_time = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
    //    std::cout << inter_time << std::endl;
    speed = (angle-last_angle)/(float(inter_time)/1000000);
    //    for(int i=0;i<NUMBER_PRED_ANGLES-1;i++){
    //        prev_angles[i] = prev_angles[i+1];
    //        filteredspeed += prev_angles[i];
    //    }
    //    prev_angles[NUMBER_PRED_ANGLES-1] = speed;
    //    filteredspeed += speed;
    //    speed = filteredspeed/NUMBER_PRED_ANGLES;


    //Angle speed filtering
    //    speed = (0.1*speed + 0.9*lastspeed)/2;

    //std::cout << (angle-last_angle) << std::endl;
    //std::cout << (float(inter_time)/1000000) << std::endl;
    //std::cout << (angle-last_angle)/(float(inter_time)/1000000) << std::endl;
    last_angle = angle;
    start = std::chrono::high_resolution_clock::now();
    // SEND ANGLE
    if(!LastMeasurementFailed){
        emit SendAngle(angle,speed);
        printf("Light: %3d Angle: %6.4f Angle speed %6.4f\r", hist_mean,angle, speed);
    }

    //angle = double(FrameCounter);
    if(balancing){
        QTextStream stream(file);
        stream << std::chrono::duration_cast<std::chrono::microseconds>(start-processorstart).count() << ", ";

        if(LastMeasurementFailed){
            stream << 90 << ", " << 1000;
        }else{
            stream << angle << ", " << speed;
        }

        stream << ", " << predictedAngle << ", " << predictedSpeed << ", " << motorSpeed << ", " << motorPos << ", " << motorPower << ", " << usedAngle << ", " << usedSpeed << ", " << den << "\n ";// ", " << period << "\n";
    }
//    stream << "Time" << "," << "Measured Angle" << "," << "Measured Angle Speed" << "," << "Predicted Angle" << "," << "Predicted Angle Speed" << "," <<  "Motor Speed" << "," <<  "Motor Position" << "," << "Motor Power" << ", " << "Used Angle" << ", " << "Used Speed" << ", " << "Denominator" << "\n";


    //printf("Current hist mean: %d\n", hist_mean);
    //printf("Current angle: %6.2f\n", angle);
    //printf("Current frame: %d\n", FrameCounter);

}

void Processing::CopyFrame(Mat *Frame)
{
    //long long time;
    //printf("Frame %4d: Before copy.\n",FrameCounter);
    //mytime.StartMeasure(0);
    Frame->copyTo(curIMAGE);
    //printf("Frame %4d: After copy.\n",FrameCounter);
    //time = mytime.StopMeasure(0);

}



double Processing::ProcessMethodOne()
{

    // Various Vars
    long long time;
    long long diff_time = 0;
    double angle = 0;
    //double thresh = 80;

    // For contour search and sort
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    std::vector<Point> approx;
    std::vector<Point> approx_child;

    // For undistortion
    Mat_<Point2d> points_distorted(nPoints, 1);
    Mat points_undistorted;


    // Current mean points
    cv::Point zero(0, 0);
    cv::Point2d mean_distorted(0, 0);
    cv::Point2d mean_undistorted(0, 0);

    // All mean points
    std::vector<Point2d> mean_points;

    // Histogram Stuff
    std::vector<double> histogram_vec;
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange = { range };

    //    mytime.StartMeasure(1);
    //    mytime.StartMeasure(0);


    curIMAGE.copyTo(drawing);
    //cvtColor(curIMAGE, drawing, CV_GRAY2RGB);



    // ----- Image Processing Steps -----
    //    mytime.StartMeasure(2);
    blur(curIMAGE, curIMAGE, Size(3, 3));
    //    time = mytime.StopMeasure(2);


    //    mytime.StartMeasure(3);
    //    printf("Data in drawing: %d\n", *(drawing.data+5));
    calcHist(&drawing, 1, 0, Mat(), histogram, 1, &histSize, &histRange);
    histogram = histogram.reshape(0, 1);
    histogram.copyTo(histogram_vec);
    for (unsigned int n = 0; n < histogram_vec.size(); n++)
    {
        hist_mean = hist_mean + n*histogram_vec.at(n);
    }
    hist_mean = hist_mean / (FRAME_WIDTH*FRAME_HEIGHT);
    //    time = mytime.StopMeasure(3);

    //ThreshConst = floor(hist_mean/2);
    //ThreshConst = hist_mean - 80;
    //equalizeHist(curIMAGE, curIMAGE);

    //    mytime.StartMeasure(4);
    threshold(curIMAGE, curIMAGE, ThreshConst, 255, THRESH_BINARY);
    //    time = mytime.StopMeasure(4);

    //curIMAGE.copyTo(drawing);

    //    mytime.StartMeasure(5);
    findContours(curIMAGE, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    //    time = mytime.StopMeasure(5);

    //    diff_time = diff_time + mytime.StopMeasure(0);
    //mytime.StartMeasure(0);
    // ----- Sorting the Contours -----
    //    mytime.StartMeasure(6);
    for (int i = 0; i < contours.size() & (diff_time < (EXPOSURE - 200)) ; i++)
    {

        // --- Check for markers ---
        //        mytime.StartMeasure(7);
        
        // Reject contours without child
        if (hierarchy[i][2] <= 0) continue;
        
        // Reject contours with an area of less than 100 pixels
        if (contourArea(Mat(contours[i])) < 100) continue;
        
        // Approx main contour with a square
        approxPolyDP(Mat(contours[i]), approx, ApproxConst, true);
        // and reject non-squares
        if (approx.size() != 4) continue;
        
        // Approx child contour with a triangle
        approxPolyDP(Mat(contours[hierarchy[i][2]]), approx_child, ApproxConst, true);
        // and reject non-triangles
        if (approx_child.size() != 3) continue;
        //        time = mytime.StopMeasure(7);

        // --- Undistort Points	of found marker ---
        //        mytime.StartMeasure(8);
        // Copz points to MAT
        for (int j = 0; j < approx.size(); j++)
        {
            points_distorted(j) = Point2f(approx.at(j).x, approx.at(j).y);
        }
        if (nPoints == 7)
        {
            for (int j = 0; j < approx_child.size(); j++)
            {
                points_distorted(j + 4) = Point2f(approx_child.at(j).x, approx_child.at(j).y);
            }
        }
        // Undistort
        undistortPoints(points_distorted, points_undistorted, intrinsic_Matrix, distortion_coeffs, intrinsic_Matrix);
        //        time = mytime.StopMeasure(8);

        // --- Calculate mean of distorted and undistorted points ---
        //        mytime.StartMeasure(9);
        if (nPoints == 7)
        {
            approx.insert(std::end(approx), std::begin(approx_child), std::end(approx_child));
        }
        mean_distorted = std::accumulate(approx.begin(), approx.end(), zero);
        mean_distorted /= nPoints;
        mean_undistorted.x = mean(points_undistorted)[0];
        mean_undistorted.y = mean(points_undistorted)[1];
        // Add mean point to list
        mean_points.push_back(mean_distorted);
        //diff_time = diff_time + mytime.StopMeasure(0);
        //mytime.StartMeasure(0);

    }
    //    time = mytime.StopMeasure(6);



    // --- Calculate angle from mean points ---
    //    mytime.StartMeasure(10);
    if (mean_points.size() == 2)
    {
        // Make sure, that P2 is 'above' P1 (P2.y < P1.y)
        if (mean_points.front().y < mean_points.back().y)
        {
            std::swap(mean_points.front(), mean_points.back());
        }
        // Calculate angle
        angle = std::atan( (mean_points.back().x - mean_points.front().x) / abs(mean_points.back().y - mean_points.front().y) );
        angle = angle * 180 / PI * (-1);
        LastMeasurementFailed = 0;
    }
    else
    {
        //        std::cout << mean_points.size() << std::endl;
        if (LastMeasurementFailed == 0)
        {
            printf("\nAngle measurement failed\n");
            LastMeasurementFailed = 1;
        }
    }
    //    time = mytime.StopMeasure(10);

    //std::cout << newestFrame << "   " << FrameReadCounter << std::endl;
    //std::cout << "Frame after: " << time << " microseconds" << std::endl;
    //    time = mytime.StopMeasure(1);
    return angle;
}


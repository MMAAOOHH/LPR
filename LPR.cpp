#include <iostream>
#include "core/core.hpp"
#include "highgui/highgui.hpp"
#include "opencv.hpp"

using namespace cv;

Mat RGBToGrey(Mat RGB)
{
    Mat grey = Mat::zeros(RGB.size(), CV_8UC1);
    for(int i = 0; i < RGB.rows; i++)
    {
        for (int j = 0; j < RGB.cols * 3; j += 3)
        {
            grey.at<uchar>(i, j / 3) = RGB.at<uchar>(i, j + 1);
        }
    }
    return grey;
} 

int main()
{
    Mat img;
    img = imread("C:\\Projects\\LPR\\res\\images\\car1.png");
    imshow("RGB image", img);
    Mat grey_img = RGBToGrey(img);
    imshow("Grey image", grey_img);

    waitKey();
}

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

Mat GreyToBinary(Mat grey, uint threshold)
{
    Mat bin = Mat::zeros(grey.size(), CV_8UC1);
    for (int i = 0; i < grey.rows; i++)
    {
        for (int j = 0; j < grey.cols; j++)
        {
            if ((grey.at<uchar>(i, j)) >= threshold)
                bin.at<uchar>(i, j) = 255;
        }
    }
    return bin;
}

Mat Inversion(Mat grey)
{
	Mat inverted = Mat::zeros(grey.size(), CV_8UC1);
    for (int i = 0; i < grey.rows; i++)
    {
        for (int j = 0; j < grey.cols; j++)
        {
            
            inverted.at<uchar>(i, j) = 255 - grey.at<uchar>(i, j);
        }
    }
    return inverted;
}

Mat Step(Mat grey, uint th1, uint th2)
{
    Mat stepped = Mat::zeros(grey.size(), CV_8UC1);
    for (int i = 0; i < grey.rows; i++)
    {
        for (int j = 0; j < grey.cols; j++)
        {
            if (grey.at<uchar>(i, j) >= th1 && grey.at<uchar>(i, j) <= th2)
                stepped.at<uchar>(i, j) = 255;
        }
    }
    return stepped;
}

Mat Avg(Mat grey, uint neighbor_size)
{
    Mat average = Mat::zeros(grey.size(), CV_8UC1);
    for (int i = 1; i < grey.rows - 1; i++)
    {
        for (int j = 1; j < grey.cols - 1; j++)
        {
            average.at<uchar>(i, j);

        }
    }
    return average;
}


Mat AVG(Mat grey, uint neighbour)
{
    

	Mat blur = Mat::zeros(grey.size(), CV_8UC1);

    //find neighbors
    for (int i = 0; i < grey.rows; i++)
    {
        for (int j = 0; j < grey.cols; j++)
        {

        }
    }
    return blur;
}

int main()
{
    Mat img;
    img = imread("C:\\Projects\\LPR\\res\\images\\car1.png");
    imshow("RGB image", img);

    Mat grey_img = RGBToGrey(img);
    imshow("Grey image", grey_img);

    Mat bin_img = GreyToBinary(grey_img, 128);
    imshow("Binary image", bin_img);

    Mat inverted_img = Inversion(grey_img);
    imshow("Inverted image", inverted_img);

    Mat stepped_img = Step(grey_img, 80, 140);
    imshow("Stepped image", stepped_img);


    waitKey();
    return 0;
}


/*  Math functions
 *  1 -> 1  
 *  Linear function = output is the same
 *  Inversion = y = 225 - x
 *
 *  AVG bluring = noisereduction
 *
 *  Masking
 *  n -> 1
 *  two identical odd numbers
 *
 *  1 , 2 , 3
 *  4 , 5 , 6
 *  7 , 8 , 9
 */
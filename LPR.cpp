#include <iostream>
#include "core/core.hpp"
#include "highgui/highgui.hpp"
#include "opencv.hpp"
#include <baseapi.h>
#include <allheaders.h>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

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

Mat Edge(Mat grey, uint th)
{
    Mat edge_image = Mat::zeros(grey.size(), CV_8UC1);
    for (int i = 1; i < grey.rows - 1; i++)
    {
        for (int j = 1; j < grey.cols -1 ; j++)
        {
            int AvgL = (grey.at<uchar>(i -1, j -1) + 
						grey.at<uchar>(i, j -1) + 
						grey.at<uchar>(i +1, j - 1)) / 3;

            int AvgR = (grey.at<uchar>(i - 1, j +1) +
						grey.at<uchar>(i, j +1) +
						grey.at<uchar>(i + 1, j +1)) / 3;

            if (abs(AvgL - AvgR) > th)
                edge_image.at<uchar>(i, j) = 255;
        }
    }
    return edge_image;
}


Mat AVG(Mat grey, int neighbors)
{
    Mat avg = Mat::zeros(grey.size(), CV_8UC1);
    auto tot = pow(2 * neighbors, 2);

    for (int i = neighbors; i < grey.rows - neighbors; i++)
    {
        for (int j = neighbors; j < grey.cols - neighbors; j++)
        {
            int sum = 0;
            for (int ii = -neighbors; ii < neighbors; ii++)
            {
                for (int jj = -neighbors; jj < neighbors; jj++)
                {
                	sum += (grey.at<uchar>(i + ii, j+ jj));
                }
            }
            avg.at<uchar>(i, j) = sum / tot;
        }
    }
    return avg;
}

Mat Max(Mat grey, int neighbors)
{
    Mat max = Mat::zeros(grey.size(), CV_8UC1);

    for (int i = neighbors; i < grey.rows - neighbors; i++)
    {
        for (int j = neighbors; j < grey.cols - neighbors; j++)
        {
            int def = -1;
            for (int ii = -neighbors; ii < neighbors; ii++)
            {
                for (int jj = -neighbors; jj < neighbors; jj++)
                {
                    if ((grey.at<uchar>(i + ii, j + jj)) > def)
                        def = grey.at<uchar>(i + ii, j + jj);
                }
            }
            max.at<uchar>(i, j) = def;
        }
    }
    return max;
}

Mat Min(Mat grey, int neighbors)
{
    Mat min = Mat::zeros(grey.size(), CV_8UC1);

    for (int i = neighbors; i < grey.rows - neighbors; i++)
    {
        for (int j = neighbors; j < grey.cols - neighbors; j++)
        {
            int def = 255;
            for (int ii = -neighbors; ii < neighbors; ii++)
            {
                for (int jj = -neighbors; jj < neighbors; jj++)
                {
                    if ((grey.at<uchar>(i + ii, j + jj)) < def)
                        def = grey.at<uchar>(i + ii, j + jj);
                }
            }
            min.at<uchar>(i, j) = def;
        }
    }
    return min;
}

Mat Erosion(Mat edge_image, int neighbors)
{
    Mat eroded_image = Mat::zeros(edge_image.size(), CV_8UC1);
    for (int i = neighbors; i < edge_image.rows - neighbors; i++) 
    {
        for (int j = neighbors; j < edge_image.cols - neighbors; j++) 
        {
            eroded_image.at<uchar>(i, j) = edge_image.at<uchar>(i, j); //Cant we just initialize the image as a copy?
        	for (int ii = -neighbors; ii <= neighbors; ii++) 
            {
                for (int jj = -neighbors; jj <= neighbors; jj++) 
                {
                    if (edge_image.at<uchar>(i + ii, j + jj) == 0) 
                    {
                        eroded_image.at<uchar>(i, j) = 0;
                    }
                }
            }
        }
    }
    return eroded_image;
}

Mat Dilation(Mat edge_image, int neighbors)
{
    Mat dilated_image = Mat::zeros(edge_image.size(), CV_8UC1);
    for (int i = neighbors; i < edge_image.rows - neighbors; i++)
    {
        for (int j = neighbors; j < edge_image.cols - neighbors; j++)
        {
            for (int ii = -neighbors; ii <= neighbors; ii++)
            {
                for (int jj = -neighbors; jj <= neighbors; jj++)
                {
                    if (edge_image.at<uchar>(i + ii, j + jj) == 255)
                    {
                        dilated_image.at<uchar>(i, j) = 255;
                        break;
                    }
                }
            }
        }
    }
    return dilated_image;
}



Mat EqHist(Mat Grey)
{
    Mat EQImg = Mat::zeros(Grey.size(), CV_8UC1);
    // count
    int count[256] = { 0 };
    for (int i = 0; i < Grey.rows; i++)
        for (int j = 0; j < Grey.cols; j++)
            count[Grey.at<uchar>(i, j)]++;


    // prob
    float prob[256] = { 0.0 };
    for (int i = 0; i < 256; i++)
        prob[i] = (float)count[i] / (float)(Grey.rows * Grey.cols);

    // accprob
    float accprob[256] = { 0.0 };
    accprob[0] = prob[0];
    for (int i = 1; i < 256; i++)
        accprob[i] = prob[i] + accprob[i - 1];
    // new = 255 * accprob 
    int newvalue[256] = { 0 };
    for (int i = 0; i < 256; i++)
        newvalue[i] = 255 * accprob[i];

    for (int i = 0; i < Grey.rows; i++)
        for (int j = 0; j < Grey.cols; j++)
            EQImg.at<uchar>(i, j) = newvalue[Grey.at<uchar>(i, j)];

    return EQImg;
}

int OTSU(Mat Grey)
{
    int count[256] = { 0 };
    for (int i = 0; i < Grey.rows; i++)
        for (int j = 0; j < Grey.cols; j++)
            count[Grey.at<uchar>(i, j)]++;


    // prob
    float prob[256] = { 0.0 };
    for (int i = 0; i < 256; i++)
        prob[i] = (float)count[i] / (float)(Grey.rows * Grey.cols);

    // accprob
    float theta[256] = { 0.0 };
    theta[0] = prob[0];
    for (int i = 1; i < 256; i++)
        theta[i] = prob[i] + theta[i - 1];

    float meu[256] = { 0.0 };
    for (int i = 1; i < 256; i++)
        meu[i] = i * prob[i] + meu[i - 1];

    float sigma[256] = { 0.0 };
    for (int i = 0; i < 256; i++)
        sigma[i] = pow(meu[255] * theta[i] - meu[i], 2) / (theta[i] * (1 - theta[i]));

    int index = 0;
    float maxVal = 0;
    for (int i = 0; i < 256; i++)
    {
        if (sigma[i] > maxVal)
        {
            maxVal = sigma[i];
            index = i;
        }
    }

    return index + 30;

}

std::vector<Mat>import_img_folder(const char* path)
{
    std::vector<Mat> images;
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        std::string s = entry.path().string();
        images.push_back(imread(s));
    }
    return images;
}


Mat process_image(Mat grey, int blur_size, float edge_threshold, int dilation_size)
{
    //Mat eqd = EqHist(grey);
    Mat blur = AVG(grey, blur_size);
    Mat edge = Edge(blur, edge_threshold);
    Mat dilated = Dilation(edge, dilation_size);

    return dilated;
}

Mat try_get_plate(Mat dilated_img, Mat grey, int min_width, float plate_ratio)
{
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;

    Mat plate;
    Rect rect;
    Scalar black = CV_RGB(0, 0, 0);

    // Clone as safeguard
    Mat dilated_img_copy = dilated_img.clone();

    findContours(dilated_img_copy, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0));

    /*Mat dst = Mat::zeros(dilated_img.size(), CV_8UC3);
    if (!contours.empty())
    {
        for (int i = 0; i < contours.size(); i++)
        {
            Scalar color((rand() & 255), (rand() & 255), (rand() & 255));
            drawContours(dst, contours, i, color, -1, 8, hierarchy);
        }
    }
	*/

    int contourCount = contours.size();

    //First search, wide plate
    contourCount = contours.size();

    for (int i = 0; i < contours.size(); i++)
    {
        rect = boundingRect(contours[i]);
        if (rect.width < min_width || rect.width < rect.height * (plate_ratio) || rect.y < 0.1f * dilated_img_copy.rows)
        {
            drawContours(dilated_img_copy, contours, i, black, -1, 8, hierarchy);
            contourCount--;
        }
        else
        {
            plate = grey(rect);
        }
    }

    if (plate.rows != 0 || plate.cols != 0)
    {
        return plate;
    }
}

void try_read_char(Mat img, tesseract::TessBaseAPI* ocr)
{
    ocr->SetImage(img.data, img.cols, img.rows, img.channels(), img.step);
    const char* letter_text = ocr->GetUTF8Text();
    std::string s(letter_text);

    if (s.empty())
    {
        std::cout << "NULL LETTER" << std::endl;
        //NullLetterCount++;
    }
    else
    {
		std::cout << s;
    }
}

Mat process_plate(Mat img, int id)
{
    cv::resize(img, img, Size(img.size() * 4), 0, 0, INTER_LINEAR);

    int OTSUTH = OTSU(img);

    Mat binary = GreyToBinary(img, OTSUTH + 35);
	Mat eroded = Erosion(binary, 1);

    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;

    findContours(eroded, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0));

    Mat dst = Mat::zeros(binary.size(), CV_8UC3);

    if (!contours.empty())
    {
        for (int i = 0; i < contours.size(); i++)
        {
            Scalar color((rand() & 255), (rand() & 255), (rand() & 255));
            drawContours(dst, contours, i, color, -1, 8, hierarchy);
        }
    }

    //cv::resize(dst, dst, Size(dst.size() * 8), 0, 0, INTER_LINEAR);
    imshow(std::to_string(id), dst);

    return eroded;
}

std::vector<Mat> segment_characters(Mat bin_plate, int letter_height_min, int letter_width_max, int letter_width_min)
{
    Mat img_copy = bin_plate.clone();

	std::vector<Mat> chars;
    Mat Char;

    Rect rect;
    Scalar black = CV_RGB(0, 0, 0);

    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;

    findContours(img_copy, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0));

    for (int i = 0; i < contours.size(); i++)
    {
        rect = boundingRect(contours[i]);

        if (rect.height < letter_height_min || rect.width > letter_width_max || rect.width < letter_width_min)
        {
            drawContours(img_copy, contours, i, black, -1, 8, hierarchy);
        }
        else
        {
            Char = img_copy(rect);

            if (Char.rows != 0)
            {
                //LetterCount++;

                // en regel
                //if (LetterCount < 6 || LetterCount > 7)
                //{
                    //Redo the thingie
                //}


                //cv::resize(Char, Char, Size(Char.size() * 8), 0, 0, INTER_LINEAR);
                //Char = AVG(Char, 4);
                //Char = GreyToBinary(Char, OTSUTH - 40 + i * 5);
                //imshow(std::to_string(200 + i), Char);

                //Char = Step(Char, 240, 256);

                //imshow(std::to_string(j), Char);

                //Sets a dark border around the letter, because some forum said that it will make it easier for tesseract to understand
                //Char = drawBorder(letter_image);

            	chars.push_back(Char);
            }
            else
            {
                std::cout << "No Character segments found" << std::endl;
            }
        }
    }
	return chars;
}


// Settings //
// Image processing
int blur_size = 2;
float edge_threshold = 50.0f;
int dilation_size = 4;

// Plate detection
int min_width = 50;
int max_width = 150;
int max_height = 130;
float plate_ratio = 1.5f;

// Character detect
int letter_height_min = 5;
int letter_width_min = 5;
int letter_width_max = 80;

int main()
{
    // Init Tesseract
    tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();
    ocr->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);
    ocr->SetPageSegMode(tesseract::PageSegMode::PSM_SINGLE_CHAR);
    ocr->SetVariable("tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

    // Import images
    std::vector<Mat> images = import_img_folder("C:\\Projects\\LPR\\res\\images");
    std::vector<Mat> plates;


	// Process images
    for (int i = 0; i < images.size(); i++)
    {
        // Process image
        Mat grey = RGBToGrey(images[i]);
        Mat dilated_img = process_image(grey, blur_size, edge_threshold, dilation_size);

        // Try get plate
        Mat plate = try_get_plate(dilated_img, grey, min_width, plate_ratio);
        if (plate.rows > 0)
        {
        	plates.push_back(plate);
        }
    }

    // PLATES
    for (int i = 0; i < plates.size(); ++i)
    {
	    
		Mat plate = plates[i];
        //imshow(std::to_string(i), plate);

        
    	Mat processed_plate = process_plate(plate, i);
    	//imshow(std::to_string(i), processed_plate);

    	// Segment characters from plate
    	std::vector<Mat> characters = segment_characters(processed_plate, letter_height_min, letter_width_max, letter_width_min);
    	std::cout << characters.size();

    	// Display characters found and run tesseract
    	for (int i = 0; i < characters.size(); i++)
    	{
             imshow(std::to_string(i), characters[i]);
             try_read_char(characters[i], ocr);
    	}
    }

	

//    Mat plate = plates[5];
//    //imshow(std::to_string(i), plate);
//
//
//    Mat processed_plate = process_plate(plate, 5);
//    imshow("Example plate", processed_plate);
//
//    // Segment characters from plate
//    std::vector<Mat> characters = segment_characters(processed_plate, letter_height_min, letter_width_max, letter_width_min);
//    std::cout << characters.size();
//
//    // Display characters found and run tesseract
//    for (int i = 0; i < characters.size(); i++)
//    {
//         imshow(std::to_string(i), characters[i]);
//         try_read_char(characters[i], ocr);
//    }
    

    waitKey();
    ocr->End();
    delete ocr;
    return 0;
}


// pipeline, rgb -> grey -> blur() -> edge (50) -> dila (4) -> ccl -> heu (w,h - ratio, xy) -> object -> tesseract (segment characters)
// segment characters -> from the grey, (binary th) -> segmentation
// th

// calculate histogram
// equalize

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

/* Heuristic rules
	Blob, starting pixel width and height.
	rectangular shape, ratio, density
	x, y
 */


// switch som går igenom olika settings
// for loop




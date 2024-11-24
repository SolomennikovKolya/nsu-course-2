
#include <iostream>
#include <opencv2/core.hpp>    // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui.hpp> // OpenCV window I/O
#include <opencv2/opencv.hpp>  // for putText
#include <time.h>

using namespace std;
using namespace cv;

// global variables
int mouseX = 0, mouseY = 0;

void callBackFunc(int event, int x, int y, int flags, void *userdata)
{
    if (event == EVENT_MOUSEMOVE)
    {
        mouseX = x;
        mouseY = y;
    }
}

void inversion(Mat img)
{
    int height = img.rows;
    int width = img.cols;

    for (int y = 0; y < height; y++)
    {
        uchar *p = img.ptr(y);
        for (int x = 0; x < width * 3; x++)
        {
            // R:
            *p = 255 - *p;
            p++;
            // G:
            *p = 255 - *p;
            p++;
            // B:
            *p = 255 - *p;
            p++;
        }
    }
}

void showInfo(Mat img, int curFPS)
{
    string s = to_string(curFPS);
    putText(img, "FPS: " + s, Point2d(5, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));

    uchar *p = img.ptr(mouseY) + (mouseX) * 3;
    s = to_string((int)*p);
    p++;
    s += " ";
    s += to_string((int)*p);
    p++;
    s += " ";
    s += to_string((int)*p);
    putText(img, "Pixel color: " + s, Point2d(5, 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));
}

int main(int argc, char *argv[])
{
    VideoCapture cap(0); // argument can be either the device index or the name of a video file
    Mat img;
    namedWindow("Image", 1);                 // create a window
    setMouseCallback("Image", callBackFunc); // binding the mouse to the window
    int curFPS = 0;

    while (true)
    {
        clock_t timeTable[4]; // table to detect time intervals

        timeTable[0] = clock();
        cap.read(img);
        timeTable[1] = clock();
        inversion(img);        // this is my frame conversion
        showInfo(img, curFPS); // show text information in Window
        timeTable[2] = clock();
        imshow("Image", img);
        timeTable[3] = clock();

        char c = waitKey(1); // 27 = esc
        if (c == 27)
            break;

        double dt = timeTable[3] - timeTable[0];
        curFPS = int(CLOCKS_PER_SEC / dt);
        cout << "FPS: " << curFPS << "    \t";
        cout << "read: " << int((timeTable[1] - timeTable[0]) / dt * 100) << "%\t";
        cout << "inversion: " << int((timeTable[2] - timeTable[1]) / dt * 100) << "%    \t";
        cout << "imshow: " << int((timeTable[3] - timeTable[2]) / dt * 100) << "%\n";
    }

    return 0;
}

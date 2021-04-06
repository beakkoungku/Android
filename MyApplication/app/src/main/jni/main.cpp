#include <jni.h>
#include "com_example_myapplication_MainActivity.h"

#include <opencv2/opencv.hpp>
#include <c++/v1/vector>
using namespace std;
//#include <vector>


using namespace cv;

extern "C" {
void setLabel(Mat &image, string str, vector<Point> contour) {
    int fontface = FONT_HERSHEY_SIMPLEX;
    double scale = 3;
    int thickness = 3;
    int baseline = 3;

    Size text = getTextSize(str, fontface, scale, thickness, &baseline);
    Rect r = boundingRect(contour);

    Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
    rectangle(image, pt + Point(0, baseline), pt + Point(text.width, -text.height),
              CV_RGB(200, 200, 200), FILLED);
    putText(image, str, pt, fontface, scale, CV_RGB(255, 255, 255), thickness, 1);
}
    void findContours(Mat &matInput,Mat &matResult){
    //    Mat cannyResult ;
    cvtColor(matInput, matResult, COLOR_RGBA2GRAY);
    threshold(matResult, matResult, 0, 100, THRESH_BINARY | THRESH_OTSU);
    Canny(matResult, matResult, 0, 10);
    vector<vector<Point>> contours;
    findContours(matResult, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Point2f> approx;
    for (size_t i = 0; i < contours.size(); i++) {
        approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.02, true);

        if (fabs(contourArea(Mat(approx))) < 1000 && fabs(contourArea(Mat(approx))) > 800)  //면적이 일정크기 이상이어야 한다.
        {

            int size = approx.size();

            //Contour를 근사화한 직선을 그린다.
            if (size % 2 == 0) {
                line(matInput, approx[0], approx[approx.size() - 1], Scalar(0, 255, 0), 10);

                for (int k = 0; k < size - 1; k++)
                    line(matInput, approx[k], approx[k + 1], Scalar(0, 255, 0), 10);

                for (int k = 0; k < size; k++)
                    circle(matInput, approx[k], 3, Scalar(0, 0, 255));
            } else {
                line(matInput, approx[0], approx[approx.size() - 1], Scalar(0, 255, 0), 10);

                for (int k = 0; k < size - 1; k++)
                    line(matInput, approx[k], approx[k + 1], Scalar(0, 255, 0), 10);

                for (int k = 0; k < size; k++)
                    circle(matInput, approx[k], 3, Scalar(0, 0, 255));
            }



            //도형을 판정한다.
            if (size == 3)
                setLabel(matInput, "triangle", contours[i]); //삼각형

                //이하는 해당 꼭지점을 가진 convex라면 찾는 도형
            else if (size == 4 && isContourConvex(Mat(approx)))
                setLabel(matInput, "rectangle", contours[i]); //사각형

            else if (size == 5 && isContourConvex(Mat(approx)))
                setLabel(matInput, "pentagon", contours[i]); //오각형

            else if (size == 6 && isContourConvex(Mat(approx)))
                setLabel(matInput, "hexagon", contours[i]);  //육각형

            else if (size == 10 && isContourConvex(Mat(approx)))
                setLabel(matInput, "decagon", contours[i]);    //십각형

                //위 조건에 해당 안되는 경우는 찾아낸 꼭지점 갯수를 표시
            else setLabel(matInput, to_string(approx.size()), contours[i]);
        }

    }

}

JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_ConvertRGBtoGray(
        JNIEnv *env,
        jobject instance,
        jlong matAddrInput,
        jlong matAddrResult) {


    Mat &matInput = *(Mat *) matAddrInput;
    Mat &matResult = *(Mat *) matAddrResult;
    findContours(matInput,matResult);
}

}
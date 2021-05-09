#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>
#include "armorDetector/armorDetector.hpp"
#include"processingtimer.hpp"

int GRAYTH = 170;
int COLORTH = 122;

ArmorDetector::Color detectColor = ArmorDetector::Color::red;
const std::string VIDEO_PATH = "../video/";

void mouse_callback(int event, int x, int y, int flags, void *userdata)
{
    if (event == cv::EVENT_RBUTTONDOWN)
    {
        std::cout << x << "," << y << std::endl;
    }
}
int main()
{
    cv::Mat img;
    int ROIFLAG = 0;
    int key = 0;
    cv::Rect2f armorRect,BefArmor;//認識した装甲の位置とサイズ情報
    cv::VideoCapture cap;
    std::string outputText;
    ArmorDetector armorDetector(ArmorDetector::Color::blue, GRAYTH, COLORTH);
    cap.open(VIDEO_PATH + "GOPRO260.MP4");
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('H', '2', '6', '4'));
    if (cap.isOpened() == false)
    {
        printf("cap open false\n");
        return -1;
    }
    double width, height;
    double max_frame, fps;
    max_frame = cap.get(CV_CAP_PROP_FRAME_COUNT);
    fps = cap.get(CV_CAP_PROP_FPS);
    width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    cv::VideoWriter writer(VIDEO_PATH + "output.avi", cv::VideoWriter::fourcc('H', '2', '6', '4'), fps, cv::Size(width, height), true);
    std::cout << max_frame << "," << fps << "," << width << "," << height << std::endl;
    uint count = 0;
    cap >> img;

    cv::namedWindow("input",CV_WINDOW_NORMAL);//表示ウインドウサイズ変更するだけ
    cv::resizeWindow("input",640,480);
    cv::namedWindow("img",CV_WINDOW_NORMAL);//表示ウインドウサイズ変更するだけ
    cv::resizeWindow("img",640,480);
    
    ProcessingTimer timer;

	timer.Start();

	int i;
    printf("FIRST i %d ",i);
    for (i = 0; i < max_frame; i++)
    {
        key = cv::waitKey(1);
        switch (key)
        {
        case 'q':
            //printf("Q Exit");
            cap.release();
            std::exit(1);
        case 'u':
            GRAYTH++;
            armorDetector.setGrayThreshold(GRAYTH);
            std::cout << GRAYTH << "," << COLORTH << std::endl;
            break;
        case 'd':
            GRAYTH--;
            armorDetector.setGrayThreshold(GRAYTH);
            std::cout << GRAYTH << "," << COLORTH << std::endl;
            break;
        case 'r':
            COLORTH++;
            armorDetector.setColorThreshold(COLORTH);
            std::cout << GRAYTH << "," << COLORTH << std::endl;
            break;
        case 'f':
            COLORTH--;
            armorDetector.setColorThreshold(COLORTH);
            std::cout << GRAYTH << "," << COLORTH << std::endl;
            break;
        /*
        case 'n':
            cap >> img;
            count++;
            //std::cout << count << std::endl;
            break;
        */
        default: 
            cap >> img;
            count++;
            //std::cout << count << std::endl;
            break;
        
        }
        //たまにエラー画像が入ってるときがあった
        if (img.empty())
            continue;

        /*

        */

        armorRect = armorDetector.detectArmor(img,BefArmor,ROIFLAG);
        if(armorRect.y == 0){
            ROIFLAG += 1;//認識不可フレーム これ装甲がy=0で認識されても認識不可認定されるから要再検討
        }else{
            ROIFLAG = 1;//装甲認識済み．次フレームでROI
            BefArmor = armorRect;
        }
        printf("COUNT %d ,Loop: %d X: %.2f ,Y: %.2f ,ROIF %d \n" ,count,i,armorRect.x,armorRect.y,ROIFLAG);
        writer << img;

        cv::imshow("input", img);
        cv::imshow("img", armorDetector.getResultImg());

    }
    timer.Stop();
    timer.PrintTime(ProcessingTimer::Unit::SECONDS);
	timer.PrintTime(ProcessingTimer::Unit::MILLI_SECONDS);
	printf("ALL FRAME %d ",i);
    return 1;
}

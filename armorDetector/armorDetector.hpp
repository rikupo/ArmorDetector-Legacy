#ifndef ARMORDETECTOR_HPP
#define ARMORDETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
class  LightBar
{
public:
    LightBar():matched(false){}
    LightBar(const cv::RotatedRect& R){
        rect.angle  = R.angle;
        rect.center = R.center;
        rect.size   = R.size;
        matched = false;
    }
    
    cv::RotatedRect rect;
    bool matched;
    size_t matched_index;
    float match_factor;
};
class Armor
{
public:
    Armor();
    Armor(const LightBar& L1,const LightBar& L2);
    cv::Rect2f drawArmor(cv::Mat img);
    double getArmorSize();
    double getErrorAngle();
    cv::Point2f getArmorCenter();
private:
    LightBar lightBar[2];
    float errorAngle;
    cv::Point2f armorCenter;
    cv::Rect2f armorRect;

    double armorSize;

};


class ArmorDetector
{
public:
    enum class Color : int
    {
        blue,
        red
    };
    ArmorDetector(Color enemyColor_, int GRAYTH_, int COLORTH_);
    ~ArmorDetector();
    cv::Rect2f detectArmor(cv::Mat inputImg,cv::Rect2f BefArmor, int ROIFLAG);
    void setGrayThreshold(int GRAYTH_);
    void setColorThreshold(int COLORTH_);
    cv::Mat getResultImg();
    cv::Mat ROI(cv::Mat inputImg,cv::Rect2f BefArmor,double roiMag,int ROIFLAG);

private:
    Color enemyColor;
    cv::Mat resultImg;
    // std::vector<cv::RotatedRect> rRect;
    int GRAYTH;
    int COLORTH;
};

#endif // ARMORDETECTOR_HPP

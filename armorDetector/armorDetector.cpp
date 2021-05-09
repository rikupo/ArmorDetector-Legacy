#include "armorDetector.hpp"

Armor::Armor(){}
Armor::Armor(const LightBar& L1,const LightBar& L2)
{
    lightBar[0] = L1;
    lightBar[1] = L2;
    errorAngle = std::fabs(L1.rect.angle - L2.rect.angle);
    armorRect.width = std::fabs(L1.rect.center.x - L2.rect.center.x);
    armorRect.height = std::fabs(L1.rect.center.x - L2.rect.center.x)*2/3;
    armorSize = armorRect.width*armorRect.height;
    // armorRect.height = std::fabs(L1.rect.center.y - L2.rect.center.y);
    armorCenter.x = (std::fabs(L1.rect.center.x + L2.rect.center.x))/2;
    armorCenter.y = (std::fabs(L1.rect.center.y + L2.rect.center.y))/2;
    armorRect.x = armorCenter.x - armorRect.width/2;
    armorRect.y = armorCenter.y - armorRect.height/2;
    // armorRect.width*= 2.0/3;
    // armorRect.height*= 2.0/3;
}
cv::Rect2f Armor::drawArmor(cv::Mat img)
{
    cv::rectangle(img,armorRect, cv::Scalar(0,255,0), 2); //ここで表示しているけどわかりにくい気がするからmainに移すのもあり 結局returnで返してるし
    return armorRect;
}
double Armor::getErrorAngle()
{
    return errorAngle;
}
double Armor::getArmorSize()
{
    return armorSize;
}
cv::Point2f Armor::getArmorCenter()
{
    return armorCenter;
}
ArmorDetector::ArmorDetector(Color enemyColor_, int GRAYTH_, int COLORTH_)
{
    enemyColor = enemyColor_;
    GRAYTH = GRAYTH_;
    COLORTH = COLORTH_;
}
ArmorDetector::~ArmorDetector()
{
}


cv::Mat ArmorDetector::ROI(cv::Mat inputImg,cv::Rect2f BefArmor,double roiMag,int ROIFLAG) //前回の装甲認識位置から今回の被認識領域(ピクセル上の位置か切り取られた画像そのもの)を返す
{
    cv::Mat trimedImg;
    cv::Rect2f roiArea;

    
    //完 inputImgをBefArmor中心にroiMag倍してトリミング
    //完 ROIしたらROI領域内で座標が求まるからどっかで全領域座標に変換しなきゃ．detectArmorからmainに返す際に変換．
    //未 ROIで画像領域外を指定しないようにする
    roiArea.x = BefArmor.x - BefArmor.width*(roiMag-1)/2;
    roiArea.y = BefArmor.y - BefArmor.height*(roiMag-1)/2;
    roiArea.width = BefArmor.width * roiMag;
    roiArea.height = BefArmor.height * roiMag;
    cv::Rect roi(roiArea);
    trimedImg = inputImg(roi);
    cv::rectangle(inputImg,roiArea, cv::Scalar(0,0,255), 2); //ROIエリアをRED枠で表示
    //printf("ROI: %f,%f,%f,%f\n",BefArmor.x,BefArmor.y,BefArmor.width * roiMag,BefArmor.height * roiMag);

    return trimedImg;
}

cv::Rect2f ArmorDetector::detectArmor(cv::Mat inputImg, cv::Rect2f BefArmor, int ROIFLAG)
{   
    cv::Rect2f armorRectAD;
    cv::Mat binaryImg, grayImg, colorImg;
    std::vector<cv::Mat> bgr;
    std::vector<LightBar> LightBarVector;
    double roiMag = 3; //ROI領域の装甲板に対する倍率

    // 可変ROI領域実装はROI-FLAGで管理したい -1だったらそのまま．1だったら通常ROI．２なら拡張ROI.以降Flag++
    if (ROIFLAG >= 1){
        roiMag = roiMag + (ROIFLAG-1)*0.3; //拡張ROI ならば前回ROI領域を拡大
        inputImg = ArmorDetector::ROI(inputImg,BefArmor,roiMag,ROIFLAG); //過去フレームで装甲を認識していた場合のみROIする．FLAGは何フレーム前に認識したかの値
    }
    // rRect.clear();

    //画像の値のある領域を膨張させるときに使う
    cv::Mat element77 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7));
    cv::Mat element55 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::Mat element33 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
    
    //バイナリー画像から輪郭抽出
    std::vector<std::vector<cv::Point>> contours;
    
    //BGR画像からグレースケール画像を生成
    cv::cvtColor(inputImg, grayImg, cv::COLOR_BGR2GRAY);
    
    //BGR画像からB,G,Rを成分ごとに分解して配列bgrにMat形式で格納
    cv::split(inputImg, bgr);

    
    if (enemyColor == Color::red)
    {
        //printf("BGR2 %d ,%d \n",bgr[2].rows,bgr[2].cols);
        //printf("BGR0 %d ,%d \n",bgr[0].rows,bgr[0].cols);
        subtract(bgr[2], bgr[0], colorImg);// 1 - 2 = 3
    }
    else
    { 
        //printf("BGR2 %d ,%d \n",bgr[2].rows,bgr[2].cols);
        //printf("BGR0 %d ,%d \n",bgr[0].rows,bgr[0].cols);
        subtract(bgr[0], bgr[2], colorImg);
    }
    
    cv::threshold(grayImg, grayImg, GRAYTH, 255, CV_THRESH_BINARY);
    cv::threshold(colorImg, colorImg, COLORTH, 255, CV_THRESH_BINARY);
    cv::dilate(colorImg, colorImg, element77);
    binaryImg = colorImg & grayImg;
    cv::dilate(binaryImg, resultImg, element55);
    cv::findContours(binaryImg, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
        //printf("CP 4 \n");
    for (size_t i = 0; i < contours.size(); i++)
    {
          //  printf("CP 5 \n");
        double area = cv::contourArea(contours[i]);
        //ここのパラメータは中国のプログラムから持ってきてるから根拠がわからない
        if (area < 15.0 || 1e5 < area)
        {
            //面積が大きすぎるor小さすぎる場合
            continue;
        }
        else
        {
            cv::RotatedRect rRect = cv::fitEllipse(contours[i]);
            cv::Point2f rectPoint[4];
            rRect.points(rectPoint);
            // rRect.push_back(tempRect);
            for (size_t j = 0; j < 4; j++)
            {
                //検出した領域を直線で囲む
                cv::line(inputImg, rectPoint[j], rectPoint[(j + 1) % 4], cv::Scalar(255, 0, 255), 2);
            }
            if(rRect.angle>90.0f){
                rRect.angle = rRect.angle - 180.0f;
            }
            // std::cout << "rRect.angle = " << rRect.angle << std::endl;
            //変な角度のやつを排除
            if(fabs(rRect.angle)<=10)
            {
                LightBar r(rRect);
                LightBarVector.push_back(r);
            }
        }
    }

    for(size_t i=0;i<LightBarVector.size();i++)
    {
        for(size_t j=i+1;j<LightBarVector.size();j++)
        {
            Armor armor(LightBarVector[i],LightBarVector[j]);
            // std::cout << armor.getErrorAngle() << ", " << armor.getArmorSize() << std::endl;
            if(armor.getErrorAngle()<4.0f && armor.getArmorSize()<5000){
                armorRectAD = armor.drawArmor(inputImg);
                if ( ROIFLAG = 0) return armorRectAD;//main関数にそのままの装甲情報を返す

                armorRectAD.x += BefArmor.x - BefArmor.width*(roiMag-1)/2;//ROIでトリミングされた位置情報を全体に復元
                armorRectAD.y += BefArmor.y - BefArmor.height*(roiMag-1)/2;
                return armorRectAD;
            }
        }
    }
}

void ArmorDetector::setGrayThreshold(int GRAYTH_)
{
    GRAYTH = GRAYTH_;
}
void ArmorDetector::setColorThreshold(int COLORTH_)
{
    COLORTH = COLORTH_;
}
cv::Mat ArmorDetector::getResultImg()
{
    return resultImg;
}

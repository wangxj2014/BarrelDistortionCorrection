#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/mat.hpp>
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>

int main()
{
    //for(double flag = 0.07; flag < 0.084; flag = flag+0.001) {
        double flag = 0;
        std::cout << flag << std::endl;
        double paramA = 0.083;//0.09;
        double paramB = 0.43;//0.5;
        double paramC = -0.23;
        double paramD = 1.0 - paramA - paramB - paramC;
        cv::Mat src_image = cv::imread("/APP3/workspace/python/fisheye/img/res.jpg", 1);  //BGR
        //cv::copyMakeBorder(src_image, src_image, 150, 150, 150, 150, cv::BORDER_CONSTANT, 0);
        //cv::imwrite("padding.jpg", src_image);
        cv::cvtColor(src_image, src_image, cv::COLOR_BGR2RGB);
        int xDim = src_image.cols; //width
        int yDim = src_image.rows; //height
        int zDim = src_image.channels();

        double xcen = (xDim - 1.0) / 2.0;
        double ycen = (yDim - 1.0) / 2.0;
        double normDist = xcen;
        if (normDist > ycen) {
            normDist = ycen;
        }
        double minv = 0.0, maxv = 0.0;
        double *imageMin = &minv;
        double *maxp = &maxv;
        cv::minMaxIdx(src_image, imageMin, maxp);
        //memset(dest_image, imageMin, sizeof(dest_image));
        //std::fill(&dest_image[0][0][0], &dest_image[0][0][0]+ sizeof(dest_image), *imageMin);
        //动态定义三维数组：
        int ***dest_image;
        int hight = yDim;
        int row = xDim;
        int col = zDim;
        dest_image = new int **[hight];
        for (int i = 0; i < hight; i++)
            dest_image[i] = new int *[row];
        for (int i = 0; i < hight; i++)
            for (int j = 0; j < row; j++)
                dest_image[i][j] = new int[col];
        //输入三维数组值：
        for (int i = 0; i < hight; i++)
            for (int j = 0; j < row; j++)
                for (int k = 0; k < col; k++)
                    dest_image[i][j][k] = *imageMin;

        //std::cout << dest_image[0][0][0] << std::endl;
        /**
        for(int i = 0;i<xDim;i++){
            for(int j = 0; j<yDim;j++) {
                dest_image[i][j] = new double[3]{*imageMin, *imageMin, *imageMin};
            }
        }**/
        cv::Mat b = cv::Mat::zeros(cv::Size(xDim, yDim), CV_8UC3);
        for (int k = 0; k < zDim; k++) {
            for (int j = 0; j < yDim; j++) {
                double yoff = (j - ycen) / normDist;
                for (int i = 0; i < xDim; i++) {
                    double xoff = (i - xcen) / normDist;
                    double rdest2 = xoff * xoff + yoff * yoff;
                    double rdest = sqrt(rdest2);
                    double rdest3 = rdest2 * rdest;
                    double rdest4 = rdest2 * rdest2;
                    double rsrc = paramA * rdest4 + paramB * rdest3 + paramC * rdest2 + paramD * rdest;
                    rsrc = normDist * rsrc;
                    double ang = atan2(yoff, xoff);
                    double xSrc = xcen + (rsrc * cos(ang));
                    double ySrc = ycen + (rsrc * sin(ang));
                    if (xSrc >= 0 && xSrc < xDim - 1 && ySrc >= 0 && ySrc < yDim - 1) {
                        int xBase = (int) floor(xSrc);
                        float delX = (float) (xSrc - xBase);
                        int yBase = (int) floor(ySrc);
                        float delY = (float) (ySrc - yBase);
                        int b = src_image.at<cv::Vec3b>(yBase, xBase)[k];
                        dest_image[j][i][k] = int((1 - delX) * (1 - delY) * b);
                        if (xSrc < (xDim - 1)) {
                            int tmp = src_image.at<cv::Vec3b>(yBase, xBase + 1)[k];
                            dest_image[j][i][k] += int(delX * (1 - delY) * tmp);
                        }
                        if (ySrc < (yDim - 1)) {
                            int tmp = src_image.at<cv::Vec3b>(yBase + 1, xBase)[k];
                            dest_image[j][i][k] += int((1 - delX) * delY * tmp);
                        }
                        if ((xSrc < (xDim - 1)) && (ySrc < (yDim - 1))) {
                            int tmp = src_image.at<cv::Vec3b>(yBase + 1, xBase + 1)[k];
                            dest_image[j][i][k] += int(delX * delY * tmp);
                        }
                        /**
                        if(i==100 && j==100){
                            std::cout << yBase << std::endl;
                            std::cout << xBase << std::endl;
                            std::cout << k << std::endl;
                            std::cout << dest_image[yBase][xBase][k] << std::endl;
                        }**/
                    }
                }
            }
        }
        std::cout << dest_image[900][1200][0] << std::endl;

        cv::Mat res = cv::Mat::zeros(yDim-300, xDim-300, CV_32FC3);

        res.at<cv::Vec3f>(0, 0)[0] = dest_image[0][0][0];
        for (int j = 0; j < yDim-300; ++j) {
            for (int i = 0; i < xDim-300; i++) {
                res.at<cv::Vec3f>(j, i)[2] = dest_image[j+150][i+150][0];
                res.at<cv::Vec3f>(j, i)[1] = dest_image[j+150][i+150][1];
                res.at<cv::Vec3f>(j, i)[0] = dest_image[j+150][i+150][2];
            }
        }
        std::string resname = "./res";
        std::string n = std::to_string(flag);
        resname += n;
        resname += ".jpg";
        std::cout << resname << std::endl;
        cv::imwrite(resname, res);
   // }
   return  0;
}

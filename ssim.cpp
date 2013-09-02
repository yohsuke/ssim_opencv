//
// this source is based on 'gpu-basics-similarity.cpp' in OpenCV 2.4.2
//

#include <iostream>                   // Console I/O

#include <opencv2/core/core.hpp>      // Basic OpenCV structures
#include <opencv2/imgproc/imgproc.hpp>// Image processing methods for the CPU
#include <opencv2/highgui/highgui.hpp>// Read images

using namespace std;
using namespace cv;

Scalar getMSSIM( const Mat& I1, const Mat& I2);

int main(int, char *argv[])
{
    Mat I1 = imread(argv[1]);           // Read the two images
    Mat I2 = imread(argv[2]);

    if (!I1.data || !I2.data)           // Check for success
    {
        cout << "Couldn't read the image";
        return 0;
    }

    Scalar x = getMSSIM(I1,I2);

    cout << "SSIM\t" << "R:" << x.val[2] << "\tG:" << x.val[1] << "\tB:" << x.val[0] << "\tAVG:" << (x.val[0] + x.val[1] + x.val[2])/3 << endl;

    return 0;
}

Scalar getMSSIM( const Mat& i1, const Mat& i2)
{
    const double C1 = 6.5025, C2 = 58.5225;
    /***************************** INITS **********************************/
    int d     = CV_32F;

    Mat I1, I2;
    i1.convertTo(I1, d);           // cannot calculate on one byte large values
    i2.convertTo(I2, d);

    Mat I2_2   = I2.mul(I2);        // I2^2
    Mat I1_2   = I1.mul(I1);        // I1^2
    Mat I1_I2  = I1.mul(I2);        // I1 * I2

    /*************************** END INITS **********************************/

    Mat mu1, mu2;   // PRELIMINARY COMPUTING
    GaussianBlur(I1, mu1, Size(11, 11), 1.5);
    GaussianBlur(I2, mu2, Size(11, 11), 1.5);

    Mat mu1_2   =   mu1.mul(mu1);
    Mat mu2_2   =   mu2.mul(mu2);
    Mat mu1_mu2 =   mu1.mul(mu2);

    Mat sigma1_2, sigma2_2, sigma12;

    GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
    sigma1_2 -= mu1_2;

    GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
    sigma2_2 -= mu2_2;

    GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
    sigma12 -= mu1_mu2;

    ///////////////////////////////// FORMULA ////////////////////////////////
    Mat t1, t2, t3;

    t1 = 2 * mu1_mu2 + C1;
    t2 = 2 * sigma12 + C2;
    t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

    t1 = mu1_2 + mu2_2 + C1;
    t2 = sigma1_2 + sigma2_2 + C2;
    t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

    Mat ssim_map;
    divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

    Scalar mssim = mean( ssim_map ); // mssim = average of ssim map
    return mssim;
}

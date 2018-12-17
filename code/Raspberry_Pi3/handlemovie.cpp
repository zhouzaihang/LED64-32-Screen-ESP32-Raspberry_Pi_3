#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define movie_length 6574

int main() {
    char image_name[17];
    memset(image_name,0,17);
    int i = 1001;
    unsigned char screenR[64][8] = {0};
    unsigned char screenG[64][8] = {0};
    while (i) {
        sprintf(image_name, "./movie/%.04d.png", i);
        Mat image = imread(image_name, 0);
        int w = image.cols;
        int h = image.rows;
//        std::cout << "w: " << w << " h: " << h;
        for (int j = 0; j < h; ++j) {
            for (int k = 0; k < w; ++k) {
                screenR[k][j/8] = (unsigned char) (image.at<uchar>(k, j) <= 50 ? (screenR[k][j / 8] >> 1) | 0x80 :
                                                   (screenR[k][j / 8] >> 1) & 0x7F);
//                if (image.at<uchar>(k, j) < 250){
//                    screenG[k][j] = 1;
//                }
//                printf("%x ",screenR[k][j/8]);
            }
//            std::cout << "\n";
        }
        cout << image_name <<endl;
        for (auto &j : screenR) {
            for (unsigned char k : j) {
                printf("%x ", k);
            }
            cout << endl;
        }
        if (i == movie_length){
            i = 0;
        }
        i++;
    }
}
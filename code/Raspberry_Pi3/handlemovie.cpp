#include <opencv2/opencv.hpp>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#define movie_length 6574
#define KEY 75
#define SIZE 4096

struct msg_form{
    long msg_type;
    unsigned char msg_text[SIZE];
}msg;

using namespace std;
using namespace cv;

void send_msg(int type){
    int msg_qid = msgget(KEY, 0777);
    msg.msg_type = type;
    for(int i = 0; i < 15; i++){
        msgsnd(msg_qid,&msg,SIZE,0);
    }
    // for (int i = 0; i < 64; i++) {
    //     for (int j = 0; j < 8; j++) {
    //         printf("%x ", msg.msg_text[i * 8 + j]);
    //     }
    //     cout << endl;
    // }
}

int main() {
    char image_name[17];
    memset(image_name,0,17);
    int i = 1;
//    unsigned char screenR[64][8] = {0};
//    unsigned char screenG[64][8] = {0};
    while (i) {
        sprintf(image_name, "./movie/%.04d.png", i);
        Mat image = imread(image_name, 0);
        int w = image.cols;
        int h = image.rows;
//        std::cout << "w: " << w << " h: " << h;
        for (int j = 0; j < h; ++j) {
            for (int k = 0; k < w; ++k) {
                // 白色亮，黑色暗
                msg.msg_text[k*8+j/8] = (unsigned char) (image.at<uchar>(k, j) >= 125 ? (msg.msg_text[k*8+j / 8] << 1) | 0x01 :
                                                   (msg.msg_text[k*8+j / 8] << 1) & 0xFE);
            }
//            std::cout << "\n";
        }
        // cout << image_name <<endl;
        send_msg(i);
        if (i == movie_length){
            i = 0;
        }
        i++;
    }
}
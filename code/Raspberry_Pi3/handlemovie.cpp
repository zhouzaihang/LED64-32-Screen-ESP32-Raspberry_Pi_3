#include <opencv2/opencv.hpp>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <ctime>
#include <cstdio>
#define movie_length 6574
#define KEY 75
#define SIZE 4096
#define max 876.53
#define fps 30

using namespace std;
using namespace cv;

struct msg_form{
    long msg_type;
    unsigned char msg_text[SIZE];
}msg;

void send_msg(int type){
    int msg_qid = msgget(KEY, 0777);
    msg.msg_type = type;
    msgsnd(msg_qid,&msg,SIZE,0);
//    for (int i = 0; i < 64; i++) {
//         for (int j = 0; j < 8; j++) {
//             printf("%x ", msg.msg_text[i * 8 + j]);
//         }
//         cout << endl;
//    }
}

int main() {
    char image_name[17];
    memset(image_name,0,17);
    int i = 1;
    int v = (int)((max + 0.5) / fps);
    time_t stop = time(nullptr);    
    time_t start = time(nullptr);
    while (i) {
        for (int l = 0; l < v; l++){
            sprintf(image_name, "./movie/%.04d.png", i);
            Mat image = imread(image_name, 0);
            int w = image.cols;
            int h = image.rows;
    //        std::cout << "w: " << w << " h: " << h;
            for (int j = 0; j < h; ++j) {
                auto * data = image.ptr<uchar>(j);
                for (int k = 0; k < w; ++k) {
                    msg.msg_text[j*8+k/8] = (unsigned char)(data[k] >= 124? ((msg.msg_text[j*8+k/8]<<1)|0x01) : ((msg.msg_text[j*8+k/8] <<1 )& 0xFE));
                }
            }
            // cout << image_name <<endl;
            send_msg(i);
        }
        if (i == movie_length){
            i = 0;
            stop = time(nullptr);
            cout << stop-start <<endl;
        }
        i++;
    }
}
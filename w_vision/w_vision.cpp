
#include "platform.h"
#ifdef WIN_PLATFORM

#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>


#include "count_frame.h"
#include "NativeWindow.h"
#include "count_net.h"
#include "count_optFlow.h"
#include "count_base.h"


int main()
{
	count_base* pBase = nullptr;
	const char* cmodel = "1.caffemodel";
	const char* cprototxt = "2.prototxt";
	Mat frame;
	string filename = "4.avi";
	int method = 2;

	switch (method) {
	case 0:
		pBase = new count_frame(22, 76, 1, 15, 0.78, 0.5, 0.65, 0.2, 0.1, 30);
		break;
	case 1:
		pBase = new count_net (22, 76, 1, 10, 0.78, 0.5, 0.65, 0.2, 0.1, 30);
		break;

	default:
		pBase = new count_optFlow(1, 30, 0.78, 0.5, 0.65, 0.2, 0.1, 30, 0.4, 0.6);
		break;
	}
	pBase->setWindow(new NativeWindow());
	pBase->initCaffe(cprototxt, cmodel);

	VideoCapture capture;
	capture.open(filename);

	double rate = capture.get(CV_CAP_PROP_FPS);
	int delay = cvRound(1000.000 / rate);

	if (!capture.isOpened()){
		return -1;
	}
	
	while (true){	
		capture >> frame;
		if (frame.empty()) continue;
		int H = frame.rows;
		int W = frame.cols;

		Mat frame2;
		//resize(frame, frame2, Size((int)W*0.6, (int)H*0.6));
		resize(frame, frame2, Size(450, 300));

		pBase->processOneFrame(frame2);

		waitKey(1);
	}
	return 0;

}
#endif

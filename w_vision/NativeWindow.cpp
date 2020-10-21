//
// Created by wangry on 2019/10/29.

#include "platform.h"
#include "NativeWindow.h"
#ifdef WIN_PLATFORM

NativeWindow::~NativeWindow() {

}
NativeWindow::NativeWindow() {

}

void NativeWindow::displayMat(cv::Mat& display_RGB_Mat, cv::Mat& img) {
	if (!display_RGB_Mat.empty()) {
		imshow("frame", display_RGB_Mat);
	}
	if(&img != nullptr){
		imshow("mask", img);
	}
}

#else

#include "common.h"

#define LOG_TAG "NativeWindow"

NativeWindow::NativeWindow(ANativeWindow* native_window)
: displayWindow(),
  m_native_window(native_window)
{

}

NativeWindow::~NativeWindow(){

}

void NativeWindow::SetNativeWindow(ANativeWindow* native_window) {
  // Save native window
  m_native_window = native_window;
}

void NativeWindow::displayMat(cv::Mat& display_RGB_Mat, cv::Mat& img){

	// copy to TextureView surface
	if(m_native_window == nullptr){
		return;
	}
	ANativeWindow_acquire(m_native_window);
	ANativeWindow_Buffer buf;

	int dstWidth = display_RGB_Mat.cols;
	int dstHeight = display_RGB_Mat.rows;


	ANativeWindow_setBuffersGeometry(m_native_window, dstWidth, dstHeight, WINDOW_FORMAT_RGBA_8888 /*format unchanged*/);

	if (int32_t err = ANativeWindow_lock(m_native_window, &buf, NULL)) {
		LOGD("ANativeWindow_lock failed with error code %d\n", err);
		ANativeWindow_release(m_native_window);
	}
	uint8_t *dstLumaPtr = reinterpret_cast<uint8_t *>(buf.bits);
	cv::Mat dstRgba(dstHeight, buf.stride, CV_8UC4,dstLumaPtr); // TextureView buffer, use stride as width			  
	cv::Mat srcRgba(dstHeight, dstWidth, CV_8UC4);
	//rgb->rgba
	cv::cvtColor(display_RGB_Mat, srcRgba, cv::COLOR_RGB2RGBA);

	unsigned char *dbuf;
	unsigned char *sbuf;
	dbuf = dstRgba.data;
	sbuf = srcRgba.data;
	int i;
	for (i = 0; i < srcRgba.rows; i++) {
		dbuf = dstRgba.data + i * buf.stride * 4;
		memcpy(dbuf, sbuf, srcRgba.cols * 4);
		sbuf += srcRgba.cols * 4;
	}

	ANativeWindow_unlockAndPost(m_native_window);
	ANativeWindow_release(m_native_window);
}

#endif


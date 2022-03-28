#include <opencv2/opencv.hpp>

#define CVUI_IMPLEMENTATION
#include "../cvui.h"
#include <opencv2/opencv.hpp>

#define MENU_WINDOW "GVMP | Garren's Video Manipulation Program"
#define VIDEO_STREAM_WINDOW "Video Stream"
#define VIDEO_STREAM_EDGES "Edges (Canny)"
#define VIDEO_STREAM_GRADIENT "Gradient (Sobel)"

int main(int argc, const char *argv[])
{
	cv::Mat menu = cv::Mat(500, 500, CV_8UC3);
	int count = 0;

	cvui::init(MENU_WINDOW);

	int camera = 0;
    cv::VideoCapture cap;

    if(!cap.open(camera))
        return 0;


	int kernelSize = 3;

	bool applyGrayscale = false;
	bool shouldScaleDown = false;
	bool shouldMirrorV = false;
	bool shouldMirrorH = false;
	bool getNegative = false;
	bool applyGaussianBlur = false;
	bool shouldDetectEdges = false;
	bool getGradientEstimative = false;

	int rotations = 0;

	double contrastAlpha = 1.0;
	int brightnessBeta = 0.0;

	while (true) {
		cv::Mat frame;
		cap >> frame;
		if(frame.empty()) break; // end of video stream

		// Fill the frame with a nice color
		menu = cv::Scalar(49, 52, 49);

		cvui::trackbar(menu, 40, 30, 220, &kernelSize, (int)3, (int)33);
		cvui::text(menu, 250, 50, "Gaussian kernel size");

		cvui::trackbar(menu, 40, 75, 220, &contrastAlpha, (double)0.5, (double)5.0);
		cvui::text(menu, 250, 95, "Contrast");
		
		cvui::trackbar(menu, 40, 120, 220, &brightnessBeta, (int)0.0, (int)255.0);
		cvui::text(menu, 250, 140, "Brightness");

		if (cvui::button(menu, 40, 180, "Hello, world!")) count++;
		if (cvui::button(menu, 40, 210, "Rotate 90")) rotations = (rotations + 1) & 3;
		if (cvui::button(menu, 40, 240, "Mirror H")) shouldMirrorH = !shouldMirrorH;
		if (cvui::button(menu, 40, 270, "Mirror V")) shouldMirrorV = !shouldMirrorV;
		if (cvui::button(menu, 40, 300, "Detect Edges")) shouldDetectEdges = !shouldDetectEdges;
		if (cvui::button(menu, 40, 330, "Get Gradient")) getGradientEstimative = !getGradientEstimative;

		cvui::checkbox(menu, 40, 375, "apply Gaussian Blur", &applyGaussianBlur);
		cvui::checkbox(menu, 40, 390, "get negative", &getNegative);
		cvui::checkbox(menu, 40, 405, "apply grayscale", &applyGrayscale);
		cvui::checkbox(menu, 40, 420, "scale down", &shouldScaleDown);

		frame.convertTo(frame, -1, contrastAlpha, brightnessBeta);
		if(applyGaussianBlur) cv::GaussianBlur(frame, frame, cv::Size(kernelSize | 1, kernelSize | 1), 1.0, 1.0);
		if (getNegative) frame.convertTo(frame, -1, -1.0, 255);
		if (applyGrayscale) cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
		if (shouldScaleDown) cv::resize(frame, frame, cv::Size(frame.size().width /2, frame.size().height /2));
		for (size_t i = 0; i < rotations; i++) cv::rotate(frame, frame, cv::ROTATE_90_COUNTERCLOCKWISE);
		if (shouldMirrorH) cv::flip(frame, frame, 1);
		if (shouldMirrorV) cv::flip(frame, frame, 0);

		cvui::printf(menu, 250, 200, 0.4, 0xff0000, "Button click count: %d", count);
		
		cvui::imshow(MENU_WINDOW, menu);		
		cv::imshow(VIDEO_STREAM_WINDOW, frame);
		cvui::update();

		// Check if ESC key was pressed
		if (cv::waitKey(20) == 27) {
			break;
		}
	}

	return 0;
}
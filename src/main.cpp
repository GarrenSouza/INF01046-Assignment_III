#define CVUI_IMPLEMENTATION
#include "../cvui.h"
#include <opencv2/opencv.hpp>
#include <cmath>
#include <string>

#define MENU_WINDOW "GVMP | Garren's Video Manipulation Program"
#define VIDEO_STREAM_WINDOW "Video Stream"
#define VIDEO_STREAM_EDGES "Edges (Canny)"
#define VIDEO_STREAM_GRADIENT "Gradient (Sobel)"

namespace Local{

template<typename valueType>
valueType clamp(valueType v, valueType lo, valueType hi){
	return (v < lo)? lo : (v > hi)? hi : v;
}

}
int main(int argc, const char *argv[])
{
	if(argc > 1){
		cv::Mat menu = cv::Mat(500, 500, CV_8UC3);

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
		bool detectEdges = false;
		bool getGradientEstimative = false;

		int rotations = 0;

		double contrastAlpha = 1.0;
		int brightnessBeta = 0.0;
		int ddepth = CV_16S;

		cv::Mat frame;
		cap >> frame;

		cv::VideoWriter video(cv::String(std::string(argv[1]) + ".avi"),cv::VideoWriter::fourcc('M','J','P','G'),30, cv::Size(frame.size().width,frame.size().height));

		while (true) {

			cap >> frame;
			if(frame.empty()) break;			

			menu = cv::Scalar(49, 52, 49);

			cvui::trackbar(menu, 40, 30, 220, &kernelSize, (int)3, (int)33);
			cvui::text(menu, 250, 50, "Kernel size");

			cvui::trackbar(menu, 40, 75, 220, &contrastAlpha, (double)0.5, (double)5.0);
			cvui::text(menu, 250, 95, "Contrast");
			
			cvui::trackbar(menu, 40, 120, 220, &brightnessBeta, (int)0.0, (int)255.0);
			cvui::text(menu, 250, 140, "Brightness");

			if (cvui::button(menu, 40, 225, "Rotate 90")) rotations = (rotations + 1) & 3;
			if (cvui::button(menu, 40, 255, "Mirror H")) shouldMirrorH = !shouldMirrorH;
			if (cvui::button(menu, 40, 285, "Mirror V")) shouldMirrorV = !shouldMirrorV;

			cvui::checkbox(menu, 40, 345, "show edges", &detectEdges);
			cvui::checkbox(menu, 40, 360, "show gradient", &getGradientEstimative);
			cvui::checkbox(menu, 40, 375, "apply Gaussian Blur", &applyGaussianBlur);
			cvui::checkbox(menu, 40, 390, "get negative", &getNegative);
			cvui::checkbox(menu, 40, 405, "apply grayscale", &applyGrayscale);
			cvui::checkbox(menu, 40, 420, "scale down", &shouldScaleDown);

			frame.convertTo(frame, -1, contrastAlpha, brightnessBeta);

			if (applyGaussianBlur) cv::GaussianBlur(frame, frame, cv::Size(kernelSize | 1, kernelSize | 1), 1.0, 1.0);

			if (detectEdges){
				cv::Mat edges;
				cv::Canny(frame, edges, 30, 100, Local::clamp(2*(kernelSize - 1) - 1, 3, 7));
				frame = edges;
			}
			
			if (getGradientEstimative && !detectEdges){
				cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
				cv::Mat grad_x, grad_y;
				cv::Mat abs_grad_x, abs_grad_y;
				cv::Sobel(frame, grad_x, ddepth, 1, 0, kernelSize, 1.0f, 0.0f, cv::BORDER_DEFAULT);
				cv::Sobel(frame, grad_y, ddepth, 0, 1, kernelSize, 1.0f, 0.0f, cv::BORDER_DEFAULT);
				convertScaleAbs(grad_x, abs_grad_x);
				convertScaleAbs(grad_y, abs_grad_y);
				uchar* row, *x_grad, *y_grad;
				for (int i = 0; i < grad_x.size().height; i++)
				{
					row = frame.ptr(i);
					x_grad = abs_grad_x.ptr(i);
					y_grad = abs_grad_y.ptr(i);
					for (int j = 0; j < grad_x.size().width; j++)
					{
						row[j] = Local::clamp(sqrt(x_grad[j]*x_grad[j] + y_grad[j]*y_grad[j]), 0.0, 255.0);
					}
					
				}
			}

			if (getNegative) frame.convertTo(frame, -1, -1.0, 255);
			if (applyGrayscale && !detectEdges && !getGradientEstimative) cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
			if (shouldScaleDown) cv::resize(frame, frame, cv::Size(frame.size().width /2, frame.size().height /2));
			for (size_t i = 0; i < rotations; i++) cv::rotate(frame, frame, cv::ROTATE_90_COUNTERCLOCKWISE);
			if (shouldMirrorH) cv::flip(frame, frame, 1);
			if (shouldMirrorV) cv::flip(frame, frame, 0);
			
			cvui::imshow(MENU_WINDOW, menu);		
			cv::imshow(VIDEO_STREAM_WINDOW, frame);
			cvui::update();

			if(rotations == 0 && !shouldScaleDown) video.write(frame);

			// Check if ESC key was pressed
			if (cv::waitKey(20) == 27) {
				break;
			}
		}
	}else{
		std::cout << "usage is: image_app <output_video_file_path>" << std::endl;
	}

	return 0;
}
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <iostream>

cv::Mat image;
cv::Mat gray;
cv::Mat blurred; 
cv::Mat edges;
std::vector<std::vector<cv::Point>> contours;

int low_threshold = 50;
int high_threshold = 90;

void onTrackbar(int, void*) {
    cv::Canny(blurred, edges, low_threshold, high_threshold, 3);
    cv::findContours(edges, contours, cv::RETR_TREE, cv::CHAIN_APPROX_TC89_L1);
    cv::Mat contours_draw = cv::Mat::zeros(edges.size(), CV_8UC3);

    for (size_t i = 0; i < contours.size(); i++) {
        std::vector<cv::Point> approx;
        cv::approxPolyDP(contours[i], approx, cv::arcLength(contours[i], true) * 0.02, true);
        size_t vertices = approx.size();

        std::string shape_label;
        if (vertices == 3)
            shape_label = "Triangle";
        else if (vertices == 4)
            shape_label = "Square";
        else
            shape_label = "Circle";

        cv::Scalar color(0, 255, 0);
        cv::drawContours(contours_draw, contours, static_cast<int>(i), color, 2);

        cv::Moments mu = cv::moments(contours[i]);
        cv::Point centroid(mu.m10 / mu.m00, mu.m01 / mu.m00);

        cv::putText(contours_draw, shape_label, centroid, cv::FONT_HERSHEY_SIMPLEX, 0.5, color, 2);
    }

    cv::imshow("Canny", edges);
    cv::imshow("Contours", contours_draw);
}

int main(int argc, char* argv[]) {
    std::string filename = (argc == 2) ? argv[1] : "D:/tc.jpg";

    image = cv::imread(filename, cv::IMREAD_COLOR);

    if (image.empty()) {
        std::cerr << "Error: " << filename << std::endl;
        return -1;
    }

    std::cout << "[i] image: " << filename << std::endl;

    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);


    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);

    cv::namedWindow("orig", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("gray", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Canny", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Contours", cv::WINDOW_AUTOSIZE);

    cv::imshow("orig", image);
    cv::imshow("gray", gray);

    cv::createTrackbar("Low", "Canny", &low_threshold, 255, onTrackbar);
    cv::createTrackbar("High", "Canny", &high_threshold, 255, onTrackbar);

    onTrackbar(0, 0);

    cv::waitKey(0);

    cv::destroyAllWindows();

    return 0;
}

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <string>
#include <random>

using namespace cv;
using namespace std;

// 전역변수 선언
int totalBlueCircles = 0;
int totalGreenCircles = 0;
int totalRedCircles = 0;

// 이미지 처리 관련 클래스
class ImageProcessor {
public:
    // 이미지 크기 조절 함수
    void resizeImage(const Mat& inputImage, Mat& outputImage, double scale = 0.7) {
        resize(inputImage, outputImage, Size(), scale, scale);
    }

    // 이미지 출력 함수
    void displayImage(const string& windowName, const Mat& image, int x, int y) {
        namedWindow(windowName, WINDOW_NORMAL);
        moveWindow(windowName, x, y);
        imshow(windowName, image);
        waitKey(300);
    }

    // 게임 설명문 출력
    void startImage(const string& imagePath) {
        Mat image = imread(imagePath, IMREAD_COLOR);
        if (image.empty()) {
            cout << "이미지를 불러오는 데 실패했습니다." << endl;
            return;
        }

        // 원본 이미지 크기 줄이기
        Mat resizedImage;
        resizeImage(image, resizedImage);
        displayImage("Ready?", resizedImage, 500, 200);

        bool shouldExit = false;
        while (!shouldExit) {
            int key = waitKey(500);
            if (key == 'b') { // 'b' 키 입력 시 창 닫기
                shouldExit = true;
                destroyWindow("Ready?");
            }
        }
    }

    // 정답여부 이미지 출력함수
    string getResultImagePath(const string& basePath, int userInput, int detectedCirclesCount) {
        return basePath + ((userInput == detectedCirclesCount) ? "Win.png" : "defeat.png");
    }

    // 벨 누르기 함수
    void ringBell() {

        int userInput = 0; // 사용자로부터 입력받은 키 (변수)
        int waitTime = 500; // 0.5초 대기
        int keyCode = waitKey(waitTime); // 0.5초 동안 키 입력을 기다림
        if (keyCode == 'b') {
            userInput = 5; // 5개의 파란색 원이 감지되면 b 키를 눌러야 합니다.
        }
        else {
            userInput = 0; // 잘못된 입력(벨을 누르지 않음)
        }

        if (totalBlueCircles == 5 || totalGreenCircles == 5 || totalRedCircles == 5) {
            string resultImagePath = getResultImagePath("C:/pic_1/", userInput, 5);

            Mat resultImage = imread(resultImagePath, IMREAD_COLOR);
            if (resultImage.empty()) {
                cout << "결과 이미지를 불러오는 데 실패했습니다." << endl;
                return;
            }

            Mat resizedResultImage;
            resizeImage(resultImage, resizedResultImage);
            displayImage("Result", resizedResultImage, 500, 500);
        }
        else if (userInput == 5) {
            if (totalBlueCircles != 5 && totalGreenCircles != 5 && totalRedCircles != 5) {
                string resultImagePath = getResultImagePath("C:/pic_1/", userInput, 0);

                Mat resultImage = imread(resultImagePath, IMREAD_COLOR);
                if (resultImage.empty()) {
                    cout << "결과 이미지를 불러오는 데 실패했습니다." << endl;
                    return;
                }

                Mat resizedResultImage;
                resizeImage(resultImage, resizedResultImage);
                displayImage("Result", resizedResultImage, 500, 500);
            }
        }
    }

    // (최종) 이미지 출력 함수
    void processImage(const string& imagePath, const string& windowName, int x, int y) {
        Mat image = imread(imagePath, IMREAD_COLOR);
        if (image.empty()) {
            cout << "이미지를 불러오는 데 실패했습니다." << endl;
            return;
        }

        // 원본 이미지 크기 줄이기
        Mat resizedImage;
        resizeImage(image, resizedImage);
        displayImage(windowName, resizedImage, x, y);
    }

    // 이미지 랜덤 호출 함수
    int randomImage(vector<string>& imageFiles) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, static_cast<int>(imageFiles.size() - 1));
        return dis(gen);
    }
};

// 원 검출 관련 클래스
class CircleDetector {
public:
    // 그레이스케일로 변환하는 함수
    void convertToGrayscale(const Mat& inputImage, Mat& outputImage) {
        cvtColor(inputImage, outputImage, COLOR_BGR2GRAY);
    }

    // 가우시안 블러 적용 함수
    void applyGaussianBlur(Mat& image) {
        GaussianBlur(image, image, Size(9, 9), 2, 2);
    }

    // 원 검출 함수
    void detectCircles(const Mat& image, vector<Vec3f>& circles) {
        HoughCircles(image, circles, HOUGH_GRADIENT, 1, image.rows / 8, 200, 100, 0, 0);
    }

    // 파란색 원 검출 및 카운트 함수
    int detectBlueCircles(const string& imagePath) {
        Mat image = imread(imagePath, IMREAD_COLOR);
        Mat gray;
        convertToGrayscale(image, gray);
        applyGaussianBlur(gray);
        vector<Vec3f> circles;
        detectCircles(gray, circles);

        int blueCircles = 0;
        for (size_t i = 0; i < circles.size(); i++) {
            Vec3i c = circles[i];
            Point center = Point(c[0], c[1]);
            // 파란색 원 검출 조건
            if (image.at<Vec3b>(center.y, center.x)[0] > 200 && // 파란색 값이 200 초과
                image.at<Vec3b>(center.y, center.x)[1] < 100 && // 녹색 값이 100 미만
                image.at<Vec3b>(center.y, center.x)[2] < 100) { // 빨간색 값이 100 미만
                blueCircles++;
            }
        }

        return blueCircles;
    }

    // 초록색 원 검출 및 카운트 함수
    int detectGreenCircles(const string& imagePath) {
        Mat image = imread(imagePath, IMREAD_COLOR);
        Mat gray;
        convertToGrayscale(image, gray);
        applyGaussianBlur(gray);
        vector<Vec3f> circles;
        detectCircles(gray, circles);

        int greenCircles = 0;
        for (size_t i = 0; i < circles.size(); i++) {
            Vec3i c = circles[i];
            Point center = Point(c[0], c[1]);
            // 녹색 원 검출 조건
            if (image.at<Vec3b>(center.y, center.x)[0] < 100 && // 파란색 값이 100 미만
                image.at<Vec3b>(center.y, center.x)[1] > 200 && // 녹색 값이 200 초과
                image.at<Vec3b>(center.y, center.x)[2] < 100) { // 빨간색 값이 100 미만
                greenCircles++;
            }
        }

        return greenCircles;
    }

    // 빨간색 원 검출 및 카운트 함수
    int detectRedCircles(const string& imagePath) {
        Mat image = imread(imagePath, IMREAD_COLOR);
        Mat gray;
        convertToGrayscale(image, gray);
        applyGaussianBlur(gray);
        vector<Vec3f> circles;
        detectCircles(gray, circles);

        int redCircles = 0;
        for (size_t i = 0; i < circles.size(); i++) {
            Vec3i c = circles[i];
            Point center = Point(c[0], c[1]);
            // 빨간색 원 검출 조건
            if (image.at<Vec3b>(center.y, center.x)[0] < 100 && // 파란색 값이 100 미만
                image.at<Vec3b>(center.y, center.x)[1] < 100 && // 녹색 값이 100 미만
                image.at<Vec3b>(center.y, center.x)[2] > 200) { // 빨간색 값이 200 초과
                redCircles++;
            }
        }

        return redCircles;
    }
};

// 메인 함수
int main() {
    // 파일 위치 선언
    string imagePath = "C:/pic_1/";

    ImageProcessor imgProcessor;
    CircleDetector circleDetector;

    imgProcessor.startImage(imagePath + "Ready.png");

    // 원 개수 변수 선언
    int totalBlueCircles_1 = 0;
    int totalGreenCircles_1 = 0;
    int totalRedCircles_1 = 0;

    int totalBlueCircles_2 = 0;
    int totalGreenCircles_2 = 0;
    int totalRedCircles_2 = 0;

    int totalBlueCircles_3 = 0;
    int totalGreenCircles_3 = 0;
    int totalRedCircles_3 = 0;

    // 파일 선언
    vector<string> imageFiles = {
        "circle_b1.png", "circle_b2.png", "circle_b3.png",
        "circle_b4.png", "circle_b5.png", "circle_G1.png",
        "circle_G2.png", "circle_G3.png", "circle_G4.png",
        "circle_G5.png", "circle_R1.png", "circle_R2.png",
        "circle_R3.png", "circle_R4.png", "circle_R5.png"
    };

    // 반복문
    do {
        // 랜덤 함수 적용
        string fileName = imageFiles[imgProcessor.randomImage(imageFiles)];

        // 이미지 출력
        imgProcessor.processImage(imagePath + fileName, "image_1", 200, 200);

        // 각 이미지에서 원 개수 검출
        totalBlueCircles_1 = circleDetector.detectBlueCircles(imagePath + fileName);
        totalGreenCircles_1 = circleDetector.detectGreenCircles(imagePath + fileName);
        totalRedCircles_1 = circleDetector.detectRedCircles(imagePath + fileName);

        totalBlueCircles = totalBlueCircles_1 + totalBlueCircles_2 + totalBlueCircles_3;
        totalGreenCircles = totalGreenCircles_1 + totalGreenCircles_2 + totalGreenCircles_3;
        totalRedCircles = totalRedCircles_1 + totalRedCircles_2 + totalRedCircles_3;

        imgProcessor.ringBell(); // 벨 누르기 함수

        // 중간 브레이크
        if (totalBlueCircles == 5 || totalGreenCircles == 5 || totalRedCircles == 5)
            break;

        // 랜덤 함수 초기화
        fileName = imageFiles[imgProcessor.randomImage(imageFiles)];

        imgProcessor.processImage(imagePath + fileName, "image_2", 600, 200);

        // 각 이미지에서 원 개수 검출
        totalBlueCircles_2 = circleDetector.detectBlueCircles(imagePath + fileName);
        totalGreenCircles_2 = circleDetector.detectGreenCircles(imagePath + fileName);
        totalRedCircles_2 = circleDetector.detectRedCircles(imagePath + fileName);

        totalBlueCircles = totalBlueCircles_1 + totalBlueCircles_2 + totalBlueCircles_3;
        totalGreenCircles = totalGreenCircles_1 + totalGreenCircles_2 + totalGreenCircles_3;
        totalRedCircles = totalRedCircles_1 + totalRedCircles_2 + totalRedCircles_3;

        imgProcessor.ringBell(); // 벨 누르기 함수

        if (totalBlueCircles == 5 || totalGreenCircles == 5 || totalRedCircles == 5)
            break;

        // 랜덤 함수 초기화
        fileName = imageFiles[imgProcessor.randomImage(imageFiles)];

        imgProcessor.processImage(imagePath + fileName, "image_3", 1000, 200);

        // 각 이미지에서 원 개수 검출
        totalBlueCircles_3 = circleDetector.detectBlueCircles(imagePath + fileName);
        totalGreenCircles_3 = circleDetector.detectGreenCircles(imagePath + fileName);
        totalRedCircles_3 = circleDetector.detectRedCircles(imagePath + fileName);

        totalBlueCircles = totalBlueCircles_1 + totalBlueCircles_2 + totalBlueCircles_3;
        totalGreenCircles = totalGreenCircles_1 + totalGreenCircles_2 + totalGreenCircles_3;
        totalRedCircles = totalRedCircles_1 + totalRedCircles_2 + totalRedCircles_3;

        imgProcessor.ringBell(); // 벨 누르기 함수

        cout << "\n모든 이미지에서 검출된 총 파란색 원의 개수: " << totalBlueCircles << endl;
        cout << "모든 이미지에서 검출된 총 녹색 원의 개수: " << totalGreenCircles << endl;
        cout << "모든 이미지에서 검출된 총 빨간색 원의 개수: " << totalRedCircles << endl;

    } while (!(totalBlueCircles == 5 || totalGreenCircles == 5 || totalRedCircles == 5));

    cout << "\n@@결과 출력@@ " << endl;
    cout << "모든 이미지에서 검출된 총 파란색 원의 개수: " << totalBlueCircles << endl;
    cout << "모든 이미지에서 검출된 총 녹색 원의 개수: " << totalGreenCircles << endl;
    cout << "모든 이미지에서 검출된 총 빨간색 원의 개수: " << totalRedCircles << endl;

    return 0;
}

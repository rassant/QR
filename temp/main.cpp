#include <algorithm>
#include <ranges>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include <filesystem>
#include <thread>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <qrencode.h>

namespace fs = std::filesystem;

class QRImageProcessor {
private:
    std::mutex mtx;
    const cv::Scalar QR_COLOR = cv::Scalar(0, 0, 0);      // Черный
    const cv::Scalar BG_COLOR = cv::Scalar(255, 255, 255); // Белый
    const int MIN_QR_SIZE = 100;                          // Минимальный размер QR-кода

public:
    void process(const std::string& path, size_t threadCount = 4) {
        if (fs::is_directory(path)) {
            processFolder(path, threadCount);
        } else if (fs::exists(path) && isImageFile(path)) {
            processSingleFile(path);
        } else {
            throw std::runtime_error("Invalid path or file format: " + path);
        }
    }

private:
    void processFolder(const std::string& folderPath, size_t threadCount) {
        std::vector<fs::path> imagePaths = getImagePaths(folderPath);
        
        auto worker = [&](size_t start, size_t end) {
            for (size_t i = start; i < end; ++i) {
                processImage(imagePaths[i]);
            }
        };

        size_t perThread = imagePaths.size() / threadCount;
        std::vector<std::thread> threads;
        for (size_t i = 0; i < threadCount; ++i) {
            size_t start = i * perThread;
            size_t end = (i == threadCount-1) ? imagePaths.size() : start + perThread;
            threads.emplace_back(worker, start, end);
        }

        for (auto& thread : threads) 
        { 
            thread.join();
        }
    }

    void processSingleFile(const std::string& filePath) {
        processImage(fs::path(filePath));
    }

    static auto getImagePaths(const std::string& folderPath) -> std::vector<fs::path> {
        std::vector<fs::path> paths;
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (isImageFile(entry.path())) {
                paths.push_back(entry.path());
            }
        }
        return paths;
    }

    static auto isImageFile(const fs::path& path) -> bool {
        const std::vector<std::string> extensions = {".jpg", ".jpeg", ".png", ".bmp"};
        std::string ext = path.extension().string();
        std::ranges::transform(ext, ext.begin(), ::tolower);
        return std::ranges::find(extensions, ext) != extensions.end();
    }

    void processImage(const fs::path& imagePath) {
        try {
            cv::Mat image = cv::imread(imagePath.string(), cv::IMREAD_COLOR);
            if (image.empty()) { return; }

            cv::Mat qr_code = generateQRCode(imagePath.string(), image.size());
            overlayQRCode(image, qr_code);

            {
                std::lock_guard<std::mutex> lock(mtx);
                cv::imwrite(imagePath.string(), image);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error processing " << imagePath << ": " << e.what() << '\n';
        }
    }

    auto generateQRCode(const std::string& text, const cv::Size& imgSize) -> cv::Mat {
        QRcode* qrcode = QRcode_encodeString(text.c_str(), 0, QR_ECLEVEL_H, QR_MODE_8, 1);
        if (qrcode == nullptr) { throw std::runtime_error("QR code generation failed"); }

        const int baseSize = qrcode->width;
        const int border = 4;
        const int moduleSize = 5; // Размер одного модуля QR-кода

        // Рассчет размера QR-кода
        double maxQRArea = imgSize.area() / 40.0;
        int finalSize = static_cast<int>(sqrt(maxQRArea));
        finalSize = std::max(finalSize, MIN_QR_SIZE);

        cv::Mat qr_code = createRoundedQR(qrcode, baseSize, border, moduleSize);
        cv::resize(qr_code, qr_code, cv::Size(finalSize, finalSize), 0, 0, cv::INTER_CUBIC);

        QRcode_free(qrcode);
        return qr_code;
    }

    auto createRoundedQR(QRcode* qrcode, int size, int border, int moduleSize) -> cv::Mat {
        const int totalSize = (size * moduleSize) + (border * 2);
        cv::Mat qr_code(totalSize, totalSize, CV_8UC3, BG_COLOR);
        const int centerOffset = moduleSize / 2;

        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                if (qrcode->data[y*size + x] & 1) {
                    cv::Point center(
                        border + x * moduleSize + centerOffset,
                        border + y * moduleSize + centerOffset
                    );
                    cv::circle(qr_code, center, moduleSize/2, QR_COLOR, -1, cv::LINE_AA);
                }
            }
        }
        return qr_code;
    }

    void overlayQRCode(cv::Mat& mainImage, const cv::Mat& qrCode) {
        int x = mainImage.cols - qrCode.cols - 20;
        int y = mainImage.rows - qrCode.rows - 20;
        x = std::max(x, 0);
        y = std::max(y, 0);

        cv::Rect roi(x, y, qrCode.cols, qrCode.rows);
        qrCode.copyTo(mainImage(roi));
    }
};

auto main(int argc, char** argv) -> int {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <path> [thread_count]\n"
                  << "  <path> - image file or directory\n"
                  << "  [thread_count] - optional, default 4\n";
        return 1;
    }

    QRImageProcessor processor;
    size_t threadCount = (argc > 2) ? std::stoul(argv[2]) : 4;

    try {

        // для безопасного передачи аргументов
        auto path = std::span(argv, size_t(1));
        std::string path_to_photo = path[1];
        std::cout << path_to_photo << '\n';
        processor.process(path_to_photo, threadCount);


        std::cout << "Processing completed successfully!" << '\n';
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

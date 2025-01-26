#include <iostream>
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
    const cv::Scalar SUN_COLOR = cv::Scalar(0, 255, 255);  // Желтый (BGR)
    const int MIN_QR_SIZE = 200;                          // Минимальный размер QR-кода

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

        for (auto& t : threads) t.join();
    }

    void processSingleFile(const std::string& filePath) {
        processImage(fs::path(filePath));
    }

    std::vector<fs::path> getImagePaths(const std::string& folderPath) {
        std::vector<fs::path> paths;
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (isImageFile(entry.path())) {
                paths.push_back(entry.path());
            }
        }
        return paths;
    }

    bool isImageFile(const fs::path& path) {
        const std::vector<std::string> extensions = {".jpg", ".jpeg", ".png", ".bmp"};
        std::string ext = path.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return std::find(extensions.begin(), extensions.end(), ext) != extensions.end();
    }

    void processImage(const fs::path& imagePath) {
        try {
            cv::Mat image = cv::imread(imagePath.string(), cv::IMREAD_COLOR);
            if (image.empty()) return;

            cv::Mat qr = generateSunQRCode(imagePath.string(), image.size());
            overlayQRCode(image, qr);

            {
                std::lock_guard<std::mutex> lock(mtx);
                cv::imwrite(imagePath.string(), image);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error processing " << imagePath << ": " << e.what() << std::endl;
        }
    }

    cv::Mat generateSunQRCode(const std::string& text, const cv::Size& imgSize) {
        QRcode* qrcode = QRcode_encodeString(text.c_str(), 5, QR_ECLEVEL_H, QR_MODE_8, 1);
        if (!qrcode) throw std::runtime_error("QR code generation failed");

        const int baseSize = qrcode->width;
        const int border = 4;
        const int moduleSize = 5; // Размер одного модуля QR-кода

        // Рассчет размера QR-кода
        double maxQRArea = imgSize.area() / 20.0;
        int finalSize = static_cast<int>(sqrt(maxQRArea));
        finalSize = std::max(finalSize, MIN_QR_SIZE);

        // Создание базового QR-кода с округлыми модулями
        cv::Mat qr = createRoundedQR(qrcode, baseSize, border, moduleSize);
        
        // Масштабирование и добавление дизайна
        cv::resize(qr, qr, cv::Size(finalSize, finalSize), 0, 0, cv::INTER_CUBIC);
        addSunDesign(qr);

        QRcode_free(qrcode);
        return qr;
    }

    cv::Mat createRoundedQR(QRcode* qrcode, int size, int border, int moduleSize) {
        const int totalSize = size * moduleSize + border * 2;
        cv::Mat qr(totalSize, totalSize, CV_8UC3, BG_COLOR);
        const int centerOffset = moduleSize / 2;

        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                if (qrcode->data[y*size + x] & 1) {
                    cv::Point center(
                        border + x * moduleSize + centerOffset,
                        border + y * moduleSize + centerOffset
                    );
                    // Рисуем округлые модули
                    cv::circle(qr, center, moduleSize/2, QR_COLOR, -1, cv::LINE_AA);
                }
            }
        }
        return qr;
    }

    void addSunDesign(cv::Mat& qr) {
        const int size = qr.rows;
        cv::Point center(size/2, size/2);

        // Солнечные лучи
        for (int i = 0; i < 360; i += 15) {
            double angle = i * CV_PI / 180.0;
            cv::Point pt(
                center.x + (size/2 - 10) * cos(angle),
                center.y + (size/2 - 10) * sin(angle)
            );
            cv::line(qr, center, pt, SUN_COLOR, 2, cv::LINE_AA);
        }

        // Центральное солнце
        cv::circle(qr, center, size/10, SUN_COLOR, -1, cv::LINE_AA);
        cv::circle(qr, center, size/20, QR_COLOR, -1, cv::LINE_AA);
    }

    void overlayQRCode(cv::Mat& mainImage, const cv::Mat& qrCode) {
        int x = mainImage.cols - qrCode.cols - 20;
        int y = mainImage.rows - qrCode.rows - 20;
        x = std::max(x, 0);
        y = std::max(y, 0);

        cv::Mat roi = mainImage(cv::Rect(x, y, qrCode.cols, qrCode.rows));
        cv::addWeighted(roi, 0.7, qrCode, 0.5, 0.0, roi);
    }
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <path> [thread_count]\n"
                  << "  <path> - image file or directory\n"
                  << "  [thread_count] - optional, default 4\n";
        return 1;
    }

    QRImageProcessor processor;
    size_t threadCount = (argc > 2) ? std::stoul(argv[2]) : 4;

    try {
        processor.process(argv[1], threadCount);
        std::cout << "Processing completed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}


/*g++ -std=c++17 -O2 -Wall main.cpp -o processor \*/
/*    $(pkg-config --cflags --libs opencv4) \*/
/*    -lqrencode*/



#include <iostream>
#include <vector>
#include <filesystem>
#include <thread>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv.hpp>
#include <qrencode.h>

namespace fs = std::filesystem;

class QRImageProcessor {
private:
    std::mutex mtx; // Мьютекс для потокобезопасных операций

public:
    // Основной метод обработки папки (может быть запущен в отдельном потоке)
    void processFolder(const std::string& folderPath, size_t threadCount = 4) {
        std::vector<fs::path> imagePaths = getImagePaths(folderPath);
        
        // Лямбда для обработки задач в потоках
        auto worker = [&](size_t start, size_t end) {
            for (size_t i = start; i < end; ++i) {
                processSingleImage(imagePaths[i]);
            }
        };

        // Распределение задач по потокам
        size_t perThread = imagePaths.size() / threadCount;
        std::vector<std::thread> threads;
        for (size_t i = 0; i < threadCount; ++i) {
            size_t start = i * perThread;
            size_t end = (i == threadCount-1) ? imagePaths.size() : start + perThread;
            threads.emplace_back(worker, start, end);
        }

        for (auto& t : threads) t.join();
    }

private:
    // Получение списка изображений в папке
    std::vector<fs::path> getImagePaths(const std::string& folderPath) {
        std::vector<fs::path> paths;
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (isImageFile(entry.path())) {
                paths.push_back(entry.path());
            }
        }
        return paths;
    }

    // Проверка расширения файла
    bool isImageFile(const fs::path& path) {
        const std::vector<std::string> extensions = {".jpg", ".jpeg", ".png", ".bmp"};
        std::string ext = path.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return std::find(extensions.begin(), extensions.end(), ext) != extensions.end();
    }

    // Обработка одного изображения
    void processSingleImage(const fs::path& imagePath) {
        try {
            cv::Mat image = cv::imread(imagePath.string(), cv::IMREAD_COLOR);
            if (image.empty()) return;

            // Генерация QR кода
            cv::Mat qr = generateQRCode(imagePath.string(), image.size());
            
            // Наложение QR кода
            overlayQRCode(image, qr);
            
            // Сохранение с блокировкой
            {
                std::lock_guard<std::mutex> lock(mtx);
                cv::imwrite(imagePath.string(), image);
            }
        } catch (...) {
            std::cerr << "Error processing: " << imagePath << std::endl;
        }
    }

cv::Mat generateQRCode(const std::string& text, const cv::Size& imgSize) {
        // Увеличиваем версию QR-кода для поддержки большего размера
        QRcode* qrcode = QRcode_encodeString(text.c_str(), 5, QR_ECLEVEL_L, QR_MODE_8, 1);
        if (!qrcode) throw std::runtime_error("QR code generation failed");

        // Рассчет максимальной площади (1/20 от исходного изображения)
        const double maxQRArea = imgSize.area() / 100.0;
        
        // Оптимальный размер стороны QR-кода с учетом его собственного соотношения
        const int baseSize = qrcode->width;
        const double scaleFactor = sqrt(maxQRArea) / baseSize;
        
        // Вычисляем финальный размер с округлением
        int finalSize = static_cast<int>(baseSize * scaleFactor + 0.5);
        finalSize = std::max(finalSize, 25); // Минимальный размер 25x25

        // Создаем базовое изображение QR-кода
        cv::Mat qrBase(baseSize, baseSize, CV_8UC1, cv::Scalar(255));
        for (int y = 0; y < baseSize; ++y) {
            for (int x = 0; x < baseSize; ++x) {
                if (qrcode->data[y * baseSize + x] & 1) {
                    qrBase.at<uchar>(y, x) = 0;
                }
            }
        }

        // Масштабируем с использованием бикубической интерполяции
        cv::Mat qrScaled;
        cv::resize(qrBase, qrScaled, cv::Size(finalSize, finalSize), 
                 0, 0, cv::INTER_CUBIC);

        // Конвертируем в цветное изображение
        cv::Mat qrColor;
        cv::cvtColor(qrScaled, qrColor, cv::COLOR_GRAY2BGR);

        QRcode_free(qrcode);
        return qrColor;
    }



    // Генерация QR кода
    /*cv::Mat generateQRCode(const std::string& text, const cv::Size& imgSize) {*/
    /*    QRcode* qrcode = QRcode_encodeString(text.c_str(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);*/
    /*    if (!qrcode) throw std::runtime_error("QR code generation failed");*/
    /**/
    /*    // Расчет максимального размера (1/20 площади)*/
    /*    int maxSide = static_cast<int>(sqrt(imgSize.area() / 20));*/
    /**/
    /*    // Определение размера QR с масштабированием*/
    /*    int qrSize = std::min(qrcode->width, maxSide);*/
    /*    float scale = static_cast<float>(qrSize) / qrcode->width;*/
    /**/
    /*    // Создание изображения QR*/
    /*    cv::Mat qr(qrSize, qrSize, CV_8UC1, cv::Scalar(255));*/
    /*    for (int y = 0; y < qrcode->width; ++y) {*/
    /*        for (int x = 0; x < qrcode->width; ++x) {*/
    /*            if (qrcode->data[y * qrcode->width + x] & 1) {*/
    /*                int ys = static_cast<int>(y * scale);*/
    /*                int xs = static_cast<int>(x * scale);*/
    /*                if (xs < qrSize && ys < qrSize) {*/
    /*                    qr.at<uchar>(ys, xs) = 0;*/
    /*                }*/
    /*            }*/
    /*        }*/
    /*    }*/
    /**/
    /*    QRcode_free(qrcode);*/
    /*    cv::cvtColor(qr, qr, cv::COLOR_GRAY2BGR);*/
    /*    return qr;*/
    /*}*/

    // Наложение QR кода на изображение
    void overlayQRCode(cv::Mat& mainImage, const cv::Mat& qrCode) {
        int x = mainImage.cols - qrCode.cols - 10; // Отступ 10 пикселей
        int y = mainImage.rows - qrCode.rows - 10;
        
        // Проверка границ
        x = std::max(x, 0);
        y = std::max(y, 0);
        
        cv::Rect roi(x, y, qrCode.cols, qrCode.rows);
        qrCode.copyTo(mainImage(roi));
    }
};

int main() {
    QRImageProcessor processor;
    processor.processFolder("../resource/to/", 4); // 4 потока
    return 0;
}

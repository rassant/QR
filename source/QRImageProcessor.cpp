#include "../header/QRImageProcessor.hpp"
#include <algorithm>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <qrencode.h>



void QRImageProcessor::
Process  (  const std::string& path,  size_t threadCount ) {
    if  ( std::filesystem::is_directory  ( path )) {
        ProcessFolder  ( path, threadCount ) ;
    } else if  ( std::filesystem::exists  ( path ) && IsImageFile  ( path )) {
        ProcessSingleFile  ( path ) ;
    } else {
        throw std::runtime_error  ( "Не верный путь к файлам: " + path ) ;
    }
}



void QRImageProcessor::
ProcessFolder  ( const std::string& folderPath, size_t threadCount ) 
{
    std::vector<std::filesystem::path> imagePaths = GetImagePaths  ( folderPath ) ;

    auto worker = [&]  ( size_t start, size_t end ) {
        for  ( size_t i = start; i < end; ++i ) {
            ProcessImage  ( imagePaths[i] ) ;
        }
    };

    size_t perThread = imagePaths.size  ( ) / threadCount;
    std::vector<std::thread> threads;
    for  ( size_t i = 0; i < threadCount; ++i ) {
        size_t start = i * perThread;
        size_t end =  ( i == threadCount-1 ) ? imagePaths.size  ( ) : start + perThread;
        threads.emplace_back  ( worker, start, end ) ;
    }

    for  ( auto& thread : threads ) 
    { 
        thread.join  ( ) ;
    }
}



void QRImageProcessor::
ProcessSingleFile  ( const std::string& filePath ) {
    ProcessImage  ( std::filesystem::path  ( filePath )) ;
}



auto QRImageProcessor::
GetImagePaths  ( const std::string& folderPath ) -> std::vector<std::filesystem::path> {
    std::vector<std::filesystem::path> paths;
    for  ( const auto& entry : std::filesystem::directory_iterator  ( folderPath )) {
        if  ( IsImageFile  ( entry.path  ( )) ) {
            paths.emplace_back  ( entry.path  ( )) ;
        }
    }
    return paths;
}



auto  QRImageProcessor::
IsImageFile  ( const std::filesystem::path& path ) -> bool {
    const std::vector<std::string> extensions = {".jpg", ".jpeg", ".png", ".bmp"};
    std::string ext = path.extension( ).string  ( ) ;
    std::ranges::transform  ( ext, ext.begin  ( ) , ::tolower ) ;
    return std::ranges::find  ( extensions, ext ) != extensions.end  ( ) ;
}



void QRImageProcessor::
ProcessImage  ( const std::filesystem::path& imagePath ) {
    try {
        cv::Mat image = cv::imread  ( imagePath.string  ( ) , cv::IMREAD_COLOR ) ;
        if  ( image.empty  ( )) { return; }

        cv::Mat qr_code = GenerateQRCode  ( imagePath.string  ( ) , image.size  ( )) ;
        OverlayQRCode  ( image, qr_code ) ;

        {
            std::lock_guard<std::mutex> lock  ( mtx ) ;
            cv::imwrite  ( imagePath.string  ( ) , image ) ;
        }
    } catch  ( const std::exception& e ) {
        std::cerr << "Error processing " << imagePath << ": " << e.what  ( ) << '\n';
    }
}



auto QRImageProcessor::
GenerateQRCode  ( const std::string& text, const cv::Size& imgSize ) -> cv::Mat {
    QRcode* qrcode = QRcode_encodeString  ( text.c_str  ( ) , 0, QR_ECLEVEL_H, QR_MODE_8, 1 ) ;
    if  ( qrcode == nullptr ) { throw std::runtime_error  ( "QR code generation failed" ) ; }

    const int baseSize = qrcode->width;
    const int border = 4;
    const int moduleSize = 5; // Размер одного модуля QR-кода
    const double area = 40.0; // размер QR code должен быть 1/40 от всего изображения

    // Рассчет размера QR-кода
    double maxQRArea = imgSize.area  ( ) / area;
    int finalSize = static_cast<int>  ( sqrt  ( maxQRArea )) ;
    finalSize = std::max  ( finalSize, MIN_QR_SIZE ) ;

    cv::Mat qr_code = CreateRoundedQR  ( qrcode, baseSize, border, moduleSize ) ;
    cv::resize  ( qr_code, qr_code, cv::Size  ( finalSize, finalSize ) , 0, 0, cv::INTER_CUBIC ) ;

    QRcode_free  ( qrcode ) ;
    return qr_code;
}



auto QRImageProcessor::
CreateRoundedQR  ( QRcode* qrcode, int size, int border, int moduleSize ) -> cv::Mat {
    const int totalSize =  ( size * moduleSize ) +  ( border * 2 ) ;
    cv::Mat qr_code  ( totalSize, totalSize, CV_8UC3, BG_COLOR ) ;
    const int centerOffset = moduleSize / 2;

    for  ( int y = 0; y < size; ++y ) {
        for  ( int x = 0; x < size; ++x ) {
            if  ( qrcode->data[(y*size) + x] & 1 ) {
                cv::Point center  ( 
                    border + (x * moduleSize) + centerOffset,
                    border + (y * moduleSize) + centerOffset
 ) ;
                cv::circle  ( qr_code, center, moduleSize/2, QR_COLOR, -1, cv::LINE_AA ) ;
            }
        }
    }
    return qr_code;
}



void QRImageProcessor::
OverlayQRCode  ( cv::Mat& mainImage, const cv::Mat& qrCode ,  int indentation ) {
    int qr_code_pos_x = mainImage.cols - qrCode.cols - indentation;
    int qr_code_pos_y = mainImage.rows - qrCode.rows - indentation;

    qr_code_pos_x = std::max  ( qr_code_pos_x, 0 ) ;
    qr_code_pos_y = std::max  ( qr_code_pos_y, 0 ) ;

    cv::Rect roi  ( qr_code_pos_x, qr_code_pos_y, qrCode.cols, qrCode.rows ) ;
    qrCode.copyTo  ( mainImage  ( roi )) ;
}

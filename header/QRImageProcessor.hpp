#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <qrencode.h>



class QRImageProcessor {
	
private:
	std::mutex mtx;
	const cv::Scalar QR_COLOR = cv::Scalar ( 0, 0, 0 );      // Черный
	const cv::Scalar BG_COLOR = cv::Scalar ( 255, 255, 255 ); // Белый

	const int MIN_QR_SIZE = 100;// Минимальный размер QR-кода

	void ProcessFolder     ( const std::string& folderPath,  size_t threadCount ) ; // путь к папке, количество потоков
	void ProcessSingleFile ( const std::string& filePath ) ; // путь к фотографии

	static auto GetImagePaths ( const std::string&     folderPath  ) -> std::vector<std::filesystem::path>;
	static auto IsImageFile   ( const std::filesystem::path& path  ) -> bool;

	void ProcessImage  ( const std::filesystem::path& imagePath ) ;

	auto GenerateQRCode   ( const std::string& text,  const cv::Size& imgSize ) -> cv::Mat;
	static void OverlayQRCode  ( cv::Mat& mainImage,  const cv::Mat&  qrCode,  int indentation = 30) ;

	auto CreateRoundedQR ( QRcode* qrcode, int size,  int border,  int moduleSize ) -> cv::Mat;

public:
	void Process (const std::string& path,  size_t threadCount = 4);
};

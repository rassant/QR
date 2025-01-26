#pragma once
#include <filesystem>
#include <exiv2/exiv2.hpp>
#include <unordered_set>

class FileCopying 
{
private:
	std::filesystem::path from_;
	std::filesystem::path to_;
mutable std::unordered_set<uint32_t> target_crc_cache_;


void ValidateDirectories ( ) const; // проверка валидности путей
void ProcessFiles();
std::filesystem::path GetAvailablePath  (const std::filesystem::path& source) const ;
void ProcessSingleFile(const std::filesystem::path& source) const;
void      CopyWithRetries  ( const std::filesystem::path& source  // копирование с повторными попытками
			   , const std::filesystem::path& destination ) const;

bool      VerifyChecksum   ( uint32_t source_crc  // проверка контрольных сумм
			   , const std::filesystem::path& destination ) const ;

    void BuildCRCCache(); // Обновляет target_crc_cache_
    uint32_t  CalculateCRC  ( const std::filesystem::path& ) const; // вычисление контрольной суммы файла

public:
	FileCopying (const std::string & from, const std::string &to);
    void Run(); // Основной метод для запуска копирования
};




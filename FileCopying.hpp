#pragma once
#include <filesystem>
#include <exiv2/exiv2.hpp>
#include <unordered_set>

class FileCopying 
{
private:
    int wait_second_;
	std::filesystem::path from_;
	std::filesystem::path to_;
mutable std::unordered_set<uint32_t> target_crc_cache_;

std::filesystem::path GetAvailablePath  (const std::filesystem::path& source) const ;

void  ValidateDirectories ( ) const; // проверка валидности путей
void  ProcessFiles        ( );
void  ProcessSingleFile   ( const std::filesystem::path& source) const;
void  CopyWithRetries     ( const std::filesystem::path& source , const std::filesystem::path& destination ) const;// копирование с повторными попытками
bool  VerifyChecksum      ( uint32_t source_crc , const std::filesystem::path& destination ) const ; // проверка контрольных сумм
void  BuildCRCCache       ( ); // Обновляет target_crc_cache_
uint32_t CalculateCRC     ( const std::filesystem::path& ) const; // вычисление контрольной суммы файла

public:
	FileCopying (const std::string & from, const std::string &to, const int wait_second );
    
	void Run();
};




#pragma once
#include <filesystem>
#include <exiv2/exiv2.hpp>
#include <unordered_set>

struct FromSourceTag {};  // Тег для исходного пути
struct ToDestinationTag {}; // Тег для целевого пути

class FileCopying 
{
public:
	std::filesystem::path from_source_;      // откуда копировать 
	std::filesystem::path to_destination_;   // куда копировать
			  /*int wait_second_;*/
 std::unordered_set<uint32_t> target_crc_cache_;


// проверка что пути существуют
void  ValidateDirectories ( ) const; 

void  ProcessFiles       ( );
void  ProcessSingleFile  ( const std::filesystem::path& source) const;

// копирование с повторными попытками
static void  CopyWithRetries  ( const std::filesystem::path& source 
                       , const std::filesystem::path& destination ) ;
// проверка контрольных сумм
static auto  VerifyChecksum   ( uint32_t source_crc 
                       , const std::filesystem::path& destination ) -> bool ;
 // Обновляет target_crc_cache_

void  BuildCRCCache    ( );

// вычисление контрольной суммы файла
static auto  CalculateCRC  ( const std::filesystem::path& ) -> uint32_t  ;

auto GetAvailablePath  (const std::filesystem::path& source) const -> std::filesystem::path ;


public:
	FileCopying (FromSourceTag,    const std::string & from_source
               , ToDestinationTag, const std::string & to_destination);
    
	void Run();
};




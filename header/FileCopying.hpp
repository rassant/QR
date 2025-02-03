#pragma once
#include <filesystem>
#include <exiv2/exiv2.hpp>
#include <unordered_set>
#include <deque>

struct FromSourceTag {};  // Тег для исходного пути
struct ToDestinationTag {}; // Тег для целевого пути

class FileCopying 
{
private:
	std::filesystem::path from_source_;      // откуда копировать 
	std::filesystem::path to_destination_;   // куда копировать
			  /*int wait_second_;*/
	std::unordered_set<uint32_t> target_crc_cache_;



	void  ValidateDirectories ( ) const; // проверка что пути существуют
	void  ProcessFiles       ( );
	/*void  ProcessSingleFile  ( const std::filesystem::path& source) const;*/
       void ProcessSingleFile(const std::filesystem::path& source) ;

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
	
	// для отслеживания какие файлы были скопированы (что бы потом их можно было бы удалить);
	std::unordered_set<std::filesystem::path> copied_files_;

public:

	FileCopying (FromSourceTag,    const std::string & from_source
	           , ToDestinationTag, const std::string & to_destination);
	auto Run() -> 	std::unordered_set<std::filesystem::path> ;
	void DeleteCopingFiles ();
};




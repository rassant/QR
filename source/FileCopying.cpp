#include "../header/FileCopying.hpp"
#include <filesystem>
#include <format>
#include <iterator>
#include <stdexcept>
#include <array>
#include <boost/crc.hpp>
#include <iostream>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

FileCopying::
FileCopying (FromSourceTag /*unused*/,   const std::string & from_source
	   , ToDestinationTag /*unused*/,const std::string & to_destination) 
    :   from_source_(from_source), to_destination_(to_destination)
{
//     ValidateDirectories();
}


void FileCopying::
Copying (FromSourceTag /*unused*/,   const std::string & from_source
	   , ToDestinationTag /*unused*/,const std::string & to_destination) 
{   
     from_source_ = from_source;
     to_destination_ = to_destination;
     Run();
}

void FileCopying::
ValidateDirectories() const {
    if (!fs::exists(from_source_) || !fs::is_directory(from_source_)) {
        throw std::runtime_error("Исходная директория недоступна: " + from_source_.string());
    }
    if (!fs::exists(to_destination_) || !fs::is_directory(to_destination_)) {
        throw std::runtime_error("Целевая директория недоступна: " + to_destination_.string());
    }
}


void
FileCopying::Run() {
    BuildCRCCache();
    
    for (const auto& entry : fs::directory_iterator(from_source_)) {
        if (entry.is_regular_file()) {
            try {
                ProcessSingleFile(entry.path());  // Передаем список для заполнения
            } catch (const std::exception& e) {
                std::cerr << "Error processing " << entry.path() << ": " << e.what() << '\n';
            }
        }
    }
}


void FileCopying::
BuildCRCCache() {
    target_crc_cache_.clear();
    for (const auto& entry : fs::directory_iterator(to_destination_)) {
        if (entry.is_regular_file()) {
            try {
                target_crc_cache_.insert(CalculateCRC(entry.path()));
            } catch (const std::exception& e) {
                std::cerr << "Warning: Не могу прочитать файл " << entry.path() << " - " << e.what() << '\n';
            }
        }
    }
}


void FileCopying::
ProcessFiles() {
    for (const auto& entry : fs::directory_iterator(from_source_)) {
        if (entry.is_regular_file()) {
            try {
                ProcessSingleFile(entry.path());
            } catch (const std::exception& e) {
                std::cerr << "Error: Обработки файла " << entry.path() << ": " << e.what() << '\n';
            }
        }
    }
    /*std::cout << "Следующее копирование файлов через "<< wait_second_<< " секунд."<< '\n';*/
}

void FileCopying::ProcessSingleFile(const fs::path& source) {

    copied_files_.insert(source);  // Добавляем исходный путь в список для удаления. 
                                   // Удаляем и то что уже было скопировано. 
                                   // Если поставить в конец то не будет дубликаты удалять
    
    const uint32_t source_crc = CalculateCRC(source);
    const fs::path dest_path = GetAvailablePath(source);

    if (target_crc_cache_.contains(source_crc)) {
        return;  // Файл с таким CRC уже существует
    }

    CopyWithRetries(source, dest_path);
    std::cout << "Успешно скопирован: " << dest_path.filename() << '\n';
}



auto FileCopying::
CalculateCRC( const fs::path& file_path ) -> uint32_t  {
	std::ifstream file( file_path, std::ios::binary );
	if ( !file ) { throw std::runtime_error( "Не могу открыть файл: " + file_path.string( )); }

	boost::crc_32_type crc;

	constexpr size_t BUFFER_SIZE = 4096; 
	std::array< char, BUFFER_SIZE> buffer{};

	while ( file.read( std::data(buffer), sizeof( buffer ))) {
		crc.process_bytes( std::data(buffer), sizeof( buffer ));
	}
	crc.process_bytes( std::data(buffer), static_cast<std::size_t>(file.gcount( )));

	return crc.checksum( );
}


auto FileCopying::
GetAvailablePath( const fs::path& source ) const -> fs::path {
	fs::path target_file = to_destination_ / source.filename( );

	if ( !fs::exists( target_file )) {
		return target_file;
	}

	const std::string stem = source.stem( ).string( );
	const std::string ext  = source.extension( ).string( );
	int counter = 1;

	while ( true ) {
		const std::string new_name = std::format("{}_{}{}", stem, counter++, ext);
		const  fs::path   new_path = to_destination_ / new_name;

		if ( !fs::exists( new_path )) {
			return new_path;
		}
	}
}


void FileCopying::
CopyWithRetries( const fs::path& source, const fs::path& destination ) {
    constexpr int max_attempts = 5;
    int attempt = 0;
    const uint32_t source_crc = CalculateCRC( source ); // контрольная сумма файла

    while ( attempt++ < max_attempts ) {
        try {
            fs::copy_file( source, destination, fs::copy_options::none );
            
            if ( VerifyChecksum( source_crc, destination )) {
                return;
            }
            
            fs::remove( destination );
            std::cerr << "Ошибка контрольной суммы CRC. При попытке номер" << attempt <<"\n";
        } catch ( const std::exception& e ) {
            std::cerr << "Ошибка копирования ( " << attempt << "/" << max_attempts << " ): " 
                      << e.what( ) << '\n';
        }
    }
    
    throw std::runtime_error( "Ошибка при копировании файла: " + source.string( ));
}


auto FileCopying::
VerifyChecksum( uint32_t source_crc, const fs::path& destination) -> bool {
    return source_crc == CalculateCRC( destination);
}




void FileCopying::
DeleteCopingFiles ()
{
    for (const auto& path : copied_files_)
    {
        if (fs::exists(path))
        {
            std::filesystem::remove(path); 
        }
    }

    copied_files_.clear();
}

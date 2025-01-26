#include "FileCopying.hpp"
#include <filesystem>
#include <stdexcept>
#include <boost/crc.hpp>
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;

FileCopying::
FileCopying(const std::string& from, const std::string& to) 
    : from_(from), to_(to) 
{
    ValidateDirectories();
}


void FileCopying::Run() {
    BuildCRCCache(); // Обновляем кэш перед каждым запуском
    ProcessFiles();
}


void FileCopying::
ValidateDirectories() const {
    if (!fs::exists(from_) || !fs::is_directory(from_)) {
        throw std::runtime_error("Исходная директория недоступна: " + from_.string());
    }
    if (!fs::exists(to_) || !fs::is_directory(to_)) {
        throw std::runtime_error("Целевая директория недоступна: " + to_.string());
    }
}


void FileCopying::
BuildCRCCache() {
    target_crc_cache_.clear();
    for (const auto& entry : fs::directory_iterator(to_)) {
        if (entry.is_regular_file()) {
            try {
                target_crc_cache_.insert(CalculateCRC(entry.path()));
            } catch (const std::exception& e) {
                std::cerr << "Warning: Не могу прочитать файл " << entry.path() << " - " << e.what() << std::endl;
            }
        }
    }
}


void FileCopying::
ProcessFiles() {
    for (const auto& entry : fs::directory_iterator(from_)) {
        if (entry.is_regular_file()) {
            try {
                ProcessSingleFile(entry.path()); // Вызов без передачи кэша
            } catch (const std::exception& e) {
                std::cerr << "Error: Обработки файла " << entry.path() << ": " << e.what() << std::endl;
            }
        }
    }
    std::cout << "Копирование файлов завершено." << std::endl;
}


void FileCopying::
ProcessSingleFile(const fs::path& source) const {
    const uint32_t source_crc = CalculateCRC(source);
    
    if (target_crc_cache_.count(source_crc)) {
        std::cout << "Файл уже существует: " << source.filename() << std::endl;
        return;
    }

    const fs::path dest_path = GetAvailablePath(source);
    CopyWithRetries(source, dest_path);
    std::cout << "Успешно скопирован: " << dest_path.filename() << std::endl;
}


uint32_t FileCopying::
CalculateCRC( const fs::path& file_path ) const {
    std::ifstream file( file_path, std::ios::binary );
    if ( !file ) throw std::runtime_error( "Не могу открыть файл: " + file_path.string( ));

    boost::crc_32_type crc;
    char buffer[4096];
    
    while ( file.read( buffer, sizeof( buffer ))) {
        crc.process_bytes( buffer, sizeof( buffer ));
    }
    crc.process_bytes( buffer, file.gcount( ));

    return crc.checksum( );
}


fs::path FileCopying::
GetAvailablePath( const fs::path& source ) const {
    const fs::path target_file = to_ / source.filename( );
    
    if ( !fs::exists( target_file )) {
        return target_file;
    }

    const std::string stem = source.stem( ).string( );
    const std::string ext = source.extension( ).string( );
    int counter = 1;

    while ( true ) {
        const std::string new_name = stem + "_" + std::to_string( counter++ ) + ext;
        const fs::path new_path = to_ / new_name;
        
        if ( !fs::exists( new_path )) {
            return new_path;
        }
    }
}


void FileCopying::
CopyWithRetries( const fs::path& source, const fs::path& destination ) const {
    constexpr int max_attempts = 5;
    int attempt = 0;
    const uint32_t source_crc = CalculateCRC( source );

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
                      << e.what( ) << std::endl;
        }
    }
    
    throw std::runtime_error( "Ошибка при копировании файла: " + source.string( ));
}


bool FileCopying::
VerifyChecksum( uint32_t expected_crc, const fs::path& file ) const {
    return expected_crc == CalculateCRC( file );
}

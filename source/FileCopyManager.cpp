#include "../header/FileCopyManager.hpp"
#include "../header/FileCopying.hpp"
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string>
#include <chrono>
#include <random>

FileCopyManager::FileCopyManager ( PhotographerCollection path_to_flash 
                                 , WorkDirectoryManager   work_directories ) : paths_flash_ (std::move (path_to_flash))
                                                                             , path_save_   (std::move (work_directories))
{
    ScanDataDirectory ();
    CopyFromFlash ();
}


auto FileCopyManager::IsDirectoryEmpty(const std::filesystem::path& dir) -> bool {
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        throw std::runtime_error("Указанный путь не существует или не является директорией");
    }
    return std::filesystem::begin(std::filesystem::directory_iterator(dir)) == std::filesystem::end(std::filesystem::directory_iterator());
}


void FileCopyManager::
MakeDirectoryIfNotExists
(const std::filesystem::path & path_to_folder)
{
        if ( !std::filesystem::exists(path_to_folder)) // папка не существует
        {
            try {
                std::filesystem::create_directories(path_to_folder);
            }
            catch (const std::filesystem::filesystem_error& e)
            {
                std::cerr << "Ошибка при создании директории" << e.what() << '\n';
            }
        }
}


void FileCopyManager::CopyFromFlash ()
{
    for (const auto & [name,paths] : paths_flash_.GetData())
    {
        std::filesystem::path folder_name_photograph = path_save_.GetTempWork()/  name;
        MakeDirectoryIfNotExists (folder_name_photograph);


        for (const auto& path : paths)
        {
            if ( !std::filesystem::exists(path)) // папка не существует
            {
                continue;
            }

            // копируем во временные файлы фотографов
            auto coping = FileCopying(FromSourceTag{}, path, ToDestinationTag{}, folder_name_photograph);
            coping.Run();

            const auto path_for_archiv = path_save_.GetArchive();
            MakeDirectoryIfNotExists (path_for_archiv);

            // копируем в папку архив
            // добавить создание папки с сегодняшней датой
            std::filesystem::path date_folder_name = MakeDataDirectoryIfNotExists(path_for_archiv);
            coping.Copying(FromSourceTag{}, path, ToDestinationTag{}, date_folder_name);
            coping.DeleteCopingFiles();
        }
    }
}


auto FileCopyManager::MakeDataDirectoryIfNotExists(const std::filesystem::path& path_to_folder) -> std::filesystem::path
{
    // Получаем текущую дату
    auto now = std::chrono::system_clock::now();
    auto today = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&today);

    std::ostringstream date_stream;
    date_stream << std::put_time(&local_tm, "%d_%m_%Y");
    std::string date_folder_name = date_stream.str();

    // создаем ункальное имя директории
    const size_t random_letter_count = 10;
    std::filesystem::path new_dir = path_to_folder / (date_folder_name + "__" + GenerateRandomWord(random_letter_count));
    
    try {
        if (!name_dates_infolders_.contains(date_folder_name))
        {
            std::filesystem::create_directories(new_dir);
            name_dates_infolders_[date_folder_name] = new_dir;
        } else 
        {
            return name_dates_infolders_.at(date_folder_name);
        }
            
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Ошибка при создании папки с датой: " << e.what() << '\n';
        return {}; // Возвращаем пустой путь при ошибке
    }

    return new_dir;
}

auto
FileCopyManager::
GenerateRandomWord(size_t length) -> std::string {
    std::mt19937 generator(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    
    std::string query;
    query.resize (length);


    for (char& character : query) {
        const int random_letters = 26; 
        const int rnd = std::uniform_int_distribution(0, random_letters - 1)(generator);
        character = static_cast<char>('a' + rnd); 
    }
    return query;
}


void FileCopyManager::ScanDataDirectory ()
{
    if (std::filesystem::exists(path_save_.GetArchive()))
    {
        for (const std::filesystem::path &path: std::filesystem::directory_iterator (path_save_.GetArchive()))
        {
            if (!name_dates_infolders_.contains (path))
            {
                name_dates_infolders_[path.filename().string().substr(0,10)] = path;
            }
        }
    }
}

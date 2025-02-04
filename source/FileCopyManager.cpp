#include "../header/FileCopyManager.hpp"
#include "../header/FileCopying.hpp"
#include <filesystem>
#include <chrono>
#include <format>
#include <iomanip>
#include <sstream>

FileCopyManager::FileCopyManager ( PhotographerCollection path_to_flash 
                                 , WorkDirectoryManager   work_directories ) : paths_flash_ (std::move (path_to_flash))
                                                                             , path_save_   (std::move (work_directories))
{
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


auto FileCopyManager::
MakeDataDirectoryIfNotExists(const std::filesystem::path & path_to_folder) -> std::filesystem::path
{
    auto now = std::chrono::system_clock::now();
    auto today = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&today);

    std::ostringstream date_stream;
    date_stream << std::put_time(&local_tm, "%d_%m_%Y");
    std::string date_folder_name = date_stream.str();

    std::filesystem::path date_dir = path_to_folder / date_folder_name;


    if (!std::filesystem::exists(date_dir))
    {
        try {
            std::filesystem::create_directories (date_dir);
        }
        catch (std::filesystem::filesystem_error &e)
        {
            std::cerr << "Ошибка при создании папки с датой\n";
        }

    }
    return date_dir;
}

#include "../header/FileCopyManager.hpp"
#include "../header/FileCopying.hpp"
#include <filesystem>
#include <format>

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



void FileCopyManager::CopyFromFlash ()
{
    for (const auto & [name,paths] : paths_flash_.GetData())
    {
        std::filesystem::path folder_name_photograph = path_save_.GetTempWork()/  name;
        if ( !std::filesystem::exists(folder_name_photograph)) // папка не существует
        {
            try {
                std::filesystem::create_directories(folder_name_photograph);
            }
            catch (const std::filesystem::filesystem_error& e)
            {
                std::cerr << "Ошибка при создании директории фотографа: " << e.what() << '\n';
            }
        }

        for (const auto& path : paths)
        {
            if ( !std::filesystem::exists(path)) // папка не существует
            {
                continue;
            }
            // 
            FileCopying(FromSourceTag{}, path, ToDestinationTag{}, folder_name_photograph).Run();
            const auto path_for_server = path_save_.GetServer();
            if (!std::filesystem::exists(path_for_server))
            {
                try {
                    std::filesystem::create_directories(path_for_server);
                }
                catch (const std::filesystem::filesystem_error& e)
                {
                    std::cerr << "Ошибка при создании директории фотографа: " << e.what() << '\n';
                }
            }
            FileCopying(FromSourceTag{}, path, ToDestinationTag{}, path_for_server).Run();
        }
    }
}


#pragma once
#include "InputData.hpp"
#include <cstddef>
#include <filesystem>
#include <unordered_set>

/*archiv        - архив фото в неизменном виде*/
/*photographs   - пути к папкам фотографов. (автоматическое копирование)*/
/*Server        - Папка сервера на ней хранятся фото без QR code*/
/*workdir       - рабочая папка где выбираем фото. Туда же автоматически создаются фото с QRcode/*/
/*WorkQR        - локальная папка где создаются файлы с QRcode что бы потом можно было их отправить на сервер*/
/*                путь задается (имя сервера) + (путь к папке с датой ) + (имя файла) + (уникальное имя) + (расширение)*/
/*for_photoshop*/

         /*"Archive":   "./Resource/Archive",*/ //здесь все файлы за весь день в неизменном виде 
         /*"TempWork":  "./Resource/TempWork",*/ //Сюда копируются файлы до того как будут созданы коды После создания кодов они тут удаляются
         /*"Server":    "./Resource/Photoshop",*/
         /*"QRCode":    "./Resource/QRCode",*/
         /*"Photoshop": "./Resource/Server"*/

class FileCopyManager
{
private:
    PhotographerCollection paths_flash_; // пути откуда копируем
    WorkDirectoryManager   path_save_;   // пути куда копируем

    std::unordered_map<std::string, std::filesystem::path> name_dates_infolders_;

static auto GenerateRandomWord(size_t length) -> std::string;
/*static auto GenerateRandomFileName(const std::filesystem::path & path, const std::string &name, size_t new_name_length) -> std::filesystem::path;*/
static void MakeDirectoryIfNotExists(const std::filesystem::path & path_to_folder);
static auto IsDirectoryEmpty(const std::filesystem::path& dir) -> bool;

auto MakeDataDirectoryIfNotExists(const std::filesystem::path & path_to_folder) -> std::filesystem::path;
void ScanDataDirectory (); // заполняем name_dates_infolders_ для ункальности папок

    void CopyFromFlash ();
    void Copy ();
    auto CreateUniqueFilename (const std::string&) -> std::string;
    void CopyToTempWork ();
    void GenerateQRCode ();
    void CopyToForPhotoshop ();
    void CopyToServer ();

    void StartCoping ();
public:
    FileCopyManager ( PhotographerCollection , WorkDirectoryManager );
};

#include "../header/Application.hpp"
#include "../header/InputData.hpp"
#include <algorithm>
#include <filesystem>
#include <memory>
#include <span>
#include <stdexcept>

using std::filesystem::directory_iterator;

Application::Application(int argc, char **argv) : threads_(1) {

  std::span<char *> args_span(argv, static_cast<size_t>(argc));
  std::vector<std::string> argv_;

  std::ranges::transform(args_span, back_inserter(argv_),
                         [](char *arg) { return std::string(arg); });

  if (argc < 2 && argv_.size() < 2) {
    // путь по умолчанию
    json_path_ = "../input.json";
  } else if (argc == 2 && argv_.size() == 2) {
    // только json
    json_path_ = argv_.at(1);
  } else if (argc == 3 && argv_.size() == 3) {
    // json и потоки
    json_path_ = argv_.at(1);
    threads_ = std::stoi(argv_.at(2));
  } else {
    throw std::runtime_error("укажи путь к файлу JSON и количество потоков\n");
  }

  app_data_ = std::make_shared<ApplicationData>(json_path_);
}

Application::
Application (const std::string & json_path) :threads_(1)
{
    app_data_  = std::make_shared<ApplicationData>(json_path);
}

void Application::
PrintUsage ( ){
    std::cout << '\n';
    std::cout << "Использование:\n";
    std::cout << "[a.out] [путь к json файлу] [количество потоков]" << '\n';
    std::cout << "Например: a.out ./input.json 5" << '\n';
    std::cout << '\n';
}


void Application::Run() {
	CopyPhotographerFlash (app_data_->GetPhotographer());
}


void Application::
CopyPhotographerFlash (const std::shared_ptr<PhotographerCollection> & flash_paths)
{
   /* РАЗИБИТЬ путь к файлу на элементы*/
   /* получить имя фотографа*/
   /* если есть файлы в тех путях создать папку с именем фотографа*/
   /* пройтись по всем папкам и фотографам. */
   /* не забыть удалить фото после проверки CRC и после копирования.*/
   /* Копирование должно происходит как только надзиратель сообщит что в папке появились файлы. */
   /* т.е. мне не надо сканировать тут все папки. надо что бы надзиратель передал папку где произошли изменения*/
   /**/
   /*копирование происходит так:*/
   /* сканируются папки на предмет новых фото*/
   /* Если нашлись. */
   /* определяем фотографа.*/
   /* копируем его фото в ПРОМЕЖУТОЧНУЮ папку с которой будут отправлены на сервер и на изготовление QR кода*/
   /* std::filesystem::exists(dir) || !std::filesystem::is_directory(dir) директория существует и это директория*/
   /* проверяем на то что скопированы без ошибок.*/
   /* проверяем что скопировались все файлы.*/
   /* Удаляем с флешки фото.*/
   /* -----------------------------------------------------*/
bool isDirectoryEmpty(const std::filesystem::path& dir) {
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        throw std::runtime_error("Указанный путь не существует или не является директорией");
    }
    return std::filesystem::begin(std::filesystem::directory_iterator(dir)) == std::filesystem::end(std::filesystem::directory_iterator());
}
-----------------------------------------------------

    for (const auto &[photograph, paths_to_flash]: flash_paths->GetData())
    {
        std::cout << photograph << '\n'; 
        for (const auto &path: paths_to_flash)
        {
            try {

                for (const auto &folder_name : directory_iterator(path)) {
                    std::cout << folder_name << '\n';
                }
            }
            catch (const std::exception&) {
                std::cout << "не могу найти путь" << '\n';
                std::cout << path << '\n';
            }
        }
    }

}

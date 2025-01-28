#include <gtest/gtest.h>
#include "../header/FileCopying.hpp"
#include <filesystem>
#include <fstream>
#include <boost/crc.hpp>

namespace fs = std::filesystem;



class FileCopyingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создаем временные директории
        fs::create_directories(source_dir);
        fs::create_directories(dest_dir);
    }

    void TearDown() override {
        // Удаляем временные файлы и директории
        fs::remove_all(source_dir);
        fs::remove_all(dest_dir);
    }

    const fs::path source_dir = "test_source";
    const fs::path dest_dir = "test_dest";
};



// Тест: Проверка валидации директорий
TEST_F(FileCopyingTest, ValidateDirectoriesThrowsOnInvalidSource) {
    EXPECT_THROW(
        FileCopying(FromSourceTag{}, "invalid_path",
                    ToDestinationTag{}, dest_dir),
        std::runtime_error
    );
}



// Тест: Проверка копирования файла
TEST_F(FileCopyingTest, CopyFileSuccessfully) {
    // Создаем тестовый файл в исходной директории
    const fs::path test_file = source_dir / "test.txt";
    std::ofstream(test_file) << "sample data";

    FileCopying file_copy(FromSourceTag{}, source_dir,
                   ToDestinationTag{}, dest_dir);
    file_copy.Run();

    // Проверяем, что файл скопирован
    EXPECT_TRUE(fs::exists(dest_dir / "test.txt"));
}



// Тест: Проверка генерации уникального имени
TEST_F(FileCopyingTest, GenerateUniqueFileName) {
    const fs::path test_file = source_dir / "test.txt";
    std::ofstream(test_file) << "data";

    // Создаем файл с тем же именем в целевой директории
    std::ofstream(dest_dir / "test.txt") << "old data";

    FileCopying file_copy(FromSourceTag{}, source_dir,
                   ToDestinationTag{}, dest_dir);
    file_copy.Run();

    // Ожидаем новое имя "test_1.txt"
    EXPECT_TRUE(fs::exists(dest_dir / "test_1.txt"));
}



// Тест: Проверка контрольной суммы
TEST_F(FileCopyingTest, CalculateCRCCorrectness) {
    const fs::path test_file = source_dir / "test.bin";

    const std::string data = "123456789";
    {
        std::ofstream ofs(test_file, std::ios::binary);
        ofs << data;
    } // закрываем файл гарантируя запись данных

    FileCopying file_copy(FromSourceTag{}, source_dir,
                   ToDestinationTag{}, dest_dir);

    uint32_t crc = FileCopying::CalculateCRC(test_file);

    boost::crc_32_type expected_crc;
    expected_crc.process_bytes(data.data(), data.size());
    EXPECT_EQ(crc, expected_crc.checksum());
}



// Проверка невалидной целевой директории
TEST_F(FileCopyingTest, ValidateDirectoriesThrowsOnInvalidDest) {
    EXPECT_THROW(
        FileCopying(FromSourceTag{}, source_dir,
                    ToDestinationTag{}, "invalid_dest"),
        std::runtime_error
    );
}



// Пропуск файла с совпадающей CRC
TEST_F(FileCopyingTest, SkipExistingFileWithSameCRC) {
    const fs::path source_file = source_dir / "test.txt";
    const fs::path dest_file = dest_dir / "test.txt";
    std::ofstream(source_file) << "same data";
    std::ofstream(dest_file) << "same data";

    FileCopying file_copy(FromSourceTag{}, source_dir, ToDestinationTag{}, dest_dir);
    file_copy.Run();

    EXPECT_TRUE(fs::exists(dest_file));
    EXPECT_FALSE(fs::exists(dest_dir / "test_1.txt"));
}



// Генерация уникального имени при множественных конфликтах
TEST_F(FileCopyingTest, GenerateUniqueFileNameWithMultipleConflicts) {
    const fs::path test_file = source_dir / "test.txt";
    std::ofstream(test_file) << "data";

    std::ofstream(dest_dir / "test.txt") << "old";
    std::ofstream(dest_dir / "test_1.txt") << "old1";
    std::ofstream(dest_dir / "test_2.txt") << "old2";

    FileCopying file_copy(FromSourceTag{}, source_dir, ToDestinationTag{}, dest_dir);
    file_copy.Run();

    EXPECT_TRUE(fs::exists(dest_dir / "test_3.txt"));
}



// Обработка ошибок при вычислении CRC
TEST_F(FileCopyingTest, CalculateCRCThrowsOnUnreadableFile) {
    const fs::path test_file = source_dir / "unreadable.txt";
    std::ofstream(test_file) << "data";
    fs::permissions(test_file, fs::perms::none);

    EXPECT_THROW(FileCopying::CalculateCRC(test_file), std::runtime_error);

    fs::permissions(test_file, fs::perms::all);
}



// Копирование с повторными попытками (CRC не совпадает):
/*TEST_F(FileCopyingTest, CopyWithRetriesFailsOnCRC) {*/
/*    const fs::path source_file = source_dir / "source.txt";*/
/*    std::ofstream(source_file) << "source data";*/
/**/
/*    // Подмена VerifyChecksum для эмуляции ошибки CRC*/
/*    class TestFileCopying : public FileCopying {*/
/*    public:*/
/*        using FileCopying::FileCopying;*/
//        static auto VerifyChecksum(uint32_t /*unused*/, const fs::path& /*unused*/)  -> bool { return false; }
/**/
/*    };*/
/**/
/*    TestFileCopying file_copy(FromSourceTag{}, source_dir, ToDestinationTag{}, dest_dir);*/
/*    EXPECT_THROW(file_copy.Run(), std::runtime_error);*/
/*    EXPECT_FALSE(fs::exists(dest_dir / "source.txt"));*/
/*}*/



// Отсутствие прав на запись в целевую директорию
/*TEST_F(FileCopyingTest, CopyFailsDueToPermissionDenied) {*/
/*    fs::permissions(dest_dir, fs::perms::all & ~fs::perms::others_write);*/
/*    const fs::path source_file = source_dir / "test.txt";*/
/*    std::ofstream(source_file) << "data";*/
/**/
/*    FileCopying file_copy(FromSourceTag{}, source_dir, ToDestinationTag{}, dest_dir);*/
/*    EXPECT_THROW(file_copy.Run(), std::runtime_error);*/
/**/
/*    fs::permissions(dest_dir, fs::perms::all);*/
/*}*/
/**/


// Обработка исключений в ProcessSingleFile:
TEST_F(FileCopyingTest, ProcessSingleFileHandlesExceptions) {
    const fs::path source_file = source_dir / "unreadable.txt";
    std::ofstream(source_file) << "data";
    fs::permissions(source_file, fs::perms::none);

    testing::internal::CaptureStderr();
    FileCopying file_copy(FromSourceTag{}, source_dir, ToDestinationTag{}, dest_dir);
    file_copy.Run();
    std::string output = testing::internal::GetCapturedStderr();

    EXPECT_NE(output.find("Обработки файла"), std::string::npos);
    fs::permissions(source_file, fs::perms::all);
}



// Обработка файлов с особыми символами
TEST_F(FileCopyingTest, HandlesFilesWithSpecialCharacters) {
    const std::string filename = "file with spaces and !@#$%^&().txt";
    const fs::path source_file = source_dir / filename;
    std::ofstream(source_file) << "data";

    FileCopying file_copy(FromSourceTag{}, source_dir, ToDestinationTag{}, dest_dir);
    file_copy.Run();

    EXPECT_TRUE(fs::exists(dest_dir / filename));
}



// Кэширование CRC с ошибками чтения:
TEST_F(FileCopyingTest, BuildCRCCacheHandlesUnreadableFiles) {
    const fs::path dest_file = dest_dir / "unreadable.txt";
    std::ofstream(dest_file) << "data";
    fs::permissions(dest_file, fs::perms::none);

    testing::internal::CaptureStderr();
    FileCopying file_copy(FromSourceTag{}, source_dir, ToDestinationTag{}, dest_dir);
    file_copy.Run();
    std::string output = testing::internal::GetCapturedStderr();

    EXPECT_NE(output.find("Warning: Не могу прочитать файл"), std::string::npos);
    fs::permissions(dest_file, fs::perms::all);
}



// Копирование файла с тем же именем, но другой CRC
TEST_F(FileCopyingTest, CopyFileWithSameNameButDifferentCRC) {
    const fs::path source_file = source_dir / "test.txt";
    std::ofstream(source_file) << "new data";
    std::ofstream(dest_dir / "test.txt") << "old data";

    FileCopying file_copy(FromSourceTag{}, source_dir, ToDestinationTag{}, dest_dir);
    file_copy.Run();

    EXPECT_TRUE(fs::exists(dest_dir / "test_1.txt"));
    EXPECT_TRUE(fs::exists(dest_dir / "test.txt"));
}

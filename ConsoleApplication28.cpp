#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <map>
#include <set>s


namespace fs = std::filesystem;

struct Directory {
    fs::path path;
    uintmax_t size;
    std::time_t last_write_time;
    bool is_directory;

    Directory(const fs::path& p) : path(p) {
        if (fs::exists(p)) {
            size = fs::is_directory(p) ? 0 : fs::file_size(p);
            last_write_time = fs::last_write_time(p).time_since_epoch().count();
            is_directory = fs::is_directory(p);
        }
    }
    bool operator==(const Directory& other) const {
        return path == other.path &&
            size == other.size &&
            last_write_time == other.last_write_time &&
            is_directory == other.is_directory;
    }
};

// сравнение директорий
class DirectoryComparator {
private:
    fs::path dir1;
    fs::path dir2;

    // сбор информации о файлах с помощью рекурсии
    void collectFilesRecursive(const fs::path& directory,
        std::map<fs::path, Directory>& fileMap,
        const fs::path& basePath = "") {
        try {
            for (const auto& entry : fs::recursive_directory_iterator(directory)) {
                try {
                    fs::path relativePath = basePath.empty() ?
                        entry.path().filename() :
                        basePath / entry.path().filename();

                    fileMap[relativePath] = Directory(entry.path());

                    if (entry.is_directory()) {
                        collectFilesRecursive(entry.path(), fileMap, relativePath);
                    }
                }
                catch (const fs::filesystem_error& e) {
                    std::cerr << "Ошибка доступа к: " << entry.path() << " - " << e.what() << std::endl;
                }
            }
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "Ошибка обхода директории: " << directory << " - " << e.what() << std::endl;
        }
    }

public:
    DirectoryComparator(const std::string& path1, const std::string& path2)
        : dir1(path1), dir2(path2) {}

    // Основной метод сравнения
    void compare() {
        // Шаг 2: Проверка существования и доступности каталогов
        if (!fs::exists(dir1) || !fs::exists(dir2)) {
            std::cerr << "Одна или обе директории не существуют!" << std::endl;
            return;
        }

        if (!fs::is_directory(dir1) || !fs::is_directory(dir2)) {
            std::cerr << "Один или оба пути не являются директориями!" << std::endl;
            return;
        }

        std::cout << "Сравнение директорий:\n";
        std::cout << "Директория 1: " << dir1 << "\n";
        std::cout << "Директория 2: " << dir2 << "\n\n";

        // Шаг 3: Рекурсивный обход и сбор информации
        std::map<fs::path, Directory> files1, files2;
        collectFilesRecursive(dir1, files1);
        collectFilesRecursive(dir2, files2);

        // Шаг 4: Сравнение структур
        std::set<fs::path> allPaths;
        for (const auto& pair : files1) allPaths.insert(pair.first);
        for (const auto& pair : files2) allPaths.insert(pair.first);

        std::vector<fs::path> onlyInDir1;
        std::vector<fs::path> onlyInDir2;
        std::vector<std::pair<fs::path, std::string>> differentFiles;

        for (const auto& path : allPaths) {
            bool inDir1 = files1.find(path) != files1.end();
            bool inDir2 = files2.find(path) != files2.end();

            if (inDir1 && !inDir2) {
                onlyInDir1.push_back(path);
            }
            else if (!inDir1 && inDir2) {
                onlyInDir2.push_back(path);
            }
            else if (inDir1 && inDir2) {
                const Directory& info1 = files1[path];
                const Directory& info2 = files2[path];

                if (!(info1 == info2)) {
                    std::string differences;
                    if (info1.size != info2.size) {
                        differences += "размер: " + std::to_string(info1.size) +
                            " vs " + std::to_string(info2.size) + "; ";
                    }
                    if (info1.last_write_time != info2.last_write_time) {
                        differences += "время изменения; ";
                    }
                    if (info1.is_directory != info2.is_directory) {
                        differences += "тип (файл/директория); ";
                    }
                    differentFiles.emplace_back(path, differences);
                }
            }
        }

        // Шаг 5: Формирование отчета
        std::cout << "=== ОТЧЕТ О РАЗЛИЧИЯХ ===\n\n";

        // Файлы только в первой директории
        if (!onlyInDir1.empty()) {
            std::cout << "Файлы/директории только в " << dir1 << ":\n";
            for (const auto& path : onlyInDir1) {
                std::cout << "  " << path << "\n";
            }
            std::cout << "\n";
        }

        // Файлы только во второй директории
        if (!onlyInDir2.empty()) {
            std::cout << "Файлы/директории только в " << dir2 << ":\n";
            for (const auto& path : onlyInDir2) {
                std::cout << "  " << path << "\n";
            }
            std::cout << "\n";
        }

        // Файлы с различиями
        if (!differentFiles.empty()) {
            std::cout << "Файлы с различиями:\n";
            for (const auto& [path, diff] : differentFiles) {
                std::cout << "  " << path << " - различия: " << diff << "\n";
            }
            std::cout << "\n";
        }

        std::cout << "=== СВОДНАЯ СТАТИСТИКА ===\n";
        std::cout << "Всего файлов в директории 1: " << files1.size() << "\n";
        std::cout << "Всего файлов в директории 2: " << files2.size() << "\n";
        std::cout << "Файлов только в директории 1: " << onlyInDir1.size() << "\n";
        std::cout << "Файлов только в директории 2: " << onlyInDir2.size() << "\n";
        std::cout << "Файлов с различиями: " << differentFiles.size() << "\n";
        std::cout << "Идентичных файлов: " << (files1.size() - onlyInDir1.size() - differentFiles.size()) << "\n";
    }
};  

int main()
{
    setlocale(LC_ALL, "ru");

    // Шаг 1: Получение путей от пользователя
    std::string path1, path2;

    std::cout << "Программа сравнения двух директорий\n";
    std::cout << "Введите путь к первой директории: ";
    std::getline(std::cin, path1);

    std::cout << "Введите путь ко второй директории: ";
    std::getline(std::cin, path2);

    try {
        DirectoryComparator comparator(path1, path2);
        comparator.compare();
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
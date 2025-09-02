#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <windows.h>

void read_directory(const std::string& name, std::vector<std::string>& v)
{
    std::string pattern(name);
    pattern.append("\\*");
    WIN32_FIND_DATA data;
    HANDLE hFind;
    if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
        do {
            v.push_back(data.cFileName);
        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
    }
}

struct Directory {
    std::string path_;
    bool is_directory;

    Directory(std::string path) : path_(path), is_directory(){}
};

int main()
{
    std::vector<std::string> v;
    read_directory(".", v);
    std::copy(v.begin(), v.end(),
        std::ostream_iterator<std::string>(std::cout, "\n"));
    std::string path1;
    std::string path2;
    std::cout << "Enter path to first directory: ";
    std::cin >> path1;
    std::cout << "Enter path to second directory: ";
    std::cin >> path2;
}
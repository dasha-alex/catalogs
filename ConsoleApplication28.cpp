#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
//#include <boost/filesystem.hpp>

namespace fs = std::filesystem;
// namespace fsb = boost::filesystem;

// struct path_leaf_string {
//     std::string operator () (const fsb::directory_entry& entry)const {
//             return entry.path().leaf().string();
//     }
// };

// void read_directory(const std::string& path, std::vector<std::string>& v){
//     fsb::path p(path);
//     fsb::directory_iterator start(p);
//     fs::directory_iterator end;
// }
struct Directory {
    std::string path_;
    bool is_directory;

    Directory(std::string path) : path_(path), is_directory(){}
};

int main()
{
    std::vector<std::string> v;
    //read_directory(".",v);


    // std::string path1;
    // std::string path2;
    // std::cout << "Enter path to first directory: ";
    // std::cin >> path1;
    // std::cout << "Enter path to second directory: ";
    // std::cin >> path2;
    // fs::create_directory(path1);
    // fs::create_directory(path1);
}
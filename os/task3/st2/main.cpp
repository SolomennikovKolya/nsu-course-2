
#include <cstdlib>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

// читает имя программы из argv[0] и возвращает его в виде строки без учёта пути до программы
string getProgName(char *arg)
{
    std::filesystem::path path = arg;
    return path.filename();
}

// существует ли файл по указанному пути
int fileExists(string fileName)
{
    std::filesystem::path filePath = fileName;
    return filesystem::exists(filePath);
}

// a. создать каталог, указанный в аргументе
int createDir(int argc, char **argv)
{
    if (fileExists(string(argv[1])))
    {
        cerr << "Directory is already exists\n";
        return 1;
    }
    return mkdir(argv[1], 0777);
}

// b. вывести содержимое каталога, указанного в аргументе
int printDirContent(int argc, char **argv)
{
    if (!fileExists(string(argv[1])))
    {
        cerr << "Directory does not exist\n";
        return 1;
    }
    std::filesystem::path dirPath = argv[1]; // Текущий рабочий каталог
    try
    {
        // Итерация по содержимому каталога
        for (const auto &entry : filesystem::directory_iterator(dirPath))
        {
            std::filesystem::path entryPath = entry.path();
            cout << string(entryPath.filename()) << "\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to list directory: " << e.what() << "\n";
        return 1;
    }
    return 0;
}

// c. удалить каталог, указанный в аргументе
int removeDir(int argc, char **argv)
{
    if (!fileExists(string(argv[1])))
        return 0;
    return system(("rmdir " + string(argv[1])).c_str());
}

// d. создать файл, указанный в аргументе
int createFile(int argc, char **argv)
{
    if (fileExists(string(argv[1])))
    {
        cerr << "File is already exists\n";
        return 1;
    }
    ofstream file(argv[1]);
    if (file.is_open())
    {
        file.close();
        return 0;
    }
    else
        return 1;
}

// e. вывести содержимое файла, указанного в аргументе;
int printFileContent(int argc, char **argv)
{
    std::ifstream file(argv[1]);
    if (file.is_open())
    {
        std::string line;
        while (getline(file, line))
            cout << line << "\n";
        file.close();
        return 0;
    }
    else
    {
        cerr << "File does not exist\n";
        return 1;
    }
}

// f. удалить файл, указанный в аргументе;
int removeFile(int argc, char **argv)
{
    return std::remove(argv[1]);
    // filesystem::path filePath = fileName;
    // return !(filesystem::remove(filePath));
}

// g. создать символьную ссылку на файл, указанный в аргументе;
int createSoftLink(int argc, char **argv)
{
    return symlink(argv[1], argv[2]);
}

// h. вывести содержимое символьной ссылки, указанный в аргументе;
int printSoftLink(int argc, char **argv)
{
    char targetPath[1024]; // Буфер для хранения пути к файлу
    ssize_t len = readlink(argv[1], targetPath, sizeof(targetPath) - 1);
    if (len != -1)
    {
        targetPath[len] = '\0'; // Добавляем завершающий нулевой символ
        cout << targetPath << "\n";
        return 0;
    }
    else
    {
        std::cerr << "Failed to read symbolic link.\n";
        return 1;
    }
}

// i. вывести содержимое файла, на который указывает символьная ссылка, указанная в аргументе;
int printFileContentBySoftLink(int argc, char **argv)
{
    return printFileContent(argc, argv);
}

// j. удалить символьную ссылку на файл, указанный в аргументе;
int removeSoftLink(int argc, char **argv)
{
    return unlink(argv[1]);
}

// k. создать жесткую ссылку на файл, указанный в аргументе;
int createHardLink(int argc, char **argv)
{
    if (fileExists(string(argv[2])))
    {
        cerr << "File " << argv[2] << " is already exists\n";
        return 1;
    }
    string command = "ln " + string(argv[1]) + " " + string(argv[2]);
    return system(command.c_str());
}

// l. удалить жесткую ссылку на файл, указанный в аргументе;
int removeHardLink(int argc, char **argv)
{
    return unlink(argv[1]);
}

// m. вывести права доступа к файлу, указанному в аргументе и количество жестких ссылок на него;
int printPermissions(int argc, char **argv)
{
    struct stat fileInfo;
    if (stat(argv[1], &fileInfo))
    {
        std::cerr << "Failed to get file information\n";
        return 1;
    }
    mode_t perm = fileInfo.st_mode;
    std::string permissions;
    permissions += (perm & S_IRUSR) ? 'r' : '-';
    permissions += (perm & S_IWUSR) ? 'w' : '-';
    permissions += (perm & S_IXUSR) ? 'x' : '-';
    permissions += (perm & S_IRGRP) ? 'r' : '-';
    permissions += (perm & S_IWGRP) ? 'w' : '-';
    permissions += (perm & S_IXGRP) ? 'x' : '-';
    permissions += (perm & S_IROTH) ? 'r' : '-';
    permissions += (perm & S_IWOTH) ? 'w' : '-';
    permissions += (perm & S_IXOTH) ? 'x' : '-';
    std::cout << permissions << " " << fileInfo.st_nlink << "\n";
    return 0;
}

int readOctalNum(int x)
{
    int ans = 0;
    int mask = 1;
    while (x > 0)
    {
        ans += (x % 10) * mask;
        x /= 10;
        mask *= 8;
    }
    return ans;
}

// n. изменить права доступа к файлу, указанному в аргументе.
int changePermissions(int argc, char **argv)
{
    int permNum;
    try
    {
        permNum = readOctalNum(stoi(argv[2]));
    }
    catch (const exception &e)
    {
        std::cerr << "Wrong permissions format\n";
        return 1;
    }
    mode_t perm = permNum;
    return chmod(argv[1], perm);
}

// Информация о функции и сама функция
struct funcData
{
    string name;
    int (*func)(int argc, char **argv);
    int argsNum;
};

int main(int argc, char **argv)
{
    string progName = getProgName(argv[0]);
    // cout << "\"" << progName << "\"\n";

    funcData funcs[] = {
        {"createDir", createDir, 1},
        {"printDirContent", printDirContent, 1},
        {"removeDir", removeDir, 1},
        {"createFile", createFile, 1},
        {"printFileContent", printFileContent, 1},
        {"removeFile", removeFile, 1},
        {"createSoftLink", createSoftLink, 2},
        {"printSoftLink", printSoftLink, 1},
        {"printFileContentBySoftLink", printFileContentBySoftLink, 1},
        {"removeSoftLink", removeSoftLink, 1},
        {"createHardLink", createHardLink, 2},
        {"removeHardLink", removeHardLink, 1},
        {"printPermissions", printPermissions, 1},
        {"changePermissions", changePermissions, 2}};

    if (progName == "main.out")
    {
        // вывод подсказки (список доступных функций)
        if (argc == 2 && strcmp(argv[1], "-h") == 0)
        {
            for (auto i : funcs)
                cout << i.name << "\n";
        }

        // создание жёстких ссылок с именами доступных функций
        else if (argc == 2 && strcmp(argv[1], "-c") == 0)
        {
            for (auto i : funcs)
            {
                if (fileExists(i.name))
                {
                    if (!filesystem::remove(i.name))
                    {
                        cerr << "Failed to remove file " << i.name << "\n";
                        return 0;
                    }
                }
                string command = "ln " + progName + " " + i.name;
                system(command.c_str());
            }
        }
    }
    else
    {
        // вызов функции соответствующей названию программы
        for (funcData entry : funcs)
        {
            if (entry.name == progName)
            {
                if (argc - 1 != entry.argsNum)
                {
                    cerr << "Wrong number of arguments\n";
                    return 0;
                }
                int status = entry.func(argc, argv);
                if (status)
                {
                    cerr << "Error while executing " << progName << "\n";
                    return 0;
                }
            }
        }
    }

    return 0;
}

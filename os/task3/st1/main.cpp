
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <stack>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

class myException : public std::exception
{
private:
    const char *msg_;

public:
    explicit myException(const char *msg) : msg_(msg){};
    const char *what() const noexcept override { return msg_; }
};

// Является ли файл папкой
bool isDirectory(const char *path)
{
    struct stat fileInfo; // Информация о файле
    if (stat(path, &fileInfo) != 0)
        throw myException("fileInfo error");
    return S_ISDIR(fileInfo.st_mode);
}

// Записывает в originalPath корректный изначальный путь без '/' вконце
void getOriginalPath(const char *const inputPath, std::string &originalPath)
{
    if (access(inputPath, F_OK) != 0)
        throw myException("cannot access input path");

    if (!isDirectory(inputPath))
        throw myException("input path is not a directory");

    int inputPathLen = strlen(inputPath);
    if (inputPath[inputPathLen - 1] == '/')
        inputPathLen--;

    for (int i = 0; i < inputPathLen; ++i)
        originalPath += inputPath[i];

    if (originalPath == ".")
        throw myException("input path should not be the current directory");
}

// Переворачивает строку originalStr и записывает в reversedStr
void getReversedStr(const std::string &originalStr, std::string &reversedStr)
{
    reversedStr = "";
    for (auto c : originalStr)
        reversedStr = c + reversedStr;
}

// Записывает в reversedPath путь originalPath, но с перевёрнутым названием последней директории
void getReversedPath(const std::string &originalPath, std::string &reversedPath)
{
    const int originalPathLen = originalPath.length();

    int lastSlashId = originalPathLen - 1;
    for (; lastSlashId >= 0; --lastSlashId)
    {
        if (originalPath[lastSlashId] == '/')
            break;
    }

    reversedPath = "";
    getReversedStr(originalPath.substr(lastSlashId + 1, originalPath.length() - lastSlashId - 1), reversedPath);
    reversedPath = originalPath.substr(0, lastSlashId + 1) + reversedPath;
    // for (int i = 0; i <= lastSlashId; ++i)
    //     reversedPath += originalPath[i];
    // for (int i = originalPathLen - 1; i > lastSlashId; --i)
    //     reversedPath += originalPath[i];
}

// Создаёт папку по пути path = <существующий путь>/<название новой директории>
void makeDir(const std::string &path)
{
    if (mkdir(path.c_str(), 0777) != 0)
    {
        if (errno == EEXIST)
            std::cerr << "warning: directory is already exists\n";
        // throw myException("directory is already exists");
        else
            throw myException("mkdir error\n");
    }
}

// Является ли файл регулярным
bool isRegularFile(const std::string &path)
{
    struct stat fileInfo;
    if (stat(path.c_str(), &fileInfo) != 0)
        throw myException("fileInfo error");
    return S_ISREG(fileInfo.st_mode);
}

// Получить все имена регулярных файлов в директории path (имена считаются без учёта пути до них, то есть считаются имена только самих файлов)
void getRegularFiles(const std::string &path, std::vector<std::string> &regularFiles)
{
    DIR *directory = opendir(path.c_str());
    if (directory == nullptr)
        throw myException("error opening the catalog");

    dirent *entry;
    while ((entry = readdir(directory)) != nullptr)
    {
        std::string direntPath(path);
        direntPath += "/";
        direntPath += entry->d_name;
        // std::cout << direntPath << "\n";
        if (isRegularFile(direntPath))
            regularFiles.push_back(entry->d_name);
    }
    closedir(directory);
}

// Получить права доступа
mode_t getPermissions(const std::string file)
{
    struct stat st;
    if (stat(file.c_str(), &st) != 0)
        throw myException("cannot get permissions");
    return st.st_mode;

    // mode_t perm = st.st_mode;
    // std::string permissions;
    // permissions += (perm & S_IRUSR) ? 'r' : '-';
    // permissions += (perm & S_IWUSR) ? 'w' : '-';
    // permissions += (perm & S_IXUSR) ? 'x' : '-';
    // permissions += (perm & S_IRGRP) ? 'r' : '-';
    // permissions += (perm & S_IWGRP) ? 'w' : '-';
    // permissions += (perm & S_IXGRP) ? 'x' : '-';
    // permissions += (perm & S_IROTH) ? 'r' : '-';
    // permissions += (perm & S_IWOTH) ? 'w' : '-';
    // permissions += (perm & S_IXOTH) ? 'x' : '-';
    // std::cout << permissions << "\n";
}

// Копирует файлы с именами из regularFiles из директории originalDir в директорию reversedPath, при этом переворачивает их имена и содержимое (побайтово)
void reverseReversedFiles(const std::string &originalPath, const std::string &destinationPath, const std::vector<std::string> &regularFiles)
{
    std::stack<char> bytes;

    for (std::string filename : regularFiles)
    {
        // Путь до оригинального файла
        std::string pathToOriginalFile(originalPath);
        pathToOriginalFile += "/";
        pathToOriginalFile += filename;

        // Открытие оригинального файла
        std::ifstream inputFile(pathToOriginalFile, std::ios::binary);
        if (!inputFile.is_open())
            throw myException("cannot open file");

        // Чтение прав доступа
        mode_t permissions = getPermissions(pathToOriginalFile);

        // Путь до перевёрнутого файла
        std::string reversedFilename = "";
        getReversedStr(filename, reversedFilename);
        std::string pathToReversedFile(destinationPath);
        pathToReversedFile += "/";
        pathToReversedFile += reversedFilename;

        // Открытие перевёрнутого файла
        std::ofstream outputFile(pathToReversedFile, std::ios::binary);
        if (!outputFile.is_open())
        {
            inputFile.close();
            throw myException("cannot open file");
        }

        // Изменить права доступа
        if (chmod(pathToReversedFile.c_str(), permissions) == -1)
            throw myException("chmod error");

        char byte;
        while (inputFile.get(byte))
            bytes.push(byte);
        while (!bytes.empty())
        {
            char topByte = bytes.top();
            outputFile.put(topByte);
            bytes.pop();
        }

        inputFile.close();
        outputFile.close();
    }
}

// Получить все имена каталогов в директории path (имена считаются без учёта пути до них, то есть считаются имена только самих директорий)
void getDirectories(const std::string &path, std::vector<std::string> &regularFiles)
{
    DIR *directory = opendir(path.c_str());
    if (directory == nullptr)
        throw myException("error opening the catalog");

    dirent *entry;
    while ((entry = readdir(directory)) != nullptr)
    {
        std::string direntPath(path);
        direntPath += "/";
        direntPath += entry->d_name;
        // std::cout << direntPath << "\n";
        if (isDirectory(direntPath.c_str()))
            regularFiles.push_back(entry->d_name);
    }
    closedir(directory);
}

// Переворачивает всё в папке originalPath (включая директории). Записывает перевёрнутые файлы и директории в destinationPath
void reverse(const std::string &originalPath, const std::string &destinationPath)
{
    // Переворот регулярных файлов
    std::vector<std::string> regularFiles;
    getRegularFiles(originalPath, regularFiles);
    reverseReversedFiles(originalPath, destinationPath, regularFiles);

    // Переворот директорий
    std::vector<std::string> directories;
    getDirectories(originalPath, directories);
    for (auto directoryName : directories)
    {
        if (directoryName == "." || directoryName == "..")
            continue;

        std::string reversedDirectoryName = "";
        getReversedStr(directoryName, reversedDirectoryName);
        reversedDirectoryName = destinationPath + "/" + reversedDirectoryName;
        // std::cout << "reversedDirectoryName: " << reversedDirectoryName << "\n";

        // Чтение прав доступа
        mode_t permissions = getPermissions(originalPath + "/" + directoryName);

        makeDir(reversedDirectoryName);

        // Изменить права доступа
        if (chmod(reversedDirectoryName.c_str(), permissions) == -1)
            throw myException("chmod error");

        reverse(originalPath + "/" + directoryName, reversedDirectoryName);
    }
}

int main(int argc, char **argv)
{
    try
    {
        if (argc < 2)
            throw myException("not enough arguments");

        std::string originalPathStart = "";
        getOriginalPath(argv[1], originalPathStart);

        std::string destinationPathStart = "";
        getReversedPath(originalPathStart, destinationPathStart);

        makeDir(destinationPathStart);

        reverse(originalPathStart, destinationPathStart);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
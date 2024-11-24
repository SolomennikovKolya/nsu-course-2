#include "libusb/libusb.h"
#include <iostream>
#include <stdio.h>
// #include <Windows.h>
// #include <fcntl.h>
// #include <io.h>
// #include <locale>

using namespace std;

void printdev(libusb_device *dev);

int main()
{
    // SetConsoleCP(1251);
    // SetConsoleOutputCP(1251);
    // cout << "Привет - Hello\n";
    // _setmode(_fileno(stdout), _O_U16TEXT);
    // _setmode(_fileno(stdin), _O_U16TEXT);
    // _setmode(_fileno(stderr), _O_U16TEXT);
    // wcout << L"Привет - Hello\n";
    // std::locale::global(std::locale("ru_RU.UTF-8"));
    // std::wcout.imbue(std::locale());
    // std::wcout << L"Привет, мир!" << std::endl;

    libusb_device **devs;       // указатель на указатель на устройство, используется для получения списка устройств
    libusb_context *ctx = NULL; // контекст сессии libusb
    int r;                      // для возвращаемых значений
    ssize_t cnt;                // число найденных USB-устройств

    r = libusb_init(&ctx); // инициализировать библиотеку libusb, открыть сессию работы с libusb
    if (r < 0)
    {
        fprintf(stderr, "Error: initialization failed, code: %d.\n", r); // Ошибка: инициализация не выполнена
        return 1;
    }

    libusb_set_debug(ctx, 3);                 // задать уровень подробности отладочных сообщений
    cnt = libusb_get_device_list(ctx, &devs); // получить список всех найденных USB- устройств
    if (cnt < 0)
    {
        fprintf(stderr, "Error: the list of USB devices was not received, code: %d.\n", r); // Ошибка: список USB устройств не получен
        return 1;
    }

    printf("devices found: %d\n", (int)cnt); // найдено устройств
    printf("===========================================================\n");
    printf("* number of possible configurations\n");        // количество возможных конфигураций (Конфигурации – это набор настроек, которые определяют поведение или внешний вид системы, программы или устройства)
    printf("| * device class\n");                           // класс устройства
    printf("| | * manufacturer's ID\n");                    // идентификатор производителя
    printf("| | | * device ID\n");                          // идентификатор устройства
    printf("| | | | * number of interfaces\n");             // количество интерфейсов
    printf("| | | | | * number of alternative settings\n"); // количество альтернативных настроек
    printf("| | | | | | * device class\n");                 // класс устройства
    printf("| | | | | | | * interface number\n");           // номер интерфейса
    printf("| | | | | | | | * number of endpoints\n");      // количество конечных точек
    printf("| | | | | | | | | * descriptor type\n");        // тип дескриптора
    printf("| | | | | | | | | | * endpoint address\n");     // адрес конечной точки
    printf("+--+--+----+----+---+--+--+--+--+--+----------------------\n");
    for (ssize_t i = 0; i < cnt; i++)
    {                      // цикл перебора всех устройств
        printdev(devs[i]); // печать параметров устройства
    }
    printf("===========================================================\n");

    libusb_free_device_list(devs, 1); // освободить память, выделенную функцией получения списка устройств
    libusb_exit(ctx);                 // завершить работу с библиотекой libusb, закрыть сессию работы с libusb
    return 0;
}

void printdev(libusb_device *dev)
{
    libusb_device_descriptor desc;    // дескриптор устройства
    libusb_config_descriptor *config; // дескриптор конфигурации объекта
    const libusb_interface *inter;
    const libusb_interface_descriptor *interdesc;
    const libusb_endpoint_descriptor *epdesc;
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0)
    {
        fprintf(stderr, "Ошибка: дескриптор устройства не получен, код: %d.\n", r);
        return;
    }

    libusb_get_config_descriptor(dev, 0, &config); // получить конфигурацию устройства
    printf("%.2d %.2d %.4d %.4d %.3d | | | | | |\n",
           (int)desc.bNumConfigurations,
           (int)desc.bDeviceClass,
           desc.idVendor,
           desc.idProduct,
           (int)config->bNumInterfaces);
    for (int i = 0; i < (int)config->bNumInterfaces; i++)
    {
        inter = &config->interface[i];
        printf("| | | | | %.2d %.2d | | | |\n",
               inter->num_altsetting,
               (int)desc.bDeviceClass);
        for (int j = 0; j < inter->num_altsetting; j++)
        {
            interdesc = &inter->altsetting[j];
            printf("| | | | | | | %.2d %.2d | |\n",
                   (int)interdesc->bInterfaceNumber,
                   (int)interdesc->bNumEndpoints);
            for (int k = 0; k < (int)interdesc->bNumEndpoints; k++)
            {
                epdesc = &interdesc->endpoint[k];
                printf("| | | | | | | | | %.2d %.9d\n",
                       (int)epdesc->bDescriptorType,
                       (int)(int)epdesc->bEndpointAddress);
            }
        }
    }

    libusb_device_handle *handle = nullptr;
    unsigned char description[256]; // Буфер для хранения описания
    libusb_open(dev, &handle);
    if (!handle)
    {
        printf("failed to open the device\n"); // не удалось открыть устройство
        libusb_close(handle);
        return;
    }
    int result = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, description, 256);
    if (result < 0)
        printf("iSerial is missing\n"); // отсутствует iSerial
    else
        printf("iSerial: %s\n", description); // СЕРИЙНЫЙ НОМЕР

    result = libusb_get_string_descriptor_ascii(handle, desc.iProduct, description, 256);
    if (result < 0)
        printf("missing iProduct\n"); // отсутствует iProduct
    else
        printf("iProduct: %s\n", description);

    result = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, description, 256);
    if (result < 0)
        printf("manufacturer is missing\n"); // отсутствует iManufacturer
    else
        printf("manufacturer: %s\n", description);
    libusb_free_config_descriptor(config);
    libusb_close(handle);
}

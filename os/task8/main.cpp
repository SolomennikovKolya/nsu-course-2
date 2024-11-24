
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

const char *filename = "privateFile.txt";
const int bufferSize = 100;
const int ownerID = 600;

int main()
{
    // if (setuid(ownerID))
    // {
    //     perror("setuid error");
    //     return 1;
    // }

    printf("uid  = %d\n", getuid());
    printf("euid = %d\n", geteuid());

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("fopen error");
        return 1;
    }

    char buffer[bufferSize];
    const size_t readed = fread(buffer, sizeof(char), bufferSize, file);
    std::cout << buffer;

    if (fclose(file))
    {
        perror("fclose error");
        return 1;
    }

    return 0;
}

/*
$ cat /etc/passwd - посмотреть информацую о всех пользователях
$ cat /etc/group - посмотреть информацую о всех группах
$ stat file - посмотреть подробную информацию о файле

$ whoami - имя текущего пользователя
$ id - возвращает информацию о пользователе, от имени которого она запускается (uid, gid, groups)
$ sudo adduser newuser - добавить пользователя
$ sudo userdel newuser - удалить пользователя
$ sudo usermod -l new_name old_name - изменить информацую о пользователе (в данном случае меняется имя пользователя, причём название группы пользователя и домашняя директория остаются старыми)
$ su username - сменить пользователя ("-" = "-l" для полного переключения окружения; по умолчанию ussername это root)
$ sudo -u username command - запустить команду из под имени другого пользователя

$ groups - группы, в которые входит текущий пользователь
$ sudo groupadd mygroup - создать группу
$ sudo usermod -a -G projectgroup username - добавить пользователя в группу
$ sudo groupmod -g 600 group_name - изменить параметры группы (в данно случае gid)
$ sudo chown user:mygroup file.txt - для изменения владельца и группы файла
$ newgrp mygroup - изменить свою текущую группу (изменение группы процесса)

$ cat /proc/filesystems - список поддерживаемых типов файловых систем
$ mount - список смонтированных систем в данный момент
$ cat /etc/fstab - список статических файловых систем
$ findmnt - дерево смонтированных систем
$ lsblk - список блочных устройств
$ dd if=/dev/zero of=./noexec.img bs=1M count=100 - создание образа устройства
$ mkfs.ext4 ./noexec.img - создать файловую систему в этом образе файла
$ sudo mount -o loop,noexec ./noexec.img ./noexecFS - смонтировать файловую систему, найденную на устройстве, к каталогу
*/

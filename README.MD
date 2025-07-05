# MAC Address Finder / Поиск MAC-адреса

A simple C program that finds the MAC address of a remote host by / 
Простая программа на C для поиска MAC-адреса удаленного хоста:

    Sending an ICMP ping to the target IP address / 
    Отправляет ICMP ping на указанный IP-адрес

    Checking the local ARP cache for the corresponding MAC address / 
    Проверяет локальный ARP-кэш для поиска соответствующего MAC-адреса


## Requirements / Требования

    Linux system / Linux система

    GCC compiler / Компилятор GCC

    Root privileges (required for raw sockets) / 
    Права root (требуются для работы с raw-сокетами)


## Build and Run / Сборка и запуск


### Compile the program / Компиляция программы

```bash
make
```


### Run with sudo (requires root privileges) / 
### Запуск с sudo (требуются права root)

```
sudo ./mac <IP_ADDRESS>
```

Example / Пример:
```bash
sudo ./mac 192.168.1.1
```

### How It Works / Как это работает

    The program sends an ICMP echo request (ping) to the specified IP address / 
    Программа отправляет ICMP echo-запрос (ping) на указанный IP-адрес

    If it receives a reply, it checks the local ARP cache (/proc/net/arp) / 
    При получении ответа проверяет локальный ARP-кэш (/proc/net/arp)

    If found, it displays the MAC address associated with the IP / 
    При нахождении выводит MAC-адрес, соответствующий IP


### Clean / Очистка

```bash
make clean  # Remove built files / Удаление собранных файлов
```

### Code Formatting / Форматирование кода

```bash
make cl     # Format code with clang-format / Форматирование кода с помощью clang-format
```

### Notes / Примечания

    Requires root privileges to create raw sockets / 
    Требуются права root для создания raw-сокетов

    Will only work if the target responds to ICMP and is in the same network segment / 
    Будет работать только если цель отвечает на ICMP и находится в том же сегменте сети

    ARP cache entries may time out if the host hasn't been contacted recently / 
    Записи в ARP-кэше могут быть удалены по таймауту, если хост не использовался недавно
    

*О проекте*

Если вдруг кому-то это понадобится.
Проект реализует задачу передачи информации между сервером, клиентом и вычислительными машинами. Предусмотрена синхронизация времени между машинами.
Примерный алгоритм действий:
    - КОМ (управляющая вычислительная машина) черпает информацию из некоторого источника (возможно файлы конфигурации, возможно из какой-то базы данных, возможно получает из от других машин(в данном случае реализовано получение значений из БД)).
      Значения могут быть различной физической сути (скорость, вес, температура, тд.). Расширение программы в этом плане может быть реализована расширением структур хранения полученных данных)
    - Сервер включает прослушивание порта, получает информацию и анализирует каков был источник и какова цель этих данных (синхронизация времени, данные для записи в БД сервера либо запрос на получение данных от клиента).
    - Клиент запрашивает у сервера информацию о значениях с той или иной КОМ (о распределении КОМ далее ).
В данном случае КОМ существуют в двух группах, в каждой по две машины. КОМ начинает передавать значения из своей БД серверу в отдельном потоке и запускает еще один поток, для синхронизации времени между КОМ и сервером. В то же время вторая машина из группы начинает дублировать информацию на сервер для надежности.
Между клиентом и сервером при передаче тоже в отдельном потоке начинается синхронизация времени.
Задача параметров в приложениях (ip, port) происходит путем чтения из файлов .ini(вы можете добавить их самостоятельно в ту же папку что и приложения, либо в другую (поменяв пути в источниках). Файл конфигурации не предусмотрен, делаем руками.)

Пример conf.ini:
[MAIN]
ip = 127.0.0.1
port = 1114

Также нужно иметь базы данных у сервера и у КОМ (для каждой группы)*смотреть код*. Структуру данных БД смотреть в коде либо переделывать под себя.
Данный проект будет адаптирован под WinForms и будет иметь лучшую изменяемость ( надеюсь;) ).

*About*

 If suddenly someone needs it.
The project implements the task of transferring information between the server, client and computers. Synchronization of time between machines is provided.
Approximate algorithm of actions:
    - CC (control computer) obtains information from some source (possibly configuration files, possibly from some database, possibly from other machines (in this case, obtaining values ​​from the database is implemented)).
      The values ​​can be of different physical nature (speed, weight, temperature, etc.). Expansion of the program in this regard can be implemented by expanding the storage structures of the received data).
    - The server turns on listening on the port, receives information and analyzes what was the source and what is the purpose of this data (time synchronization, data to be written to the server database, or a request to receive data from the client).
    - The client asks the server for information about the values ​​with one or another COM (about the distribution of the COM further).
In this case, the CCs exist in two groups, each with two cars. The COM starts transferring values ​​from its database to the server in a separate thread and starts another thread to synchronize the time between the COM and the server. At the same time, the second machine from the group begins to duplicate information to the server for reliability.
Time synchronization also begins between the client and the server during transmission in a separate stream.
The task of parameters in applications (ip, port) is done by reading from .ini files (you can add them yourself to the same folder as applications, or to another (by changing the paths in the sources). The configuration file is not provided, we do it by hand.)

Conf.ini example:
[MAIN]
ip = 127.0.0.1
port = 1114

You also need to have databases at the server and at the COM (for each group) * see the code *. Look at the database data structure in the code or alter it for yourself.
This project will be adapted for WinForms and will have better mutability (hopefully;)).

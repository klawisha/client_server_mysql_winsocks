
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
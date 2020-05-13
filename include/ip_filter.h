#pragma once

namespace ipf
{
const char ipaddr_str_length = 15;
const char ipaddr_byte_length = 4;
const char ipaddr_str_minlength = 7;
const char field_delimiter = '\t';
const char ipaddr_delimiter = '.';
const int ipaddr_position = 0;      // position of ip address element in line starting from 0



/*
lines_in    - вектор со всеми входящими строками без изменений
original_order - номер строки в исходном потоке
v           - вектор, содержащий отдельные слова в текущей строке
vsub        - вектор, содержащий отдельные байты IP-адреса 
ipstr       - строка, содержащая только ip адрес входящего потока в исходном формате
skipped     - флаг true, если невозможно было распознать ip адрес в строке
bytes[]     - байты ip адреса
idfromipv4  - id числовой (int) для ip v4
*/
}

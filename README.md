Для управления удвоенной шириной используем команду
0xff 0x02 - обычная ширина
0xff 0x03 - удвоенная ширина

Для управления шириной пробела используем посылку
0xff 0x1W где W это кол-во пробелов (0-15)

Для управления шрифтом 
0xff 0x2W где W это номер шрифта

0b0000 001 X - двойная ширина
0b0001 XXXX  - ширина пробела
0b0010 XXXX  - номер шрифта


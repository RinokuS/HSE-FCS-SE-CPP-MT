# Соколовский Вацлав, БПИ191
## Вариант 1 (Задача о парикмахере)
Основная информация о работе представлена в [Пояснительной записке](https://github.com/RinokuS/HSE-FCS-SE-CPP-MT/tree/main/MicroProject_2/PZ.pdf)

В [данной папке](https://github.com/RinokuS/HSE-FCS-SE-CPP-MT/tree/main/MicroProject_2) представлено выполнение второго микропроекта на языке С++. Исходники лежат в [MicroProject_2/src/](https://github.com/RinokuS/HSE-FCS-SE-CPP-MT/tree/main/MicroProject_2/src), а скриншоты в [MicroProject_2/pictures/](https://github.com/RinokuS/HSE-FCS-SE-CPP-MT/tree/main/MicroProject_2/pictures)

Код запускался из консоли с использованием команд:

	clang++ -std=c++17 main.cpp -o main
    ./main

## Условие
В тихом городке есть парикмахерская. Салон 
парикмахерской мал, ходить там может только парикмахер и один 
посетитель. Парикмахер всю жизнь обслуживает посетителей. Когда в салоне 
никого нет, он спит в кресле. Когда посетитель приходит и видит спящего 
парикмахера, он будет его, садится в кресло и спит, пока парикмахер занят 
стрижкой. Если посетитель приходит, а парикмахер занят, то он встает в 
очередь и засыпает. После стрижки парикмахер сам провожает посетителя. 
Если есть ожидающие посетители, то парикмахер будит одного из них и ждет 
пока тот сядет в кресло парикмахера и начинает стрижку. Если никого нет, 
он снова садится в свое кресло и засыпает до прихода посетителя. Создать 
многопоточное приложение, моделирующее рабочий день парикмахерской.

## Код программы
[Исходный код](https://github.com/RinokuS/HSE-FCS-SE-CPP-MT/tree/main/MicroProject_2/src/main.cpp)<br>

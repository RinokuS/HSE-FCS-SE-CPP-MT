#include <iostream>
#include <vector>
#include <optional>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>

#include "profile.h"

using namespace std;

struct MyObject{
    double const cost;

    explicit MyObject(double c) : cost(c){}
};

mt19937 randomizer(chrono::high_resolution_clock::now().time_since_epoch().count());
mutex mut;
optional<MyObject> curr_object, counted_object; // используем optional дабы упростить проверку на наличие элемента внутри
vector<MyObject> trunk;
double num = 0;
long counter = 0;

void steal_from_warehouse(long count){
    while (count > 0) { // пока на складе есть элементы, Иванов вытаскивает их на улицу
        if (!curr_object.has_value()) {
            lock_guard<mutex> lock(mut); // лочим мьютексом объект, дабы избежать одновременного владения двумя потоками
            cout << "Иванов выносит со склада элемент военного инвентаря (№" <<
                 counter - count + 1 << ")...\n";
            // curr_object.emplace(MyObject(count)); // элементы с предсказуемой стоимостью для удобства тестов
            curr_object.emplace(randomizer() % 1000); // достаем элемент со "склада" (рандом стоимости)
            count--; // не забываем вести счетчик оставшихся на складе элементов
        }
    }
}

void load_into_a_truck(long count){
    while (count > 0) { // пока есть незагруженные элементы, Петров ждет и грузит их в грузовик
        if (counted_object.has_value()) { // кладем элемент в грузовик, если он уже посчитан Нечепорчуком
            lock_guard<mutex> lock(mut); // лочим мьютексом объект, дабы избежать одновременного владения двумя потоками
            cout << "Петров грузит элемент военного инвентаря в грузовик (№" <<
                 counter - count + 1 << ")...\n";
            trunk.push_back(counted_object.value()); // кладем элемент в "грузовик"
            counted_object.reset(); // расчищаем место под новый элемент
            count--;
        }
    }
}

void count_cost(long count){
    while (count > 0) { // пока есть непосчитанные элементы, Нечепорчук считает их цену
        if (curr_object.has_value() && !counted_object.has_value()) { // если в грузовике есть элементы, то можно начать подсчет
            lock_guard<mutex> lock(mut);
            num += curr_object.value().cost;
            counted_object.emplace(curr_object.value());
            curr_object.reset();
            cout << "Нечепорчук подсчитывает стоимтость (№" << counter - count + 1 << ")...\n";
            count--; // не забываем вести счетчик непосчитанных элементов
        }
    }
}

/*
 * Вариант 23
 * Первая военная задача. Темной-темной ночью прапорщики Иванов,
 * Петров и Нечепорчук занимаются хищением военного имущества со склада родной военной части.
 * Будучи умными людьми и отличниками боевой и строевой подготовки, прапорщики ввели разделение труда:
 * Иванов выносит имущество со склада, Петров грузит его в грузовик, а Нечепорчук подсчитывает рыночную
 * стоимость добычи. Требуется составить многопоточное приложение, моделирующее деятельность прапорщиков.
 * Прирешении использовать парадигму «производитель-потребитель».
 */
int main() {
    cout << "Введите количество инвентаря на складе (целое число): ";
    cin >> counter;

    {
        LOG_DURATION("Время работы трех прапорщиков")
        thread Ivanov(steal_from_warehouse, counter);
        thread Necheporchuk(count_cost, counter);
        thread Petrov(load_into_a_truck, counter);

        Ivanov.detach();
        Necheporchuk.detach();
        Petrov.join(); // дожидаемся окончания работы Петрова, ибо его действие - последнее
    }

    cout << "Конечная стоимость украденного: " << num << "$\n";
    return 0;
}

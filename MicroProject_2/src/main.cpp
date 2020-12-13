#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <condition_variable>
#include <vector>
#include <queue>

using namespace std;

class semaphore {
private:
    mutex mtx;
    condition_variable cv;
    int count;
public:
    explicit semaphore(int count_ = 0) : count(count_){}

    void notify() {
        unique_lock<mutex> lock(mtx);
        ++count;
        cv.notify_one();
    }

    void wait() {
        unique_lock<mutex> lock(mtx);
        while(count == 0) {
            cv.wait(lock);
        }

        --count;
    }
};

vector<thread> v; // вектор потоков для сохранения потоков (чтобы они не уничтожались после создания)
queue<thread::id> q; // очередь потоков (для соблюдения порядка в зале ожидания)
semaphore waiting_customers(0); // семафор для посетителей, ожидающих в очереди
semaphore barbers(0); // семафор для парикмахеров, в нашем случае он всегда один, но можно сделать и больше
mutex console_mutex; // мутекс для записи в консоль, ибо иначе вывод будет некрасивым
std::atomic_int waiting = 0, limit = 0, result = 0;
int chairs = 0;

void barber() {
    while (limit > 0 || waiting != 0) {
        barbers.notify(); // парикмахер готов принять клиента
        waiting_customers.wait(); // ждем пока семафор посетителей не объявит о добавлении нового посетителя
        {
            lock_guard<mutex> lock_console(console_mutex);
            cout << "Парикмахер просыпается\n";
        }
        waiting--; /* уменьшаем кол-во ожидающих клиентов */
        {
            lock_guard<mutex> lock_console(console_mutex);
            cout << "Парикмахер стрижет клиента\n";
        }
        this_thread::sleep_for(1s); // каждая стрижка длится 1 секунду (для удобства)
        {
            lock_guard<mutex> lock_console(console_mutex);
            cout << "Парикмахер засыпает\n";
        }
        result++;
    }
}

void customer() {
    if (waiting < chairs) /* есть место для ожидания */ {
        q.push(this_thread::get_id());
        waiting++; /* увеличиваем кол-во ожидающих клиентов */
        {
            lock_guard<mutex> lock_console(console_mutex);
            cout << "Клиент " << this_thread::get_id() << " проходит в зал ожидания." << endl;
        }
        while (this_thread::get_id() != q.front()); // проверка на очередь, чтобы к парикмахеру первым заходил не самый быстрый поток)
        barbers.wait(); /* если парикмахер занят, переходим в состояние ожидания, иначе – занимаем парикмахера*/
        waiting_customers.notify(); /* если парикмахер спит, это его разбудит */
        {
            lock_guard<mutex> lock_console(console_mutex);
            cout << "Клиент " << this_thread::get_id() << " проходит на стрижку.\n";
        }
        q.pop();
    } else {
        lock_guard<mutex> lock_console(console_mutex);
        cout << "Нет мест в зале ожидания. Поток " << this_thread::get_id() << " уходит домой" << endl;
        result++;
    }
}

/// Метод для проверки ввода
/// \param text текст запроса
/// \return корректное значение
int get_int_input(const string &text) {
    string str;
    int value = 0;

    while (value <= 0 || value > 10'000) { // проверка ввода
        cout << text;
        getline(cin, str);
        try {
            value = stoi(str);
        } catch (...) {
            cout << "Неверный ввод! Попробуйте еще раз.\n";
            value = 0;
        }
    }

    return value;
}

chrono::duration<double> rand_some_chrono_sec(double left, double right) {
    random_device rd; // источник энтропии
    mt19937 generator(rd()); // генератор чисел

    uniform_real_distribution<> dist(left, right); // равномерное распределение от left до right
    double k = dist(generator);

    return chrono::duration<double>(k);
}

int main() {
    chairs = get_int_input("Введине количество мест в зале ожидания "
                           "(целочисленное значение из диапазона (0; 10000]: ");
    limit = get_int_input("Введине количество людей в городе "
                          "(целочисленное значение из диапазона (0; 10000]: ");

    thread barb(barber);
    thread helper([] {
        while (limit-- != 0) {
            v.emplace_back(customer);
            this_thread::sleep_for(rand_some_chrono_sec(0.1, 0.8));
        }
    });
    barb.join();
    helper.join(); // джоиним все потоки с мейном, ибо они все что-то выводят, а терять строки в логе не хочется
    for (auto &thread : v)
        thread.join();

    cout << "Всего клиентов пришло за день: " << result << endl;

    return 0;
}

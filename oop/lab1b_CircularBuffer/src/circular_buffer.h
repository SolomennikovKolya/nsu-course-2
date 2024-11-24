#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <string>

template <typename valueType>
class CircularBuffer
{
private:
    valueType *buffer_; // Указывает на выделенную память
    int capacity_;      // Максимальное количество элементов, которые могут поместиться в буфер
    int size_;          // Количество элементов в буфере на данный момент
    int first_;         // Номер первого элемента в буфере
    int last_;          // Номер последнего элемента в буфере

public:
    CircularBuffer();                                    // Инициализация без входных параметров
    explicit CircularBuffer(int capacity);               // Инициализация буфера заданной ёмкости
    CircularBuffer(int capacity, const valueType &elem); // Инициализация буфера заданной ёмкости, заполненного элементами elem
    CircularBuffer(const CircularBuffer &cb);            // Инициализация буфера по примеру другого буфера
    ~CircularBuffer() noexcept;                          // Деструктор

    int size() const noexcept;     // Количество элементов, хранящихся в буфере
    int capacity() const noexcept; // Ёмкость буфера, т.е. максимальное количество элементов, которые могут в нём храниться
    bool empty() const noexcept;   // true, если size() == 0
    bool full() const noexcept;    // true, если size() == capacity()
    int reserve() const noexcept;  // Количество свободных ячеек в буфере

    valueType &operator[](int i);             // Доступ по индексу. Не проверяют правильность индекса
    const valueType &operator[](int i) const; // Доступ по индексу. Не проверяют правильность индекса
    valueType &at(int i);                     // Доступ по индексу. Методы бросают исключение в случае неверного индекса
    const valueType &at(int i) const;         // Доступ по индексу. Методы бросают исключение в случае неверного индекса

    valueType &front();             // Ссылка на первый элемент
    valueType &back();              // Ссылка на последний элемент
    const valueType &front() const; // Ссылка на первый элемент
    const valueType &back() const;  // Ссылка на последний элемент

    bool is_linearized() const noexcept; // Проверяет, является ли буфер линеаризованным
    valueType *linearize() noexcept;     // Линеаризация - сдвинуть кольцевой буфер так, что его первый элемент переместится в начало аллоцированной памяти. Возвращает указатель на первый элемент
    void rotate(int newBegin) noexcept;  // Сдвигает буфер так, что по нулевому индексу окажется элемент с индексом new_begin

    void set_capacity(int newCapacity);               // Изменить ёмкость буфера
    void resize(int new_size, const valueType &item); // Изменяет размер буфера. В случае расширения, новые элементы заполняются элементом item

    CircularBuffer &operator=(const CircularBuffer &cb); // Оператор присваивания
    void swap(CircularBuffer &cb);                       // Обменивает содержимое буфера с буфером cb

    void push_back(const valueType &item) noexcept;  // Добавляет элемент в конец буфера. Если текущий размер буфера равен его ёмкости, то переписывается первый элемент буфера (т.е., буфер закольцован)
    void push_front(const valueType &item) noexcept; // Добавляет новый элемент перед первым элементом буфера. Аналогично push_back, может переписать последний элемент буфера
    void pop_back() noexcept;                        // Удаляет последний элемент буфера.
    void pop_front() noexcept;                       // Удаляет первый элемент буфера.

    void insert(int pos, const valueType &item); // Вставляет элемент item по индексу pos. Ёмкость буфера остается неизменной.
    void erase(int first, int last) noexcept;    // Удаляет элементы из буфера в интервале [first, last).
    void clear() noexcept;                       // Очищает буфер.

    void print() const noexcept;               // Выводит на экран буфер (от первого элемента до последнего).
    void printBufferInMemory() const noexcept; // Выводит на экран буфер (в точности как он находится в памяти).

    class iterator
    {
    private:
        CircularBuffer &cb_; // Ссылка на буфер, на котором определён указатель
        int index_;          // Индекс элемента на который указывает итератор в буфере

    public:
        iterator(CircularBuffer &cb, int beginIndex) noexcept;
        valueType &operator*() const;
        iterator &operator=(const iterator &other) noexcept;
        iterator &operator+(int shift) noexcept;
        iterator &operator-(int shift) noexcept;
        iterator &operator+=(int shift) noexcept;
        iterator &operator-=(int shift) noexcept;
        iterator &operator++() noexcept;
        iterator operator++(int) noexcept;
        iterator &operator--() noexcept;
        iterator operator--(int) noexcept;
        bool operator==(const iterator &other) const noexcept;
        bool operator!=(const iterator &other) const noexcept;
    };

    iterator begin() const noexcept; // Итератор на первый элемент буфера
    iterator end() const noexcept;   // Итератор на следующий за последним элемент буфера

    class exception : public std::exception
    {
    private:
        const char *message_;

    public:
        explicit exception(const char *message) : message_(message) {}
        // exception(exception const&) noexcept = default;
        // exception& operator=(exception const&) noexcept = default;
        // ~exception() override = default;
        const char *what() const noexcept override { return message_; }
    };
};

template <typename valueType>
bool operator==(const CircularBuffer<valueType> &a, const CircularBuffer<valueType> &b); // Проверяет два буфера на равенство
template <typename valueType>
bool operator!=(const CircularBuffer<valueType> &a, const CircularBuffer<valueType> &b); // Проверяет два буфера на неравенство

#endif


#include "circular_buffer.h"
#include <iostream>
#include <string>
#include <vector>

static constexpr int DEFAULT_CAPACITY = 16;

template <typename valueType>
CircularBuffer<valueType>::CircularBuffer() : CircularBuffer(DEFAULT_CAPACITY){};

template <typename valueType>
CircularBuffer<valueType>::CircularBuffer(int capacity)
{
    if (capacity <= 0)
        throw CircularBuffer<valueType>::exception("Capacity of CircularBuffer must be greater than zero");
    else
        capacity_ = capacity;
    buffer_ = new valueType[capacity_];
    size_ = 0, first_ = 0, last_ = -1;
}

template <typename valueType>
CircularBuffer<valueType>::CircularBuffer(int capacity, const valueType &elem) : CircularBuffer(capacity)
{
    for (int i = 0; i < capacity_; i++)
        buffer_[i] = elem;
    size_ = capacity_, last_ = capacity_ - 1;
}

template <typename valueType>
CircularBuffer<valueType>::CircularBuffer(const CircularBuffer<valueType> &other) : CircularBuffer(other.capacity_)
{
    size_ = other.size_, first_ = 0, last_ = size_ - 1;
    for (int i = 0; i < size_; i++)
        buffer_[i] = other[i];
}

template <typename valueType>
CircularBuffer<valueType>::~CircularBuffer() noexcept
{
    delete[] buffer_;
}

template <typename valueType>
int CircularBuffer<valueType>::size() const noexcept
{
    return size_;
}

template <typename valueType>
int CircularBuffer<valueType>::capacity() const noexcept
{
    return capacity_;
}

template <typename valueType>
bool CircularBuffer<valueType>::empty() const noexcept
{
    return size_ == 0;
}

template <typename valueType>
bool CircularBuffer<valueType>::full() const noexcept
{
    return size_ == capacity_;
}

template <typename valueType>
int CircularBuffer<valueType>::reserve() const noexcept
{
    return capacity_ - size_;
}

static int mod(int index, int base)
{
    if (index >= 0)
        return index % base;
    return index % base + base;
}

template <typename valueType>
static void throwIfEmpty(const CircularBuffer<valueType> *cb)
{
    if (cb->empty())
        throw CircularBuffer<valueType>::exception("You cannot access the elements of an empty CircularBuffer");
}

template <typename valueType>
valueType &CircularBuffer<valueType>::operator[](int i)
{
    throwIfEmpty(this);
    return buffer_[mod(first_ + mod(i, size_), capacity_)];
}

template <typename valueType>
const valueType &CircularBuffer<valueType>::operator[](int i) const
{
    throwIfEmpty(this);
    return buffer_[mod(first_ + mod(i, size_), capacity_)];
}

template <typename valueType>
valueType &CircularBuffer<valueType>::at(int i)
{
    throwIfEmpty(this);
    if (i < 0 || i >= size_)
        throw CircularBuffer<valueType>::exception("Index is either less than zero or larger than the size of the CircularBuffer");
    return buffer_[(first_ + i) % capacity_];
}

template <typename valueType>
const valueType &CircularBuffer<valueType>::at(int i) const
{
    throwIfEmpty(this);
    if (i < 0 || i >= size_)
        throw CircularBuffer<valueType>::exception("Index is either less than zero or larger than the size of the CircularBuffer");
    return buffer_[(first_ + i) % capacity_];
}

template <typename valueType>
valueType &CircularBuffer<valueType>::front()
{
    throwIfEmpty(this);
    return buffer_[first_];
}

template <typename valueType>
valueType &CircularBuffer<valueType>::back()
{
    throwIfEmpty(this);
    return buffer_[last_];
}

template <typename valueType>
const valueType &CircularBuffer<valueType>::front() const
{
    throwIfEmpty(this);
    return buffer_[first_];
}

template <typename valueType>
const valueType &CircularBuffer<valueType>::back() const
{
    throwIfEmpty(this);
    return buffer_[last_];
}

template <typename valueType>
bool CircularBuffer<valueType>::is_linearized() const noexcept
{
    return first_ == 0;
}

template <typename valueType>
valueType *CircularBuffer<valueType>::linearize() noexcept
{
    if (this->empty())
        return nullptr;
    if (this->is_linearized())
        return buffer_;
    std::vector<valueType> tmp(size_);
    for (int i = 0; i < size_; i++)
        tmp[i] = (*this)[i];
    for (int i = 0; i < size_; i++)
        buffer_[i] = tmp[i];
    first_ = 0;
    last_ = size_ - 1;
    return buffer_;
}

template <typename valueType>
void CircularBuffer<valueType>::rotate(int newBegin) noexcept
{
    if (newBegin == 0 || this->empty())
        return;
    std::vector<valueType> tmp(size_);
    for (int i = 0; i < size_; i++)
        tmp[i] = (*this)[i + newBegin];
    for (int i = 0; i < size_; i++)
        buffer_[i] = tmp[i];
    first_ = 0;
    last_ = size_ - 1;
}

template <typename valueType>
void CircularBuffer<valueType>::set_capacity(int newCapacity)
{
    if (newCapacity <= 0)
        throw CircularBuffer<valueType>::exception("You cannot set newCapacity <= 0");
    else if (newCapacity < capacity_)
    {
        int newSize = size_;
        if (newCapacity < size_)
            newSize = newCapacity;
        std::vector<valueType> tmp(newSize);
        for (int i = 0; i < newSize; i++)
            tmp[i] = (*this)[i];
        delete[] buffer_;
        buffer_ = new valueType[newCapacity];
        for (int i = 0; i < newSize; i++)
            buffer_[i] = tmp[i];
        capacity_ = newCapacity;
        size_ = newSize;
        first_ = 0;
        last_ = size_ - 1;
    }
    else if (newCapacity == capacity_)
    {
        this->linearize();
        return;
    }
    else // (newCapacity > capacity_)
    {
        std::vector<valueType> tmp(size_);
        for (int i = 0; i < size_; i++)
            tmp[i] = (*this)[i];
        delete[] buffer_;
        buffer_ = new valueType[newCapacity];
        for (int i = 0; i < size_; i++)
            buffer_[i] = tmp[i];
        capacity_ = newCapacity;
        first_ = 0;
        last_ = size_ - 1;
    }
}

template <typename valueType>
void CircularBuffer<valueType>::resize(int newSize, const valueType &item)
{
    if (newSize < 0)
        throw CircularBuffer<valueType>::exception("You cannot set newSize < 0");
    else if (newSize == 0)
    {
        size_ = 0;
        first_ = 0;
        last_ = -1;
    }
    else if (newSize < size_)
    {
        size_ = newSize;
        last_ = mod(last_ - (size_ - newSize), capacity_);
    }
    else if (newSize == size_)
        return;
    else if (newSize <= capacity_)
    {
        const int newElemsNum = newSize - size_;
        for (int i = 0; i < newElemsNum; i++)
            this->push_back(item);
    }
    else // (newSize > capacity_)
    {
        std::vector<valueType> tmp(size_);
        for (int i = 0; i < size_; i++)
            tmp[i] = (*this)[i];
        delete[] buffer_;
        buffer_ = new valueType[newSize];
        for (int i = 0; i < size_; i++)
            buffer_[i] = tmp[i];
        for (int i = size_; i < newSize; i++)
            buffer_[i] = item;
        size_ = newSize;
        capacity_ = newSize;
        first_ = 0;
        last_ = size_ - 1;
    }
}

template <typename valueType>
CircularBuffer<valueType> &CircularBuffer<valueType>::operator=(const CircularBuffer &other)
{
    if (other.buffer_ == this->buffer_)
        return *this;
    this->set_capacity(other.capacity());
    capacity_ = other.capacity();
    size_ = other.size();
    first_ = 0;
    if (size_ == 0)
        last_ = -1;
    else
    {
        last_ = size_ - 1;
        for (int i = 0; i < size_; i++)
            buffer_[i] = other[i];
    }
    return *this;
}

template <typename valueType>
void CircularBuffer<valueType>::swap(CircularBuffer &cb)
{
    CircularBuffer<valueType> tmp(cb);
    cb = *this;
    *this = tmp;
}

template <typename valueType>
void CircularBuffer<valueType>::push_back(const valueType &item) noexcept
{
    if (size_ == 0)
    {
        buffer_[0] = item;
        size_ = 1;
        first_ = 0;
        last_ = 0;
    }
    else if (size_ < capacity_)
    {
        last_ = mod(last_ + 1, capacity_);
        buffer_[last_] = item;
        size_++;
    }
    else
    {
        last_ = first_;
        first_ = mod(first_ + 1, capacity_);
        buffer_[last_] = item;
    }
}

template <typename valueType>
void CircularBuffer<valueType>::push_front(const valueType &item) noexcept
{
    if (size_ == 0)
    {
        buffer_[0] = item;
        size_ = 1;
        first_ = 0;
        last_ = 0;
    }
    else if (size_ < capacity_)
    {
        first_ = mod(first_ - 1, capacity_);
        buffer_[first_] = item;
        size_++;
    }
    else
    {
        first_ = last_;
        last_ = mod(first_ - 1, capacity_);
        buffer_[first_] = item;
    }
}

template <typename valueType>
void CircularBuffer<valueType>::pop_back() noexcept
{
    if (size_ == 0)
        return;
    last_ = mod(last_ - 1, capacity_);
    size_--;
}

template <typename valueType>
void CircularBuffer<valueType>::pop_front() noexcept
{
    if (size_ == 0)
        return;
    first_ = mod(first_ + 1, capacity_);
    size_--;
}

template <typename valueType>
void CircularBuffer<valueType>::insert(int pos, const valueType &item)
{
    if (this->full())
        throw CircularBuffer<valueType>::exception("Buffer is already full");
    else
    {
        size_++;
        buffer_[mod(first_ + size_ - 1, capacity_)] = item;
        for (int i = mod(pos, size_); i < size_ - 1; i++)
        {
            valueType tmp = (*this)[i];
            (*this)[i] = (*this)[size_ - 1];
            (*this)[size_ - 1] = tmp;
        }
    }
}

template <typename valueType>
void CircularBuffer<valueType>::erase(int first, int last) noexcept
{
    const int newSize = size_ - mod(last - first, size_);
    for (int i = 0; i < newSize; i++)
        (*this)[first + i] = (*this)[last + i];
    size_ = newSize;
}

template <typename valueType>
void CircularBuffer<valueType>::clear() noexcept
{
    size_ = 0;
    first_ = 0;
    last_ = -1;
}

template <typename valueType>
bool operator==(const CircularBuffer<valueType> &a, const CircularBuffer<valueType> &b)
{
    if (a.size() != b.size() || a.capacity() != b.capacity())
        return false;
    for (int i = 0; i < a.size(); i++)
    {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

template <typename valueType>
bool operator!=(const CircularBuffer<valueType> &a, const CircularBuffer<valueType> &b)
{
    return !(a == b);
}

template <typename valueType>
void CircularBuffer<valueType>::print() const noexcept
{
    std::cout << "[ ";
    for (int i = 0; i < size_; i++)
        std::cout << (*this)[i] << " ";
    std::cout << "] ";
    std::cout << "size = " << size_ << " capacity = " << capacity_ << "\n";
}

template <typename valueType>
void CircularBuffer<valueType>::printBufferInMemory() const noexcept
{
    std::cout << "[ ";
    for (int i = 0; i < capacity_; i++)
    {
        if (first_ <= last_)
        {
            if (first_ <= i && i <= last_)
                std::cout << buffer_[i] << " ";
            else
                std::cout << "_ ";
        }
        else
        {
            if ((first_ <= i && i <= capacity_ - 1) || (0 <= i && i <= last_))
                std::cout << buffer_[i] << " ";
            else
                std::cout << "_ ";
        }
    }
    std::cout << "]\n";
}

// ====================================================================== iterator ====================================================================== //

template <typename valueType>
CircularBuffer<valueType>::iterator::iterator(CircularBuffer &cb, int beginIndex) noexcept : cb_(cb), index_(beginIndex){};

template <typename valueType>
valueType &CircularBuffer<valueType>::iterator::operator*() const
{
    if (cb_.empty())
        throw CircularBuffer<valueType>::exception("You cannot dereference an iterator that points to an empty CircularBuffer");
    return cb_[index_];
}

template <typename valueType>
typename CircularBuffer<valueType>::iterator &CircularBuffer<valueType>::iterator::operator=(const iterator &other) noexcept
{
    cb_ = other.cb_;
    index_ = other.index_;
    return *this;
}

template <typename valueType>
typename CircularBuffer<valueType>::iterator &CircularBuffer<valueType>::iterator::operator+(int shift) noexcept
{
    index_ += shift;
    return *this;
}

template <typename valueType>
typename CircularBuffer<valueType>::iterator &CircularBuffer<valueType>::iterator::operator-(int shift) noexcept
{
    index_ -= shift;
    return *this;
}

template <typename valueType>
typename CircularBuffer<valueType>::iterator &CircularBuffer<valueType>::iterator::operator+=(int shift) noexcept
{
    index_ += shift;
    return *this;
}

template <typename valueType>
typename CircularBuffer<valueType>::iterator &CircularBuffer<valueType>::iterator::operator-=(int shift) noexcept
{
    index_ -= shift;
    return *this;
}

template <typename valueType>
typename CircularBuffer<valueType>::iterator &CircularBuffer<valueType>::iterator::operator++() noexcept // ++a;
{
    index_++;
    return *this;
}

template <typename valueType>
typename CircularBuffer<valueType>::iterator CircularBuffer<valueType>::iterator::operator++(int) noexcept // a++;
{
    CircularBuffer<valueType>::iterator tmp(cb_, index_);
    index_++;
    return tmp;
}

template <typename valueType>
typename CircularBuffer<valueType>::iterator &CircularBuffer<valueType>::iterator::operator--() noexcept // --a;
{
    index_--;
    return *this;
}

template <typename valueType>
typename CircularBuffer<valueType>::iterator CircularBuffer<valueType>::iterator::operator--(int) noexcept // a--;
{
    CircularBuffer<valueType>::iterator tmp(cb_, index_);
    index_--;
    return tmp;
}

template <typename valueType>
bool CircularBuffer<valueType>::iterator::operator==(const iterator &other) const noexcept
{
    // return (cb_ == other.cb_ && mod(index_, cb_.size()) == mod(other.index_, other.cb_.size()));
    return (cb_ == other.cb_ && index_ == other.index_);
}

template <typename valueType>
bool CircularBuffer<valueType>::iterator::operator!=(const iterator &other) const noexcept
{
    // return !(cb_ == other.cb_ && mod(index_, cb_.size()) == mod(other.index_, other.cb_.size()));
    return !(cb_ == other.cb_ && index_ == other.index_);
}

template <typename valueType>
typename CircularBuffer<valueType>::iterator CircularBuffer<valueType>::begin() const noexcept
{
    return iterator(const_cast<CircularBuffer<valueType> &>(*this), 0);
}

template <typename valueType>
typename CircularBuffer<valueType>::iterator CircularBuffer<valueType>::end() const noexcept
{
    return iterator(const_cast<CircularBuffer<valueType> &>(*this), this->size());
}

#pragma once

/*
Class Iterator and ConstIterator. Class to iterate over Storage.

Class Storage. Due to this class there is no need to write rule of five.

Class Matrix. Functionality:
    operator[]
    size_type rows()
    size_type cols()
    bool square()
    bool empty()
    bool equals(const Matrix&)
    value_type det()
    void transpose()
    operator+=
    operator-=
    operator*=
    operator/=
    void dump()
    void read()
*/

#include <list>
#include <memory>
#include <fstream>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include "Utils.hpp"

namespace matrix {

template<typename T>
class Iterator;
template<typename T>
class ConstIterator;
template<typename T>
class Storage;
template<typename T>
class Matrix;

template<typename T>
class Iterator
{
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using reference = T &;
    using pointer = T *;

    pointer ptr_;

public:
    Iterator(pointer ptr = nullptr):
        ptr_(ptr) {}

    pointer get() {
        return ptr_;
    }

    reference operator*() const {
        assert(ptr_);
        return *ptr_;
    }

    pointer operator->() const {
        return ptr_;
    }

    Iterator& operator++() {
        ptr_++;
        return *this;
    }

    Iterator operator++(int) {
        Iterator me = *this;
        ptr_++;
        return me;
    }

    Iterator& operator--() {
        ptr_--;
        return *this;
    }

    Iterator operator--(int) {
        Iterator me = *this;
        ptr_--;
        return me;
    }

    Iterator& operator+=(difference_type d) {
        ptr_ += d;
        return *this;
    }

    Iterator& operator-=(difference_type d) {
        ptr_ -= d;
        return *this;
    }

    auto operator<=>(const Iterator& rhs) const = default;

    reference operator[](difference_type d) const {
        return *(*this + d);
    }
};

template<typename T>
class ConstIterator
{
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using reference = const T &;
    using pointer = const T *;

    pointer ptr_;

public:
    ConstIterator(pointer ptr = nullptr):
        ptr_(ptr) {}

    pointer get() {
        return ptr_;
    }

    reference operator*() const {
        assert(ptr_);
        return *ptr_;
    }

    pointer operator->() const {
        return ptr_;
    }

    ConstIterator& operator++() {
        ptr_++;
        return *this;
    }

    ConstIterator operator++(int) {
        Iterator me = *this;
        ptr_++;
        return me;
    }

    ConstIterator& operator--() {
        ptr_--;
        return *this;
    }

    ConstIterator operator--(int) {
        Iterator me = *this;
        ptr_--;
        return me;
    }

    ConstIterator& operator+=(difference_type d) {
        ptr_ += d;
        return *this;
    }

    ConstIterator& operator-=(difference_type d) {
        ptr_ -= d;
        return *this;
    }

    auto operator<=>(const ConstIterator& rhs) const = default;

    reference operator[](difference_type d) const {
        return *(*this + d);
    }
};

template<typename T>
class Storage final
{
    using value_type = T;
    using size_type = std::size_t;
    using iterator = Iterator<T>;
    using const_iterator = ConstIterator<T>;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;

    std::unique_ptr<value_type[]> data_;
    size_type size_ = 0U;

    bool equals(const Storage& rhs) const {
        if (size_ != rhs.size_) {
            return false;
        }
        for (size_type i = 0; i < size_; ++i) {
            if (!::matrix::equals((*this)[i], rhs[i])) {
                return false;
            }
        }
        return true;
    }

public:
    Storage() {}

    Storage(size_type size) {
        data_ = std::make_unique<value_type[]>(size);
        size_ = size;
    }

    Storage(size_type size, const_reference val):
        Storage(size) {
        std::fill_n(data_.get(), size, val);
    }

    Storage(size_type size, std::initializer_list<value_type> lst):
        Storage(size) {
        std::copy_n(lst.begin(), std::min(size, lst.size()), data_.get());
    }

    Storage(const Storage<T>& rhs):
        Storage(rhs.size_) {
        std::copy_n(rhs.data_.get(), rhs.size_, data_.get());
    }

    Storage(Storage<T>&& rhs) noexcept {
        std::swap(data_, rhs.data_);
        std::swap(size_, rhs.size_);
    }

    Storage& operator=(const Storage& rhs) {
        if (this == std::addressof(rhs)) {
            return *this;
        }
        Storage temp{rhs};
        std::swap(data_, temp.data_);
        std::swap(size_, temp.size_);
        return *this;
    }

    Storage& operator=(Storage&& rhs) noexcept {
        if (this == std::addressof(rhs)) {
            return *this;
        }
        Storage temp{std::move(rhs)};
        std::swap(data_, temp.data_);
        std::swap(size_, temp.size_);
        return *this;
    }

    ~Storage() = default;

    size_type size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0U;
    }

    bool operator==(const Storage& rhs) const {
        return equals(rhs);
    }

    bool operator!=(const Storage& rhs) const {
        return !equals(rhs);
    }

    reference operator[](size_type indx) {
        if (empty()) {
            throw std::logic_error("Operator[]: storage is empty");
        }
        if (indx >= size_) {
            throw std::out_of_range("Operator[]: index >= size");
        }
        return data_[indx];
    }

    const_reference operator[](size_type indx) const {
        if (empty()) {
            throw std::logic_error("Operator[]: storage is empty");
        }
        if (indx >= size_) {
            throw std::out_of_range("Operator[]: index >= size");
        }
        return data_[indx];
    }

    iterator begin() {
        return iterator{data_.get()};
    }

    iterator end() {
        return iterator{data_.get() + size_};
    }

    const_iterator begin() const {
        return const_iterator{data_.get()};
    }

    const_iterator end() const {
        return const_iterator{data_.get() + size_};
    }

    void resize(size_type size) {
        Storage temp{size};
        std::move(data_.get(), data_.get() + std::min(size_, size), temp.data_.get());
        std::swap(temp.data_, data_);
        std::swap(temp.size_, size_);
    }

    void clear() {
        size_ = 0U;
        data_.reset();
    }

    void dump() const {
        std::cout << "size " << size_ << "\n" << "data {";
        for (auto it = begin(); it != end(); ++it) {
            if (it != begin()) {
                std::cout << ", ";
            }
            std::cout << *it;
        }
        std::cout << "}" << std::endl;
    }
};

template<typename T>
class Matrix final
{
    using value_type = T;
    using size_type = std::size_t;
    using iterator = Iterator<T>;
    using const_iterator = ConstIterator<T>;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;

    size_type m_;
    size_type n_;
    Storage<value_type> buffer_;
    Storage<size_type> rows_;

    class ProxyRow
    {
        pointer row_ = nullptr;
        size_type n_ = 0U;

    public:
        using iterator = Iterator<value_type>;
        using const_iterator = ConstIterator<value_type>;

        ProxyRow(pointer row, size_type n):
            row_(row), n_(n) {}

        reference operator[](size_type indx) {
            if (indx >= n_) {
                throw std::out_of_range("Operator[]: index >= n");
            }
            return row_[indx];
        }

        const_reference operator[](size_type indx) const {
            if (indx >= n_) {
                throw std::out_of_range("Operator[]: index >= n");
            }
            return row_[indx];
        }

        size_type size() const {
            return n_;
        }

        iterator begin() {
            return iterator{row_};
        }

        iterator end() {
            return iterator{row_ + n_};
        }

        const_iterator begin() const {
            return iterator{row_};
        }

        const_iterator end() const {
            return iterator{row_ + n_};
        }
    };

    class ConstProxyRow
    {
        const_pointer row_ = nullptr;
        size_type n_ = 0U;

    public:
        using const_iterator = ConstIterator<value_type>;

        ConstProxyRow(const_pointer row, size_type n):
            row_(row), n_(n) {}

        const_reference operator[](size_type indx) const {
            if (indx >= n_) {
                throw std::logic_error("Operator[]: index >= n");
            }
            return row_[indx];
        }

        size_type size() const {
            return n_;
        }

        const_iterator begin() const {
            return const_iterator{row_};
        }

        const_iterator end() const {
            return const_iterator{row_ + n_};
        }
    };

    void setRows() {
        for (size_type i = 0; i < m_; ++i) {
            rows_[i] = i * n_;
        }
    }

    void swapRows(size_type i, size_type j) {
        std::swap(rows_[i], rows_[j]);
    }

    Matrix& transposeSquare() {
        if (m_ != 1U) {
            for (size_type i = 0; i < m_; ++i) {
                for (size_type j = i + 1; j < n_; ++j) {
                    std::swap((*this)[i][j], (*this)[j][i]);
                }
            }
        }
        return *this;
    }

    bool equals(const Matrix& rhs) const {
        if (m_ != rhs.m_ || n_ != rhs.n_) {
            return false;
        }
        for (size_type i = 0; i < m_; ++i) {
            for (size_type j = 0; j < n_; ++j) {
                if (!::matrix::equals((*this)[i][j], rhs[i][j])) {
                    return false;
                }
            }
        }
        return true;
    }

    size_type nonZeroRowInCol(size_type k) const {
        if (::matrix::isZero((*this)[k][k])) {
            size_type m = 0;
            for (m = k + 1; m < n_; ++m) {
                if (!::matrix::isZero((*this)[m][k])) {       
                    return m;
                }
            }
            return n_;
        }
        return k;
    }

    value_type detBareiss() const {
        if (!square()) {
            throw std::logic_error("Matrix isn't square");
        }
        if (empty()) {
            throw std::logic_error("Matrix is empty");
        }
        if (n_ == 1U) {
            return (*this)[0][0];
        }
        Matrix mtx{*this};
        value_type sign = value_type{1};
        for (size_type k = 0; k < n_ - 1; ++k) {
            size_type m = mtx.nonZeroRowInCol(k);
            if (m == n_) {
                return value_type{0};
            } else if (m != k) {
                mtx.swapRows(m, k);
                sign = -sign;
            }

            for (size_type i = k + 1; i < n_; ++i) {
                for (size_type j = k + 1; j < n_; ++j) {
                    mtx[i][j] = mtx[k][k] * mtx[i][j] - mtx[i][k] * mtx[k][j];
                    if (k != 0) {
                        mtx[i][j] /= mtx[k - 1][k - 1];
                    }
                }
            }
        }
        return sign * mtx[n_ - 1][n_ - 1];
    }

    value_type detGauss() const requires std::is_floating_point_v<value_type> {
        if (!square()) {
            throw std::logic_error("Matrix isn't square");
        }
        if (empty()) {
            throw std::logic_error("Matrix is empty");
        }
        if (n_ == 1U) {
            return (*this)[0][0];
        }
        Matrix mtx{*this};
        value_type sign = value_type{1};
        for(size_type k = 0; k < n_ - 1; ++k) {
            size_type m = mtx.nonZeroRowInCol(k);
            if (m == n_) {
                return value_type{0};
            } else if (m != k) {
                mtx.swapRows(m, k);
                sign = -sign;
            }

            for (size_type i = k + 1; i < n_; ++i) {
                value_type coef = mtx[i][k] / mtx[k][k];
                for (size_type j = 0; j < n_; ++j) {
                    mtx[i][j] -= coef * mtx[k][j];
                }
            }
        }
        value_type detAbs = value_type{1};
        for (size_type i = 0; i < n_; ++i) {
            detAbs *= mtx[i][i];
        }
        return sign * detAbs;
    }

public:
    Matrix() = default;

    Matrix(size_type size):
        Matrix(size, size) {}

    Matrix(size_type m, size_type n):
        m_(m), n_(n), buffer_(m * n), rows_(m) {
        setRows();
    }

    Matrix(std::initializer_list<std::initializer_list<value_type>> lst):
        m_(lst.size()), n_(::matrix::maxNestedLstSize(lst)), buffer_(m_ * n_), rows_(m_) {
        size_type i = 0U;
        for (const auto& nestedLst: lst) {
            size_type j = 0U;
            for (const auto& elem: nestedLst) {
                buffer_[i * n_ + j++] = elem;
            }
            i++;
        }
        setRows();
    }

    static Matrix eye(size_type size, const value_type& val) {
        Matrix res{size};
        for (size_type i = 0; i < size; ++i) {
            res[i][i] = val;
        }
        return res;
    }

    static Matrix diag(size_type size, std::initializer_list<value_type> lst) {
        Matrix res{size};
        size_type i = 0;
        for (const auto& elem: lst) {
            if (i == lst.size()) {
                break;
            }
            res[i][i++] = elem;
        }
        return res;
    }

    ProxyRow operator[](size_type indx) {
        return ProxyRow{std::addressof(buffer_[0]) + rows_[indx], n_};
    }

    ConstProxyRow operator[](size_type indx) const {
        return ConstProxyRow{std::addressof(buffer_[0]) + rows_[indx], n_};
    }

    size_type rows() const {
        return m_;
    }

    size_type cols() const {
        return n_;
    }

    bool square() const {
        return m_ == n_;
    }

    bool empty() const {
        return n_ == 0U || m_ == 0U;
    }

    bool operator==(const Matrix& rhs) const {
        return equals(rhs);
    }

    bool operator!=(const Matrix& rhs) const {
        return !equals(rhs);
    }

    value_type det() const {
        return detBareiss();
    }

    value_type det() const requires std::is_floating_point_v<value_type> {
        return detGauss();
    }

    Matrix& transpose() {
        if (square()) {
            return transposeSquare();
        }
        Matrix transposed{n_, m_};
        for (size_type i = 0; i < m_; ++i) {
            for (size_type j = 0; j < n_; ++j) {
                transposed[j][i] = std::move((*this)[i][j]);
            }
        }
        *this = std::move(transposed);
        return *this;
    }

    Matrix& operator+=(const Matrix& rhs) {
        if (m_ != rhs.m_ || n_ != rhs.n_) {
            throw std::logic_error("Operator +=: sizes don't match");
        }
        for (size_type i = 0; i < m_; ++i) {
            for (size_type j = 0; j < n_; ++j) {
                (*this)[i][j] += rhs[i][j];
            }
        }
        return *this;
    }

    Matrix& operator-=(const Matrix& rhs) {
        if (m_ != rhs.m_ || n_ != rhs.n_) {
            throw std::logic_error("Operator -=: sizes don't match");
        }
        for (size_type i = 0; i < m_; ++i) {
            for (size_type j = 0; j < n_; ++j) {
                (*this)[i][j] -= rhs[i][j];
            }
        }
        return *this;
    }

    Matrix& operator*=(const Matrix& rhs) {
        if (n_ != rhs.m_) {
            throw std::logic_error("Operator *=: sizes don't match");
        }
        Matrix product{m_, rhs.n_};
        for (size_type i = 0; i < m_; ++i) {
            for (size_type j = 0; j < rhs.n_; ++j) {
                for (size_type k = 0; k < n_; ++k) {
                    product[i][j] += (*this)[i][k] * rhs[k][j];
                }
            }
        }
        std::swap(*this, product);
        return *this;
    }

    Matrix& operator*=(const value_type& rhs) {
        for (size_type i = 0; i < m_; ++i) {
            for (size_type j = 0; j < n_; ++j) {
                (*this)[i][j] *= rhs;
            }
        }
        return *this;
    }

    Matrix& operator/=(const value_type& rhs) {
        for (size_type i = 0; i < m_; ++i) {
            for (size_type j = 0; j < n_; ++j) {
                (*this)[i][j] /= rhs;
            }
        }
        return *this;
    }

    void clear() {
        m_ = 0;
        n_ = 0;
        buffer_.clear();
        rows_.clear();
    }

    void dump(std::ostream& os) const {
        os << "M " << m_ << "; N " << n_ << "\n";
        for (size_type i = 0; i < m_; ++i) {
            for (size_type j = 0; j < n_; ++j) {
                os << std::setw(4) << (*this)[i][j] << " ";
            }
            os << "\n";
        }
        os.flush();
    }

    void read(std::istream& is) {
        for (size_type i = 0; i < m_; ++i) {
            for (size_type j = 0; j < n_; ++j) {
                is >> (*this)[i][j];
            }
        }
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& mtx)
{
    mtx.dump(os);
    return os;
}

template<typename T>
std::istream& operator>>(std::istream& is, Matrix<T>& mtx)
{
    mtx.read(is);
    return is;
}

template<typename T>
Matrix<T> operator+(const Matrix<T>& lhs, const Matrix<T>& rhs) {
    Matrix<T> res{lhs};
    res+=rhs;
    return res;
}

template<typename T>
Matrix<T> operator-(const Matrix<T>& lhs, const Matrix<T>& rhs) {
    Matrix<T> res{lhs};
    res-=rhs;
    return res;
}

template<typename T>
Matrix<T> operator*(const Matrix<T>& lhs, const T& rhs) {
    Matrix<T> res{lhs};
    res*=rhs;
    return res;
}

template<typename T>
Matrix<T> operator*(const T& lhs, const Matrix<T>& rhs) {
    Matrix<T> res{rhs};
    res*=lhs;
    return res;
}

template<typename T>
Matrix<T> operator*(const Matrix<T>& lhs, const Matrix<T>& rhs) {
    Matrix<T> res{lhs};
    res*=rhs;
    return res;
}

template<typename T>
Matrix<T> operator/(const Matrix<T>& lhs, const T& rhs) {
    Matrix<T> res{lhs};
    res/=rhs;
    return res;
}

} //namespace matrix

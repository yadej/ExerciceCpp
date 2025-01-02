#pragma once

#include <cstddef>
#include <iterator>

template<class T>
class iterator_base {
    public:
    using iterator_base_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t; 
    using value_type = T;
    using pointer = T*; 
    using reference =  T&;

    iterator_base(): m_curr(nullptr){
    }

    iterator_base( pointer it): m_curr(it){
    }


    iterator_base& operator++(){
        ++m_curr;
        return *this;
    }
    iterator_base operator++(int){
        iterator_base temp = *this;
        ++m_curr;
        return temp;
    }
    
    iterator_base& operator--(){
        --m_curr;
        return *this;
    }
    iterator_base operator--(int){
        iterator_base temp = *this;
        --m_curr;
        return temp;
    }
    
    iterator_base& operator+=(difference_type n){
        m_curr += n;
        return *this;
    }

    iterator_base& operator-=(difference_type n){
        m_curr -= n;
        return *this;
    }

    reference operator*() const {
        return *m_curr;
    }

    pointer operator->() const {
        return m_curr;
    }
    
    reference operator[](difference_type n) const {
        return *(m_curr + n);
    }

    pointer get() const {
        return m_curr;
    }

    friend bool operator==(const iterator_base& l_arg, const iterator_base& r_arg){
        return l_arg.m_curr == r_arg.m_curr;
    }

    friend bool operator!=(const iterator_base& l_arg, const iterator_base& r_arg){
        return !(l_arg == r_arg);
    }

    friend bool operator<(const iterator_base& l_arg, const iterator_base& r_arg){
        return l_arg.m_curr < r_arg.m_curr;
    }

    friend bool operator>(const iterator_base& l_arg, const iterator_base& r_arg){
        return r_arg < l_arg;
    }

    friend bool operator>=(const iterator_base& l_arg, const iterator_base& r_arg){
        return !(l_arg< r_arg);
    }

    friend bool operator<=(const iterator_base& l_arg, const iterator_base& r_arg){
        return !(r_arg < l_arg);
    }

    friend iterator_base operator+(const iterator_base& it, difference_type n){
        iterator_base temp = it;
        temp += n;
        return temp;
    }

    friend iterator_base operator+(difference_type n, const iterator_base& it){
        return it + n;
    }

    friend iterator_base operator-(const iterator_base& it, difference_type n){
        iterator_base temp = it;
        temp -= n;
        return temp;
    }
    
    friend iterator_base operator-(difference_type n, const iterator_base& it){
        return it - n;
    }

    friend difference_type operator-(const iterator_base& l_arg, const iterator_base& r_arg){
        return l_arg.m_curr - r_arg.m_curr;
    }

private:
    pointer m_curr{nullptr};
};

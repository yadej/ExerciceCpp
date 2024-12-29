#pragma once

#include <cstddef> // size_t ptrdiff_t  
#include <iterator> // random_access_iterator_tag
#include <limits> // numeric_limits -> max
#include <memory> // ptrdiff_t
#include <stdexcept> // invalid_argument 
#include <bit>  //For bit_ceil
#include <type_traits> // is_constructible_v
#include <utility> // forward move 

namespace yadej {


// Add concept to check either the type is a iterator
template<class Iter>
concept is_iterator = std::random_access_iterator<Iter>;


template<class T>
concept default_construction = std::is_default_constructible_v<T>;

//Well most function has either 
// a MoveInsertable
// or a CopyInsertable
// or a MoveAssignable
// or a CopyAssignable
template<class T>
concept can_use_move_copy = std::is_move_assignable_v<T>
                    && std::is_move_constructible_v<T>
                    && std::is_copy_assignable_v<T>
                    && std::is_copy_constructible_v<T>;

// TODO: Add the requirement for all function when needed

template<class T, class Allocator = std::allocator<T>> 
class Vector {
public:
    
    // Declaration of type
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference_type = T&;
    using const_reference_type = const T&;
    using pointer_type = T*;
    using const_pointer_type = const T*;
   // Constructors and Destructors 
    constexpr Vector() noexcept(noexcept(Allocator()));
    constexpr Vector( const allocator_type& alloc) noexcept;
    Vector( size_type count, const_reference_type value, const allocator_type& alloc = Allocator());
    explicit Vector( size_type count, const allocator_type& alloc = Allocator());
    template<class InputIt> requires is_iterator<InputIt>
    constexpr Vector( InputIt first, InputIt last, const allocator_type& alloc = Allocator());
    constexpr Vector(Vector && other);
    constexpr Vector(const Vector & other) noexcept;
    constexpr Vector( std::initializer_list<T> init, const allocator_type& alloc = Allocator());
    constexpr Vector &operator=(Vector &&);
    constexpr Vector &operator=(const Vector &);
    constexpr ~Vector();

    // Element access
    constexpr reference_type at(size_type position);
    constexpr const_reference_type at(size_type position) const;

    constexpr reference_type operator[](size_type position);
    constexpr const_reference_type operator[](size_type position) const;

    constexpr reference_type front();
    constexpr const_reference_type front() const;

    constexpr reference_type back();
    constexpr const_reference_type back() const;

    constexpr pointer_type data();
    constexpr const_pointer_type data() const;

    // iterator
    //
    class iterator;
    using const_iterator = const iterator;
    constexpr iterator begin();
    constexpr const_iterator begin() const noexcept;
    constexpr iterator end();
    constexpr const_iterator end() const noexcept;
    constexpr const_iterator cbegin() const noexcept;
    constexpr const_iterator cend() const noexcept;

    // Container
    //
    constexpr bool empty() const noexcept;
    constexpr size_type size() const noexcept;
    constexpr size_type max_size() const noexcept;
    constexpr void reserve( const size_type new_cap);
    constexpr size_type capacity() const noexcept;
    constexpr void shrink_to_fit();
    
    // Modifier
    constexpr void clear() noexcept;

    constexpr iterator insert( const_iterator pos, const_reference_type value);
    constexpr iterator insert( const_iterator pos, T&& value);
    constexpr iterator insert( const_iterator pos, size_type count, const_reference_type value);
    template<class InputIt> requires is_iterator<InputIt>
    constexpr iterator insert( const_iterator pos, InputIt first, InputIt last);
    constexpr iterator insert( const_iterator pos, std::initializer_list<T> ilist);
    template<class... Args>
    constexpr iterator emplace( const_iterator pos, Args&&... args);
    // TODO erase
    constexpr void erase( const_iterator pos);
    constexpr void erase( const_iterator first, const_iterator last);

    constexpr void push_back( const_reference_type value);
    constexpr void push_back( value_type&& value);
    template<class... Args>
    constexpr reference_type emplace_back( Args&&... args);
    constexpr void pop_back();
    constexpr void resize(size_type count);
    constexpr void resize(size_type count, const_reference_type value);
    // TODO swap
    // TODO Do Some operator
private:
    pointer_type m_elements=nullptr;
    size_type m_current_size{0};
    size_type m_max_size{0};
    Allocator allocator{};
    void destroy_elements(iterator first, iterator last);
    void deallocate_elements(pointer_type elements);
};

template<class T, class Allocator>
constexpr Vector<T, Allocator>::Vector() noexcept(noexcept(Allocator()))
        : m_elements(nullptr), m_current_size{0}, m_max_size{0},
          allocator(Allocator()){
}

template<class T, class Allocator>
constexpr Vector<T, Allocator>::Vector( const allocator_type& alloc) noexcept
        : m_elements(nullptr), m_current_size(0), m_max_size(0),
            allocator(alloc){
}

template<class T, class Allocator>
Vector<T,Allocator>::Vector( size_type count, 
                            const_reference_type value, 
                            const allocator_type& alloc )
                    : m_current_size(count),
                      m_max_size(std::bit_ceil(count)),
                      allocator(alloc){
    try {
        m_elements = std::allocator_traits<Allocator>::allocate(allocator, m_max_size);
        for (size_type i=0; i < count; ++i)
            std::allocator_traits<Allocator>::construct(allocator, m_elements + i, value);
    }
    catch(...) {
        destroy_elements(begin(), end());
        deallocate_elements(m_elements);
        throw;
    }
}

template<class T, class Allocator>
Vector<T,Allocator>::Vector( size_type count, const allocator_type& alloc )
        : m_current_size(count),
          m_max_size(std::bit_ceil(count)),
          allocator(alloc){
    try {
        m_elements = std::allocator_traits<Allocator>::allocate(allocator, m_max_size);
        for( size_type i =0; i  < count; ++i ){
            std::allocator_traits<Allocator>::construct(allocator, m_elements + i);
        }
    }catch (...){
        destroy_elements(begin(), end());
        deallocate_elements(m_elements);
        throw;
    }



}
template<class T, class Allocator>
template<class InputIt> requires is_iterator<InputIt>
constexpr Vector<T,Allocator>::Vector( InputIt first, InputIt last, const allocator_type& alloc )
        : allocator(alloc){
    // Get size of the Vector
    m_current_size = static_cast<size_type>(std::distance(first, last));
    m_max_size = std::bit_ceil(m_current_size);

    try {
        m_elements = std::allocator_traits<Allocator>::allocate(allocator, m_max_size);
        for(size_type i=0; i < m_current_size; first++,++i)
            std::allocator_traits<Allocator>::construct(allocator, m_elements + i, *first);
    } catch (...) {
        destroy_elements(begin(), end());
        deallocate_elements(m_elements);
        throw;
    }
}

template<class T, class Allocator>
constexpr Vector<T,Allocator>::Vector(Vector && other)
        : m_current_size(other.m_current_size), 
          m_max_size(other.m_max_size),
          m_elements(std::move(other.m_elements)),
          allocator(other.allocator){
    other.m_elements = nullptr;
    other.m_max_size = 0;
    other.m_current_size = 0;
}

template<class T, class Allocator>
constexpr Vector<T, Allocator>::Vector(const Vector & other) noexcept 
            : m_current_size(other.m_current_size),
              m_max_size(other.m_max_size),
              allocator(other.allocator){
    try {
        m_elements = std::allocator_traits<Allocator>::allocate(allocator, m_max_size);
        for(size_type i=0; i < m_current_size; ++i)
            std::allocator_traits<Allocator>::construct(allocator, m_elements + i, other.m_elements[i]);
    }catch(...){
        destroy_elements(begin(), end());
        deallocate_elements(m_elements);
        throw;
    }
}

template<class T, class Allocator>
constexpr Vector<T, Allocator>::Vector(std::initializer_list<T> init, const Allocator& alloc)
    : m_current_size( init.size()), 
      m_max_size( std::bit_ceil( init.size())), 
      allocator( alloc){
    
    try {
        m_elements = std::allocator_traits<Allocator>::allocate(allocator, m_max_size);
        auto copy = init.begin();
        for(size_type i=0 ;i < m_current_size; ++i, ++copy)
            std::allocator_traits<Allocator>::construct(allocator, m_elements + i, *copy); 
    }catch(...){
        destroy_elements(begin(), end());
        deallocate_elements(m_elements);
        throw;
    }
}

template<class T, class Allocator>
constexpr Vector<T, Allocator>& Vector<T, Allocator>::operator=(Vector<T, Allocator> && other){
    m_current_size = other.m_current_size;
    m_max_size = other.m_max_size;
    m_elements = other.m_elements;
    allocator = other.allocator;
    //other.m_elements = nullptr;
    other.m_max_size = 0;
    other.m_current_size = 0;
    return *this;
}

template<class T, class Allocator>
constexpr Vector<T,Allocator>& Vector<T, Allocator>::operator=(const Vector<T, Allocator>& other){
    m_current_size =  other.m_current_size;
    m_max_size = other.m_max_size;
    allocator = other.allocator;
    try {
        m_elements = std::allocator_traits<Allocator>::allocate(allocator, m_max_size);
        for(size_type i=0; i < m_current_size; ++i)
            std::allocator_traits<Allocator>::construct(allocator, &m_elements[i], other.m_elements[i]);
    }
    catch(...){
        destroy_elements(begin(), end());
        deallocate_elements(m_elements);
        throw;
    }
    return *this;
}


template<class T, class Allocator>
constexpr Vector<T, Allocator>::~Vector() {
    if(m_max_size == 0)
        return;

    destroy_elements(begin(), end());
    deallocate_elements( m_elements);
    m_current_size = 0;
    m_max_size = 0;
    m_elements = nullptr;
}


template<class T, class Allocator>
constexpr T& Vector<T, Allocator>::at(size_type position){
    if( position > m_current_size ) 
        throw std::out_of_range("");
    return m_elements[position];
}
template<class T, class Allocator>
constexpr const T& Vector<T, Allocator>::at(size_type position) const{
    if( position > m_current_size ) 
        throw std::out_of_range("");
    return m_elements[position];
}

template<class T, class Allocator>
constexpr T& Vector<T, Allocator>::operator[](size_type position){
    return m_elements[position];
}
template<class T, class Allocator>
constexpr const T& Vector<T, Allocator>::operator[](size_type position) const{
    return m_elements[position];
}

template<class T, class Allocator>
constexpr T& Vector<T, Allocator>::front(){
    return at(0);
}
template<class T, class Allocator>
constexpr const T& Vector<T, Allocator>::front()const{
    return at(0);
}


template<class T, class Allocator>
constexpr T& Vector<T, Allocator>::back(){
    return at(m_current_size - 1);
}
template<class T, class Allocator>
constexpr const T& Vector<T, Allocator>::back() const{
    return at(m_current_size - 1);
}

template<class T, class Allocator>
constexpr T* Vector<T, Allocator>::data(){
    return m_elements;
}
template<class T, class Allocator>
constexpr const T* Vector<T, Allocator>::data() const{
    return m_elements;
}

template<class T, class Allocator>
class Vector<T, Allocator>::iterator {
    public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = Vector<T, Allocator>::difference_type;
    using value_type = Vector<T, Allocator>::value_type;
    using pointer = Vector<T, Allocator>::pointer_type;
    using reference = Vector<T, Allocator>::reference_type;

    iterator(): m_curr(nullptr){
    }

    iterator( pointer_type it): m_curr(it){
    }


    iterator& operator++(){
        ++m_curr;
        return *this;
    }
    iterator operator++(int){
        iterator temp = *this;
        ++m_curr;
        return temp;
    }
    
    iterator& operator--(){
        --m_curr;
        return *this;
    }
    iterator operator--(int){
        iterator temp = *this;
        --m_curr;
        return temp;
    }
    
    iterator& operator+=(difference_type n){
        m_curr += n;
        return *this;
    }

    iterator& operator-=(difference_type n){
        m_curr -= n;
        return *this;
    }

    reference_type operator*() const {
        return *m_curr;
    }

    pointer_type operator->() const {
        return m_curr;
    }
    
    reference_type operator[](difference_type n) const {
        return *(m_curr + n);
    }

    pointer_type get() const {
        return m_curr;
    }

    friend bool operator==(const iterator& l_arg, const iterator& r_arg){
        return l_arg.m_curr == r_arg.m_curr;
    }

    friend bool operator!=(const iterator& l_arg, const iterator& r_arg){
        return !(l_arg == r_arg);
    }

    friend bool operator<(const iterator& l_arg, const iterator& r_arg){
        return l_arg.m_curr < r_arg.m_curr;
    }

    friend bool operator>(const iterator& l_arg, const iterator& r_arg){
        return r_arg < l_arg;
    }

    friend bool operator>=(const iterator& l_arg, const iterator& r_arg){
        return !(l_arg< r_arg);
    }

    friend bool operator<=(const iterator& l_arg, const iterator& r_arg){
        return !(r_arg < l_arg);
    }

    friend iterator operator+(const iterator& it, difference_type n){
        iterator temp = it;
        temp += n;
        return temp;
    }

    friend iterator operator+(difference_type n, const iterator& it){
        return it + n;
    }

    friend iterator operator-(const iterator& it, difference_type n){
        iterator temp = it;
        temp -= n;
        return temp;
    }
    
    friend iterator operator-(difference_type n, const iterator& it){
        return it - n;
    }

    friend difference_type operator-(const iterator& l_arg, const iterator& r_arg){
        return l_arg.m_curr - r_arg.m_curr;
    }

private:
    pointer_type m_curr{nullptr};
};

template<class T, class Allocator>
constexpr Vector<T, Allocator>::iterator Vector<T, Allocator>::begin(){
    return Vector<T, Allocator>::iterator(m_elements);
}

template<class T, class Allocator>
constexpr const Vector<T, Allocator>::iterator Vector<T, Allocator>::begin() const noexcept{
    return Vector<T, Allocator>::iterator(m_elements);
}

template<class T, class Allocator>
constexpr Vector<T, Allocator>::iterator Vector<T, Allocator>::end(){
    return Vector<T, Allocator>::iterator(&m_elements[m_current_size]);
}

template<class T, class Allocator>
constexpr const Vector<T, Allocator>::iterator Vector<T, Allocator>::end() const noexcept{
    return Vector<T, Allocator>::iterator(&m_elements[m_current_size]);
}

template<class T, class Allocator>
constexpr const Vector<T, Allocator>::iterator Vector<T, Allocator>::cbegin() const noexcept{
    return Vector<T, Allocator>::iterator(m_elements);
}

template<class T, class Allocator>
constexpr const Vector<T, Allocator>::iterator Vector<T, Allocator>::cend() const noexcept{
    return Vector<T, Allocator>::iterator(&m_elements[m_current_size - 1]);
}

template<class T, class Allocator>
constexpr bool Vector<T, Allocator>::empty() const noexcept{
    return ( m_current_size == 0);
}

template<class T, class Allocator>
constexpr std::size_t Vector<T, Allocator>::size() const noexcept{
    return m_current_size;
}

template<class T, class Allocator>
constexpr std::size_t Vector<T, Allocator>::max_size() const noexcept{
    return std::numeric_limits<ptrdiff_t>::max();
}

template<class T, class Allocator>
constexpr void Vector<T, Allocator>::reserve(const size_type new_cap){
    if (new_cap > max_size())
        throw std::length_error(" The new cap exceed the absolute maximum size");
    if (new_cap <= m_max_size) 
        return;

    pointer_type new_element = std::allocator_traits<Allocator>::allocate(allocator, new_cap);

    for(size_type i=0; i < m_current_size; ++i)
        std::allocator_traits<Allocator>::construct(allocator,new_element+i, m_elements[i]);

    destroy_elements(begin(), end());
    deallocate_elements( m_elements);

    m_elements = new_element;

    m_max_size = new_cap;


}

template<class T, class Allocator>
constexpr std::size_t Vector<T, Allocator>::capacity() const noexcept{
    return m_max_size;
}

template<class T, class Allocator>
constexpr void Vector<T, Allocator>::shrink_to_fit(){
    pointer_type new_elements = std::move( m_elements);
    try { 
        m_elements = std::allocator_traits<Allocator>::allocate(allocator, m_current_size);
    }catch(...){
        deallocate_elements(new_elements);
        m_max_size = m_current_size;
        deallocate_elements(m_elements);
    }
    for(size_type i=0; i < m_current_size; ++i)
        std::allocator_traits<Allocator>::construct(allocator, m_elements + i, new_elements[i]);
    deallocate_elements(new_elements);
    m_max_size = m_current_size;
}

template<class T, class Allocator>
constexpr void Vector<T, Allocator>::clear() noexcept{
    if ( m_max_size == 0)
        return;

    destroy_elements(begin(), end());
    deallocate_elements(m_elements);
    m_elements = nullptr;
    m_max_size = 0;
    m_current_size = 0;
}


template<class T, class Allocator>
constexpr Vector<T, Allocator>::iterator Vector<T, Allocator>::insert( const_iterator pos,
                                                                      const_reference_type value){
    // Check if pos is inside the container
    // Since our iterator is random access iterator
    // We have comparator to test
    if( pos < begin() || end() < pos)
        throw std::invalid_argument("insert position not in container");

    difference_type insert_pos = std::distance(begin(), pos);
    if( m_current_size  == m_max_size) {
        pointer_type new_element = std::allocator_traits<Allocator>::allocate(allocator,
                                                                              std::bit_ceil(m_current_size + 1));

        for(size_type i=0; i < insert_pos;++i)
            std::allocator_traits<Allocator>::construct(allocator, &new_element[i], m_elements[i]);

        std::allocator_traits<Allocator>::construct(allocator, &new_element[insert_pos], value);

        for(size_type i=insert_pos; i < m_current_size; ++i)
            std::allocator_traits<Allocator>::construct(allocator, &new_element[i + 1],m_elements[i]);

        destroy_elements(begin(), end());
        deallocate_elements(m_elements);

        m_elements = new_element;
        ++m_current_size;
        m_max_size = std::bit_ceil(m_current_size);
    } else {

        // We allocate the last one as N - 1 to up the iteration 
        // to be able to put value inside
        std::allocator_traits<Allocator>::construct(allocator,
                                                    &m_elements[m_current_size],
                                                    m_elements[m_current_size - 1]);

        // Iteration of N-1 to pos to shift the variable
        for(iterator it = end() - 1; it > pos; --it){
            *it = *(it-1);
        }
        *pos = value;
        ++m_current_size;
    }
    return pos;
}

template<class T, class Allocator>
constexpr Vector<T, Allocator>::iterator Vector<T, Allocator>::insert( const_iterator pos, T&& value){
    
    // Check if pos is inside the container
    // Since our iterator is random access iterator
    // We have comparator to test
    if( pos < begin() || end() < pos)
        throw std::invalid_argument("insert position not in container");

    difference_type insert_pos = std::distance(begin(), pos);
    
    if( m_current_size == m_max_size){
        pointer_type new_element = std::allocator_traits<Allocator>::allocate(allocator,
                                                                              std::bit_ceil(m_max_size + 1));

        for(size_type i=0; i < insert_pos;++i)
            std::allocator_traits<Allocator>::construct(allocator, &new_element[i], m_elements[i]);

        std::allocator_traits<Allocator>::construct(allocator, &new_element[insert_pos], std::move(value));

        for(size_type i=insert_pos; i < m_current_size; ++i)
            std::allocator_traits<Allocator>::construct(allocator, &new_element[i + 1],m_elements[i]);

        destroy_elements(begin(), end());
        deallocate_elements(m_elements);

        m_elements = new_element;
        ++m_current_size;
        m_max_size = std::bit_ceil(m_current_size);
        new_element = nullptr;
    } else {
        // We allocate the last one as N - 1 to up the iteration 
        // to be able to put value inside
        std::allocator_traits<Allocator>::construct(allocator,
                                                    &m_elements[m_current_size],
                                                    m_elements[m_current_size - 1]);

        // Iteration of N-1 to pos to shift the variable
        for(size_type i=m_current_size - 1; i > insert_pos; --i){
            *( begin() + i) = *( begin() + i - 1);
        }

        *( begin() + insert_pos) = std::move(value);
        ++m_current_size;
    }
    return pos;
}

template<class T, class Allocator>
constexpr Vector<T, Allocator>::iterator Vector<T, Allocator>::insert( const_iterator pos, size_type count, const_reference_type value){

    // Check if pos is inside the container
    // Since our iterator is random access iterator
    // We have comparator to test
    if( pos < begin() || end() < pos)
        throw std::invalid_argument("insert position not in container");

    difference_type insert_pos = std::distance(begin(), pos);
    
    if( m_current_size + count > m_max_size){
        pointer_type new_element = std::allocator_traits<Allocator>::allocate(allocator,
                                                                              std::bit_ceil(m_current_size + count));

        for(size_type i=0; i < insert_pos;++i)
            std::allocator_traits<Allocator>::construct(allocator, &new_element[i], m_elements[i]);
        
        for(size_type i=insert_pos;i < insert_pos + count;++i)
            std::allocator_traits<Allocator>::construct(allocator, &new_element[i], value);

        for(size_type i=insert_pos ; i < m_current_size; ++i)
            std::allocator_traits<Allocator>::construct(allocator, &new_element[i + count + 1],m_elements[i]);

        destroy_elements(begin(), end());
        deallocate_elements(m_elements);

        m_elements = new_element;
        m_current_size += count;
        m_max_size = std::bit_ceil(m_current_size);
    } else { 
        
        // Put value N-count to N-1 -> N to N+count-1
        for(size_type i=0; i < count; ++i){
            std::allocator_traits<Allocator>::construct(allocator, 
                                                        &m_elements[m_current_size+i],
                                                        m_elements[m_current_size-count+i]);
        }

        // Shift value pos, N-count to pos+count to N-1
        for(size_type i=m_current_size; i > insert_pos + count;++i){
            *(begin() + i) = *(begin() + i - 1);
        }
        
        // put value in pos, pos+count
        for(size_type i=insert_pos; i < insert_pos + count;++i){
            *(begin() + i) = value;
        }

        m_current_size += count;



    }
    return pos;
}
template<class T, class Allocator>
template<class InputIt> requires is_iterator<InputIt>
constexpr Vector<T, Allocator>::iterator Vector<T, Allocator>::insert( const_iterator pos, InputIt first, InputIt last){


    // Check if pos is inside the container
    // Since our iterator is random access iterator
    // We have comparator to test
    if( pos < begin() || end() < pos)
        throw std::invalid_argument("insert position not in container");

    difference_type insert_pos = std::distance(begin(), pos);
    difference_type size_insert = std::distance(first, last);

    pointer_type new_element = std::allocator_traits<Allocator>::allocate(allocator,
                                                                          std::bit_ceil(m_current_size + size_insert+ 1));

    for(size_type i=0; i < insert_pos;++i)
        std::allocator_traits<Allocator>::construct(allocator, &new_element[i], m_elements[i]);
   
    // Get first input to roll up in the new elements
    InputIt cycle = first;
    for(size_type i=insert_pos;cycle != last;++i, ++cycle)
        std::allocator_traits<Allocator>::construct(allocator, &new_element[i], *cycle);

    for(size_type i=insert_pos ; i < m_current_size; ++i)
        std::allocator_traits<Allocator>::construct(allocator, &new_element[i + size_insert + 1],m_elements[i]);

    destroy_elements(begin(), end());
    deallocate_elements(m_elements);

    m_elements = new_element;
    m_current_size += size_insert;
    m_max_size = std::bit_ceil(m_current_size);
    new_element = nullptr;
    return pos;
}

template<class T, class Allocator>
constexpr Vector<T, Allocator>::iterator Vector<T, Allocator>::insert( const_iterator pos, std::initializer_list<T> ilist){
    insert(pos, ilist.begin(), ilist.end());
    return pos;
}

template<class T, class Allocator>
template<class... Args>
constexpr Vector<T, Allocator>::iterator Vector<T, Allocator>::emplace( const_iterator pos, Args&&... args){

     // Check if pos is inside the container
    // Since our iterator is random access iterator
    // We have comparator to test
    if( pos < begin() || end() < pos)
        throw std::invalid_argument("insert position not in container");

    difference_type insert_pos = std::distance(begin(), pos);
    
    if( m_current_size == m_max_size){
        pointer_type new_element = std::allocator_traits<Allocator>::allocate(allocator,
                                                                              std::bit_ceil(m_max_size + 1));

        for(size_type i=0; i < insert_pos;++i)
            std::allocator_traits<Allocator>::construct(allocator, &new_element[i], m_elements[i]);

        std::allocator_traits<Allocator>::construct(allocator,
                                                    &new_element[insert_pos], 
                                                    new T(std::forward<T>(args)...));

        for(size_type i=insert_pos; i < m_current_size; ++i)
            std::allocator_traits<Allocator>::construct(allocator, &new_element[i + 1],m_elements[i]);

        destroy_elements(begin(), end());
        deallocate_elements(m_elements);

        m_elements = new_element;
        ++m_current_size;
        m_max_size = std::bit_ceil(m_current_size);
        new_element = nullptr;
    } else {
        // We allocate the last one as N - 1 to up the iteration 
        // to be able to put value inside
        std::allocator_traits<Allocator>::construct(allocator,
                                                    &m_elements[m_current_size],
                                                    m_elements[m_current_size - 1]);

        // Iteration of N-1 to pos to shift the variable
        for(size_type i=m_current_size - 1; i > insert_pos; --i){
            *( begin() + i) = *( begin() + i - 1);
        }

        *( begin() + insert_pos) = new T(std::forward<T>(args)...);
        ++m_current_size;
    }
    
    return pos;
}

template<class T, class Allocator>
constexpr void Vector<T, Allocator>::erase( Vector<T, Allocator>::iterator pos){
    if( pos < begin() || pos > end())
        return;
    
    difference_type erase_pos = std::distance(begin(), pos);
    for( size_type i=erase_pos; i < m_current_size; ++i){
        *( begin() + i) = *( begin() + i + 1);
    }
    destroy_elements(end()-1, end());
    m_current_size--;
}

template<class T, class Allocator>
constexpr void Vector<T, Allocator>::erase( Vector<T, Allocator>::iterator first,
                                            Vector<T, Allocator>::iterator last){
    if( first < begin() || first > end() || first < begin() || last > end())
        return;
    difference_type erase_pos = std::distance(begin(), first);
    difference_type end_pos = std::distance(first, last);
    for( size_type i=erase_pos; i < m_current_size - end_pos; ++i){
        *( begin() + i) = *( begin() + i + end_pos);
    }
    destroy_elements(end()-end_pos, end());
    m_current_size -= end_pos;

}

template<class T, class Allocator>
constexpr void Vector<T, Allocator>::push_back( const_reference_type value){
    if( m_current_size == m_max_size){
        size_type new_max_size = std::bit_ceil(m_max_size +1);
        pointer_type new_elements = std::allocator_traits<Allocator>::allocate(allocator, new_max_size);

        for(size_type i=0; i < m_max_size; ++i)
            std::allocator_traits<Allocator>::construct(allocator, &new_elements[i], m_elements[i]);

        destroy_elements(begin(), end());
        deallocate_elements(m_elements);

        m_elements = std::move(new_elements);
        new_elements = nullptr;
        m_max_size = new_max_size;
    }

    std::allocator_traits<Allocator>::construct(allocator, m_elements + m_current_size, value);
    ++m_current_size;
}

template<class T, class Allocator>
constexpr void Vector<T, Allocator>::push_back( value_type&& value){
    if( m_current_size == m_max_size){
        size_type new_max_size = std::bit_ceil(m_max_size +1);
        pointer_type new_elements = std::allocator_traits<Allocator>::allocate(allocator, new_max_size);

        for(size_type i=0; i < m_max_size; ++i)
            std::allocator_traits<Allocator>::construct(allocator, &new_elements[i], m_elements[i]);

        destroy_elements(begin(), end());
        deallocate_elements(m_elements);

        m_elements = std::move(new_elements);
        new_elements = nullptr;
        m_max_size = new_max_size;
    }
    std::allocator_traits<Allocator>::construct(allocator, m_elements + m_current_size, value);
    ++m_current_size;
}

template<class T, class Allocator>
template<class... Args>
constexpr T& Vector<T, Allocator>::emplace_back(Args&&... args){
     if( m_current_size == m_max_size){
        size_type new_max_size = std::bit_ceil(m_max_size +1);
        pointer_type new_elements = std::allocator_traits<Allocator>::allocate(allocator, new_max_size);

        for(size_type i=0; i < m_max_size; ++i)
            std::allocator_traits<Allocator>::construct(allocator, &new_elements[i], m_elements[i]);

        destroy_elements(begin(), end());
        deallocate_elements(m_elements);

        m_elements = std::move(new_elements);
        new_elements = nullptr;
        m_max_size = new_max_size;
    }
    // Make the allocation
    reference_type new_element = new T(std::forward<Args>(args)...);
    std::allocator_traits<Allocator>::construct(allocator, &new_element[m_current_size],new_element);
    ++m_current_size;
    return new_element;

}

template<class T, class Allocator>
constexpr void Vector<T, Allocator>::pop_back(){
    if(m_current_size == 0)
        return;

    std::allocator_traits<Allocator>::destroy(allocator, &m_elements[m_current_size - 1]);
    m_current_size--;
}

template<class T, class Allocator>
constexpr void Vector<T, Allocator>::resize(size_type count){
    if( count == m_current_size)
        return;

    if( count < m_current_size){
        Vector<T, Allocator>::iterator begin_destroyed_element = begin();
        destroy_elements(begin_destroyed_element+count, end());
    } else if( count > m_current_size && count < m_max_size){
        for(size_type i=m_current_size; i < count; ++i)
            std::allocator_traits<Allocator>::construct(allocator, m_elements + i, T());
    }else{

        // Last possibility count > size and count > capacity 
        // So that we need some reallocation
        size_type new_max_size = std::bit_ceil(count);
        pointer_type new_elements = std::allocator_traits<Allocator>::allocate(allocator, new_max_size);

        for(size_type i=0; i < m_current_size; ++i)
            std::allocator_traits<Allocator>::construct(allocator, &new_elements[i], m_elements[i]);

        for(size_type i=m_current_size; i < count; ++i)
            std::allocator_traits<Allocator>::construct(allocator, &new_elements[i], T());


        destroy_elements(begin(), end());
        deallocate_elements(m_elements);

        m_elements = std::move(new_elements);
        //new_elements = nullptr;
        m_max_size = new_max_size;
    }
    m_current_size = count;
}

template<class T, class Allocator>
constexpr void Vector<T, Allocator>::resize(size_type count, const_reference_type value) {
    if( count == m_current_size)
        return;

    if( count < m_current_size){
        Vector<T, Allocator>::iterator begin_destroyed_element = begin();
        destroy_elements(begin_destroyed_element+count, end());
    } else if( count > m_current_size && count < m_max_size){
        
        for(size_type i=m_current_size; i < count; ++i)
            std::allocator_traits<Allocator>::construct(allocator, &m_elements[i], value);
    } else {

        // Last possibility count > size and count > capacity 
        // So that we need some reallocation
        size_type new_max_size = std::bit_ceil(count);
        pointer_type new_elements = std::allocator_traits<Allocator>::allocate(allocator, new_max_size);

        for(size_type i=0; i < m_current_size; ++i)
            std::allocator_traits<Allocator>::construct(allocator, &new_elements[i], m_elements[i]);

        for(size_type i=m_current_size; i < count; ++i)
            std::allocator_traits<Allocator>::construct(allocator, &new_elements[i], value);

        destroy_elements(begin(), end());
        deallocate_elements(m_elements);

        m_elements = std::move(new_elements);
        //new_elements = nullptr;
        m_max_size = new_max_size;
    }
    m_current_size = count;
}

template<class T, class Allocator>
void Vector<T, Allocator>::destroy_elements(Vector<T, Allocator>::iterator first,
                                            Vector<T, Allocator>::iterator last){
    for(; first != last; first++){
        std::allocator_traits<Allocator>::destroy(allocator, first.get());
    }
}

template<class T, class Allocator>
void Vector<T, Allocator>::deallocate_elements(pointer_type elements){
    if(m_max_size == 0)
        return;

    std::allocator_traits<Allocator>::deallocate(allocator, elements, m_max_size);
}

}


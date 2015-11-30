/******************************************************************************
* Copyright (C) 2012-2014 Maximilien de Bayser
*
* SelfPortrait API - http://github.com/maxdebayser/SelfPortrait
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#ifndef SMALL_ARRAY_H
#define SMALL_ARRAY_H

#include <stdlib.h>
#include <stddef.h>
#include <stdexcept>
#include <initializer_list>

/* This is an extremely lean array class. No memory is initialized unless
 * explicitly requested. It can only grow by push_back and emplace_back
 * Iterators can become invalid by addition of elements.
 */

template<int ALIGN, int SIZE>
class SmallArrayBase {
public:
    typedef size_t size_type;
protected:
    enum {
        Small = 5
    };

    alignas(ALIGN) char m_fixed[Small*SIZE];

    char* m_excess;
    size_type m_size;

    char* access_mem(size_type i) {
        if (i < Small) {
            return m_fixed + i*SIZE;
        } else {
            return m_excess+ (i-Small)*SIZE;
        }
    }

    void inc() {
        ++m_size;
        if (m_size > Small) {
            int extra_space = (m_size - Small);
            void * new_space = realloc(m_excess, extra_space*SIZE);
            if (!new_space) {
                throw std::runtime_error("out of memory");
            }
            m_excess = reinterpret_cast<char*>(new_space);
        }
    }

    SmallArrayBase()
        : m_excess(nullptr)
        , m_size(0)
    {}

    ~SmallArrayBase() {
        if (m_excess) {
            free(m_excess);
        }
    }

public:
    size_type size() const {
        return m_size;
    }

};

template<class T>
class SmallArray: public SmallArrayBase<alignof(T), sizeof(T)> {
    typedef SmallArrayBase<alignof(T), sizeof(T)> Base;
public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef typename Base::size_type size_type;

    template<typename ptr, typename ref, typename arr>
    struct SmallArrayIterator {
           arr* array;
           size_type pos;

           ref operator*() {
               return *array->access_elem(pos);
           }

           ptr operator->() {
               return array->access_elem(pos);
           }

           SmallArrayIterator& operator++() {
               ++pos;
               return *this;
           }

           SmallArrayIterator operator++(int) {
               iterator tmp(*this);
               ++pos;
               return tmp;
           }


           SmallArrayIterator& operator--() {
               --pos;
               return *this;
           }

           SmallArrayIterator operator--(int) {
               iterator tmp(*this);
               --pos;
               return tmp;
           }

           bool operator==(const SmallArrayIterator& that) const {
               return (that.pos == this->pos) && (that.array == this->array);
           }
           bool operator!=(const SmallArrayIterator& that) const {
               return !(that == *this);
           }
    };

    typedef SmallArrayIterator<pointer, reference, SmallArray> iterator;
    typedef SmallArrayIterator<const_pointer, const_reference, const SmallArray> const_iterator;

private:

    T* access_elem(size_type i) {
        return reinterpret_cast<T*>(Base::access_mem(i));
    }

    const T* access_elem(size_type i) const {
        return const_cast<SmallArray&>(*this).access_elem(i);
    }

    template<class... Args>
    T& construct(size_type i, Args&&... args) {
        new(Base::access_mem(i)) T(::std::forward<Args>(args)...);
    }

    void destruct(size_type i) {
        access_elem(i)->~T();
    }

public:

    ~SmallArray() {
        for (int i = 0; i < Base::size(); ++i) {
            destruct(i);
        }
    }

    SmallArray() : Base() {}

    SmallArray(std::initializer_list<T> l) : Base() {
        for (const T& e: l) push_back(e);
    }

    T& operator[](size_type i) {
        return *(access_elem(i));
    }

    const T& operator[](size_type i) const {
        return const_cast<SmallArray&>(*this)[i];
    }

    void push_back(const_reference t) {
        Base::inc();
        construct(Base::size()-1, t);
    }

    void push_back(T&& t) {
        Base::inc();
        construct(Base::size()-1, std::move(t));
    }

    template<class... Args>
    void emplace_back(Args&&... args) {
        Base::inc();
        construct(Base::size()-1, ::std::forward<Args>(args)...);
    }

    iterator begin() {
        return {this, 0 };
    }

    iterator end() {
        return {this, Base::size() };
    }

    const_iterator begin() const {
        return const_iterator{this, 0 };
    }

    const_iterator end() const {
        return {this, Base::size() };
    }

    reference front() { return *(access_elem(0)); }
    const_reference front() const { return *(access_elem(0)); }

    reference back() { return *(access_elem(Base::size()-1)); }
    const_reference back() const { return *(access_elem(Base::size()-1)); }
};



#endif

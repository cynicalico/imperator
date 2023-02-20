/*
*  segmented_array.hpp
*
*  Copyright (c) 2007 Leigh Johnston.
*
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are
*  met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*
*     * Neither the name of Leigh Johnston nor the names of any
*       other contributors to this software may be used to endorse or
*       promote products derived from this software without specific prior
*       written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
*  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
*  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
*  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
*  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
*  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
*  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
*  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <type_traits>
#include <cstdint>
#include <utility>
#include <variant>
#include <optional>
#include <chrono>

//#include <neolib/neolib_export.hpp>

#ifdef NDEBUG
constexpr bool ndebug = true;
#else
constexpr bool ndebug = false;
#endif

#define rvalue_cast static_cast

#define GENERATE_HAS_MEMBER_TYPE(Type)                                            \
                                                                                  \
template <class T, bool OK = std::is_class_v<T>>                                  \
class HasMemberType_##Type                                                        \
{                                                                                 \
public:                                                                           \
    static constexpr bool RESULT = false;                                         \
};                                                                                \
                                                                                  \
template <class T>                                                                \
class HasMemberType_##Type<T, true>                                               \
{                                                                                 \
private:                                                                          \
    using Yes = char[2];                                                          \
    using  No = char[1];                                                          \
                                                                                  \
    struct Fallback { struct Type { }; };                                         \
    struct Derived : T, Fallback { };                                             \
                                                                                  \
    template < class U >                                                          \
    static No& test ( typename U::Type* );                                        \
    template < typename U >                                                       \
    static Yes& test ( U* );                                                      \
                                                                                  \
public:                                                                           \
    static constexpr bool RESULT = sizeof(test<Derived>(nullptr)) == sizeof(Yes); \
};                                                                                \
                                                                                  \
template < class T >                                                              \
struct has_member_type_##Type                                                     \
: public std::integral_constant<bool, HasMemberType_##Type<T>::RESULT>            \
{ };

GENERATE_HAS_MEMBER_TYPE(abstract_type)

namespace neolib
{
struct sfinae {};

template <typename T>
using to_const_reference_t = const std::remove_reference_t<T>&;
template <typename T>
inline to_const_reference_t<T> to_const(T&& object)
{
  return const_cast<to_const_reference_t<T>>(object);
}

template <typename T, typename... Ts>
struct variadic_index;

template <typename T, typename... Ts>
struct variadic_index<T, T, Ts...> : std::integral_constant<std::size_t, 0> {};

template <typename T, typename Tail, typename... Ts>
struct variadic_index<T, Tail, Ts...> : std::integral_constant<std::size_t, 1 + variadic_index<T, Ts...>::value> {};

template <typename T, typename... Ts>
constexpr std::size_t index_of_v = variadic_index<T, Ts...>::value;

template <typename T1, typename T2>
class pair;

namespace detail
{
template <typename T>
struct is_pair { static constexpr bool value = false; };
template <typename T1, typename T2>
struct is_pair<std::pair<T1, T2>> { static constexpr bool value = true; };
template <typename T1, typename T2>
struct is_pair<const std::pair<T1, T2>> { static constexpr bool value = true; };
template <typename T>
constexpr bool is_pair_v = is_pair<T>::value;

template <typename T>
constexpr bool abstract_class_possible_v = std::is_class_v<T> && has_member_type_abstract_type<T>::value;

template <typename T, typename AT, typename = sfinae>
struct correct_const;
template <typename T, typename AT>
struct correct_const<T, AT, typename std::enable_if_t<!std::is_const_v<T>, sfinae>> { typedef AT type; };
template <typename T, typename AT>
struct correct_const<T, AT, typename std::enable_if_t<std::is_const_v<T>, sfinae>> { typedef const AT type; };

template <typename T, typename AT>
using correct_const_t = typename correct_const<T, AT>::type;

template <typename, typename = sfinae>
struct abstract_type : std::false_type {};
template <typename T>
struct abstract_type<T, typename std::enable_if_t<abstract_class_possible_v<T>, sfinae>> : std::true_type { typedef correct_const_t<T, typename T::abstract_type> type; };
template <typename T>
struct abstract_type<T, typename std::enable_if_t<std::is_arithmetic_v<T>, sfinae>> : std::true_type { typedef correct_const_t<T, T> type; };
template <typename T>
struct abstract_type<T, typename std::enable_if_t<std::is_enum_v<T>, sfinae>> : std::true_type { typedef correct_const_t<T, T> type; };
template <typename T>
struct abstract_type<T, typename std::enable_if_t<std::is_pointer_v<T>, sfinae>> : std::true_type { typedef correct_const_t<T, T> type; };
template <typename T1, typename T2>
struct abstract_type<std::pair<T1, pair<T1, T2>>> : std::false_type { typedef typename abstract_type<pair<T1, T2>>::type type; };
template <typename T1, typename T2>
struct abstract_type<const std::pair<T1, pair<T1, T2>>> : std::false_type { typedef typename abstract_type<const pair<T1, T2>>::type type; };
template <>
struct abstract_type<std::monostate> : std::true_type { typedef std::monostate type; };
template <typename Rep, typename Period>
struct abstract_type<std::chrono::duration<Rep, Period>> : std::true_type { typedef std::chrono::duration<Rep, Period> type; };
}

template <typename T>
using abstract_t = typename detail::abstract_type<T>::type;

template <typename T, typename = std::enable_if_t<detail::abstract_type<T>::value, sfinae>>
inline const abstract_t<T>& to_abstract(const T& aArgument)
{
  return static_cast<const abstract_t<T>&>(aArgument);
}

template <typename T, typename = std::enable_if_t<detail::abstract_type<T>::value, sfinae>>
inline abstract_t<T>& to_abstract(T& aArgument)
{
  return static_cast<abstract_t<T>&>(aArgument);
}

template <typename T1, typename T2>
inline const abstract_t<pair<T1, T2>>& to_abstract(const std::pair<T1, pair<T1, T2>>& aArgument)
{
  return static_cast<const abstract_t<pair<T1, T2>>&>(aArgument.second);
}

template <typename T1, typename T2>
inline abstract_t<neolib::pair<T1, T2>>& to_abstract(std::pair<T1, pair<T1, T2>>& aArgument)
{
  return static_cast<abstract_t<pair<T1, T2>>&>(aArgument.second);
}

namespace detail
{
template <typename T, typename = sfinae>
struct abstract_return_type { typedef abstract_t<T>& type; };
template <typename T>
struct abstract_return_type<T, std::enable_if_t<std::is_scalar_v<T>, sfinae>> { typedef std::remove_const_t<T> type; };
}

template <typename T>
using abstract_return_t = typename detail::abstract_return_type<T>::type;
}

#ifdef NEOLIB_HOSTED_ENVIRONMENT

// SIMD support
#ifndef NO_SIMD
#ifndef USE_AVX_DYNAMIC
#define USE_AVX
#endif
#ifndef USE_EMM_DYNAMIC
#define USE_EMM
#endif
#endif

#define USING_BOOST
#define BOOST_ASIO_DISABLE_SMALL_BLOCK_RECYCLING
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING

#ifdef _WIN32
#include <neolib/core/win32/win32.hpp>
#endif

#ifdef USING_BOOST
#ifndef API
#include <boost/dll.hpp>
#define API extern "C" BOOST_SYMBOL_EXPORT
#endif
#endif

#endif // NEOLIB_HOSTED_ENVIRONMENT

#include <memory>
#include <iterator>

#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <vector>

namespace neolib
{
struct vecarray_overflow : public std::exception
{
  virtual const char* what() const throw() { return "neolib::vecarray_overflow"; }
};

struct nocheck
{
  inline static void test(bool aValid)
  {
    (void)aValid;
    assert(aValid);
  }
};

template <typename Exception>
struct check
{
  inline static void test(bool aValid)
  {
    if (!aValid)
      throw Exception();
  }
};

template <typename T>
bool constexpr vecarray_trivial_v = std::is_trivial_v<T>;

namespace detail
{
template <typename InputIter1, typename InputIter2, typename ForwardIter1, typename ForwardIter2>
inline static void uninitialized_copy2(InputIter1 first1, InputIter1 last1, ForwardIter1 dest1, InputIter2 first2, InputIter2 last2, ForwardIter2 dest2)
{
  std::uninitialized_copy(first1, last1, dest1);
  try
  {
    std::uninitialized_copy(first2, last2, dest2);
  }
  catch(...)
  {
    auto last = dest1 + (last1 - first1);
    typedef typename std::iterator_traits<ForwardIter1>::value_type value_type;
    if constexpr (!vecarray_trivial_v<value_type>)
      for (auto i = dest1; i != last; ++i)
        (*i).~value_type();
    throw;
  }
}
}

template<typename T, std::size_t ArraySize, std::size_t MaxVectorSize = ArraySize, typename CheckPolicy = check<vecarray_overflow>, typename Alloc = std::allocator<T> >
class vecarray
{
public:
  struct iterator_invalid : std::logic_error { iterator_invalid() : std::logic_error("neolib::vecarray::iterator_invalid") {} };
private:
  enum iterator_type_e { ArrayIterator, VectorIterator };
public:
  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  typedef Alloc allocator_type;
  typedef std::vector<value_type, allocator_type> vector_type;
  class const_iterator;
  class iterator
  {
    friend class const_iterator;

  public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef vecarray::value_type value_type;
    typedef vecarray::difference_type difference_type;
    typedef vecarray::pointer pointer;
    typedef vecarray::reference reference;

  public:
    iterator() : iType(ArrayIterator), iArrayPtr(0), iVectorIter()
    {
    }
    iterator(const iterator& aOther) : iType(aOther.iType), iArrayPtr(aOther.iArrayPtr), iVectorIter(aOther.iVectorIter)
    {
    }
    iterator(T* aPtr) : iType(ArrayIterator), iArrayPtr(aPtr), iVectorIter()
    {
    }
    iterator(typename vector_type::iterator aIter) : iType(VectorIterator), iArrayPtr(0), iVectorIter(aIter)
    {
    }

  public:
    iterator& operator++()
    {
      if (iType == ArrayIterator)
        ++iArrayPtr;
      else
        ++iVectorIter;
      return *this;
    }
    iterator& operator--()
    {
      if (iType == ArrayIterator)
        --iArrayPtr;
      else
        --iVectorIter;
      return *this;
    }
    iterator operator++(int) { iterator ret(*this); operator++(); return ret; }
    iterator operator--(int) { iterator ret(*this); operator--(); return ret; }
    iterator& operator+=(difference_type aDifference)
    {
      if (aDifference < 0)
        return operator-=(-aDifference);
      if (iType == ArrayIterator)
        iArrayPtr += aDifference;
      else
        iVectorIter += aDifference;
      return *this;
    }
    iterator& operator-=(difference_type aDifference)
    {
      if (aDifference < 0)
        return operator+=(-aDifference);
      if (iType == ArrayIterator)
        iArrayPtr -= aDifference;
      else
        iVectorIter -= aDifference;
      return *this;
    }
    iterator operator+(difference_type aDifference) const { iterator result(*this); result += aDifference; return result; }
    iterator operator-(difference_type aDifference) const { iterator result(*this); result -= aDifference; return result; }
    reference operator[](difference_type aDifference) const { return *((*this) + aDifference); }
    difference_type operator-(const iterator& aOther) const
    {
      if (iType == ArrayIterator)
        return iArrayPtr - aOther.iArrayPtr;
      else
        return iVectorIter - aOther.iVectorIter;
    }
    reference operator*() const
    {
      if (iType == ArrayIterator)
        return *iArrayPtr;
      else
        return *iVectorIter;
    }
    pointer operator->() const { return &operator*(); }
    bool operator==(const iterator& aOther) const
    {
      if (iType == ArrayIterator)
        return iArrayPtr == aOther.iArrayPtr;
      else
        return iVectorIter == aOther.iVectorIter;
    }
    bool operator!=(const iterator& aOther) const
    {
      if (iType == ArrayIterator)
        return iArrayPtr != aOther.iArrayPtr;
      else
        return iVectorIter != aOther.iVectorIter;
    }
    bool operator<(const iterator& aOther) const
    {
      if (iType == ArrayIterator)
        return iArrayPtr < aOther.iArrayPtr;
      else
        return iVectorIter < aOther.iVectorIter;
    }
  public:
    T* array_ptr() const
    {
      if (iType == ArrayIterator)
        return iArrayPtr;
      else
        throw iterator_invalid();
    }
    typename vector_type::iterator vector_iter() const
    {
      if (iType == VectorIterator)
        return iVectorIter;
      else
        throw iterator_invalid();
    }
  private:
    iterator_type_e iType;
    T* iArrayPtr;
    typename vector_type::iterator iVectorIter;
  };
  class const_iterator
  {
  public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef vecarray::value_type value_type;
    typedef vecarray::difference_type difference_type;
    typedef vecarray::const_pointer pointer;
    typedef vecarray::const_reference reference;

  public:
    const_iterator() : iType(ArrayIterator), iArrayPtr(0), iVectorIter()
    {
    }
    const_iterator(const const_iterator& aOther) : iType(aOther.iType), iArrayPtr(aOther.iArrayPtr), iVectorIter(aOther.iVectorIter)
    {
    }
    const_iterator(const typename vecarray::iterator& aOther) : iType(aOther.iType), iArrayPtr(aOther.iArrayPtr), iVectorIter(aOther.iVectorIter)
    {
    }
    const_iterator(const T* aPtr) : iType(ArrayIterator), iArrayPtr(aPtr), iVectorIter()
    {
    }
    const_iterator(typename vector_type::const_iterator aIter) : iType(VectorIterator), iArrayPtr(0), iVectorIter(aIter)
    {
    }
    const_iterator(typename vector_type::iterator aIter) : iType(VectorIterator), iArrayPtr(0), iVectorIter(aIter)
    {
    }

  public:
    const_iterator& operator++()
    {
      if (iType == ArrayIterator)
        ++iArrayPtr;
      else
        ++iVectorIter;
      return *this;
    }
    const_iterator& operator--()
    {
      if (iType == ArrayIterator)
        --iArrayPtr;
      else
        --iVectorIter;
      return *this;
    }
    const_iterator operator++(int) { const_iterator ret(*this); operator++(); return ret; }
    const_iterator operator--(int) { const_iterator ret(*this); operator--(); return ret; }
    const_iterator& operator+=(difference_type aDifference)
    {
      if (aDifference < 0)
        return operator-=(-aDifference);
      if (iType == ArrayIterator)
        iArrayPtr += aDifference;
      else
        iVectorIter += aDifference;
      return *this;
    }
    const_iterator& operator-=(difference_type aDifference)
    {
      if (aDifference < 0)
        return operator+=(-aDifference);
      if (iType == ArrayIterator)
        iArrayPtr -= aDifference;
      else
        iVectorIter -= aDifference;
      return *this;
    }
    const_iterator operator+(difference_type aDifference) const { const_iterator result(*this); result += aDifference; return result; }
    const_iterator operator-(difference_type aDifference) const { const_iterator result(*this); result -= aDifference; return result; }
    const_reference operator[](difference_type aDifference) const { return *((*this) + aDifference); }
    difference_type operator-(const const_iterator& aOther) const
    {
      if (iType == ArrayIterator)
        return iArrayPtr - aOther.iArrayPtr;
      else
        return iVectorIter - aOther.iVectorIter;
    }
    const_reference operator*() const
    {
      if (iType == ArrayIterator)
        return *iArrayPtr;
      else
        return *iVectorIter;
    }
    const_pointer operator->() const { return &operator*(); }
    bool operator==(const const_iterator& aOther) const
    {
      if (iType == ArrayIterator)
        return iArrayPtr == aOther.iArrayPtr;
      else
        return iVectorIter == aOther.iVectorIter;
    }
    bool operator!=(const const_iterator& aOther) const
    {
      if (iType == ArrayIterator)
        return iArrayPtr != aOther.iArrayPtr;
      else
        return iVectorIter != aOther.iVectorIter;
    }
    bool operator<(const const_iterator& aOther) const
    {
      if (iType == ArrayIterator)
        return iArrayPtr < aOther.iArrayPtr;
      else
        return iVectorIter < aOther.iVectorIter;
    }

  public:
    const T* array_ptr() const
    {
      if (iType == ArrayIterator)
        return iArrayPtr;
      else
        throw iterator_invalid();
    }
    typename vector_type::const_iterator vector_iter() const
    {
      if (iType == VectorIterator)
        return iVectorIter;
      else
        throw iterator_invalid();
    }

  private:
    iterator_type_e iType;
    const T* iArrayPtr;
    typename vector_type::const_iterator iVectorIter;
  };
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

public:
  static constexpr bool is_fixed_size() { return ArraySize == MaxVectorSize; }

public:
  // construction
  vecarray() : iSize{ 0 }
  {
  }
  vecarray(const vecarray& rhs) : iSize{ 0 }
  {
    insert(begin(), rhs.begin(), rhs.end());
  }
  vecarray(vecarray&& rhs) : iSize{0}
  {
    if (using_vector())
    {
      vector() = std::move(rhs.vector());
      iSize = rhs.iSize;
    }
    else
    {
      for (auto&& element : rhs)
        push_back(std::move(element));
      rhs.clear();
    }
  }
  template <typename T2, std::size_t N2>
  vecarray(const vecarray<T2, N2>& rhs) : iSize{ 0 }
  {
    insert(begin(), rhs.begin(), rhs.end());
  }
  vecarray(size_type n) : iSize{ 0 }
  {
    insert(begin(), n, value_type());
  }
  vecarray(size_type n, value_type value) : iSize{ 0 }
  {
    insert(begin(), n, value);
  }
  template<typename InputIterator>
  vecarray(InputIterator first, InputIterator last) : iSize{ 0 }
  {
    insert(begin(), first, last);
  }
  vecarray(std::initializer_list<T> init) : iSize{ 0 }
  {
    insert(begin(), init.begin(), init.end());
  }
  ~vecarray()
  {
    clear();
    if (using_vector())
      vector().~vector_type();
  }

  // traversals
public:
  const_iterator cbegin() const { return using_array() ? const_iterator(reinterpret_cast<const_pointer>(iAlignedBuffer.iData)) : const_iterator(vector().begin()); }
  const_iterator begin() const { return cbegin(); }
  iterator begin() { return using_array() ? iterator(reinterpret_cast<pointer>(iAlignedBuffer.iData)) : iterator(vector().begin()); }
  const_iterator cend() const { return using_array() ? const_iterator(reinterpret_cast<const_pointer>(iAlignedBuffer.iData) + iSize) : const_iterator(vector().end()); }
  const_iterator end() const { return cend(); }
  iterator end() { return using_array() ? iterator(reinterpret_cast<pointer>(iAlignedBuffer.iData) + iSize) : iterator(vector().end()); }
  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
  bool empty() const { return size() == 0; }
  bool full() const { return size() == MaxVectorSize; }
  size_type size() const { return using_array() ? iSize : vector().size(); }
  size_type available() const { return MaxVectorSize - size(); }
  size_type capacity() const { return MaxVectorSize; }
  size_type max_size() const { return MaxVectorSize; }
  size_type after(size_type position) const { return position < size() ? size() - position : 0; }

  // element access
public:
  reference operator[](size_type n) { return *(begin() + n); }
  const_reference operator[](size_type n) const { return *(begin() + n); }
  reference at(size_type n) { if (n < size()) return operator[](n); throw std::out_of_range("vecarray::at"); }
  const_reference at(size_type n) const { if (n < size()) return operator[](n); throw std::out_of_range("vecarray::at"); }
  reference front() { return *begin(); }
  reference back() { return *(begin() + (size() - 1)); }
  const_reference front() const { return *begin(); }
  const_reference back() const { return *(begin() + (size() - 1)); }

  // modifiers
public:
  vecarray& operator=(const vecarray& rhs)
  {
    if (&rhs != this)
      assign(rhs.begin(), rhs.end());
    return *this;
  }
  template<typename T2, std::size_t N2>
  vecarray& operator=(const vecarray<T2, N2>& rhs)
  {
    if (static_cast<const void*>(&rhs) != static_cast<const void*>(this))
      assign(rhs.begin(), rhs.end());
    return *this;
  }
  template <typename InputIterator>
  void assign(InputIterator first, InputIterator last)
  {
    clear();
    insert(cbegin(), first, last);
  }
  void assign(size_type n, value_type value)
  {
    clear();
    insert(cbegin(), n, value);
  }
  template <class InputIterator>
  typename std::enable_if_t<!std::is_integral<InputIterator>::value, iterator>
  insert(const_iterator position, InputIterator first, InputIterator last)
  {
    need(std::distance(first, last), position);
    position = do_insert(position, first, last);
    return begin() + (position - cbegin());
  }
  iterator insert(const_iterator position, value_type value)
  {
    need(1, position);
    insert(position, 1, value);
    return begin() + (position - cbegin());
  }
  iterator insert(const_iterator position, size_type count, const value_type& value)
  {
    need(count, position);
    if (using_array())
    {
      iterator next = std::next(begin(), std::distance(cbegin(), position));
      while (count > 0)
      {
        next = std::next(insert(next, &value, &value + 1));
        --count;
      }
      return next;
    }
    else
      return vector().insert(position.vector_iter(), count, value);
  }
  iterator erase(const_iterator position)
  {
    if (using_array())
    {
      assert(iSize > 0);
      iterator dest = std::next(begin(), std::distance(cbegin(), position));
      auto garbage = std::copy(std::make_move_iterator(std::next(dest)), std::make_move_iterator(end()), dest);
      (*garbage).~value_type();
      --iSize;
      return dest;
    }
    else
      return vector().erase(position.vector_iter());
  }
  iterator erase(const_iterator first, const_iterator last)
  {
    if (first == last)
      return std::next(begin(), std::distance(cbegin(), first));
    if (using_array())
    {
      assert(iSize > 0);
      iterator first2 = std::next(begin(), std::distance(cbegin(), first));
      iterator last2 = std::next(begin(), std::distance(cbegin(), last));
      auto garbage = std::copy(std::make_move_iterator(last2), std::make_move_iterator(end()), first2);
      auto garbageLast = end();
      if constexpr (!vecarray_trivial_v<value_type>)
        for (auto i = garbage; i != garbageLast; ++i)
          (*i).~value_type();
      iSize -= (last - first);
      return first2;
    }
    else
      return vector().erase(first.vector_iter(), last.vector_iter());
  }
  void clear()
  {
    erase(cbegin(), cend());
  }
  template< class... Args >
  reference emplace_back(Args&&... args)
  {
    CheckPolicy::test(size() < MaxVectorSize);
    need(1);
    if (using_array())
    {
      new (end().array_ptr()) value_type{ std::forward<Args>(args)... };
      ++iSize;
      return back();
    }
    else
      return vector().emplace_back(std::forward<Args>(args)...);
  }
  void push_back(const value_type& value)
  {
    CheckPolicy::test(size() < MaxVectorSize);
    need(1);
    if (using_array())
    {
      new (end().array_ptr()) value_type{ value };
      ++iSize;
    }
    else
      vector().push_back(value);
  }
  void push_back(value_type&& value)
  {
    CheckPolicy::test(size() < MaxVectorSize);
    need(1);
    if (using_array())
    {
      new (end().array_ptr()) value_type{ std::move(value) };
      ++iSize;
    }
    else
      vector().push_back(std::move(value));
  }
  void pop_back()
  {
    erase(end() - 1);
  }
  void remove(value_type value, bool multiple = true)
  {
    auto last = end();
    for (iterator i = begin(); i != last; )
    {
      if (*i == value)
      {
        erase(i);
        if (!multiple)
          return;
      }
      else
        ++i;
    }
  }
  void resize(size_type n)
  {
    if (using_array())
    {
      if (size() > n)
        erase(begin() + n, end());
      else if (size() < n)
        insert(end(), n - size(), value_type{});
    }
    else
      vector().resize(n);
  }
  void resize(size_type n, const value_type& value)
  {
    if (using_array())
    {
      if (size() > n)
        erase(begin() + n, end());
      else if (size() < n)
        insert(end(), n - size(), value);
    }
    else
      vector().resize(n, value);
  }
  void reserve(size_type n)
  {
    if (n > ArraySize && n > size())
      need(n - size());
  }
  template<typename T2, std::size_t ArraySize2, std::size_t MaxVectorSize2, typename CheckPolicy2, typename Alloc2>
  void swap(vecarray<T2, ArraySize2, MaxVectorSize2, CheckPolicy2, Alloc2>& rhs)
  {
    vecarray tmp = rhs;
    rhs = *this;
    *this = tmp;
  }

  // equality
public:
  template<typename T2, std::size_t ArraySize2, std::size_t MaxVectorSize2, typename CheckPolicy2, typename Alloc2>
  bool operator==(const vecarray<T2, ArraySize2, MaxVectorSize2, CheckPolicy2, Alloc2>& rhs) const
  {
    return rhs.size() == size() && std::equal(begin(), end(), rhs.begin());
  }
  template<typename T2, std::size_t ArraySize2, std::size_t MaxVectorSize2, typename CheckPolicy2, typename Alloc2>
  bool operator!=(const vecarray<T2, ArraySize2, MaxVectorSize2, CheckPolicy2, Alloc2>& rhs) const
  {
    return !operator==(rhs);
  }

private:
  template <std::size_t ArraySize2 = ArraySize, std::size_t MaxVectorSize2 = MaxVectorSize>
  constexpr typename std::enable_if<ArraySize2 == MaxVectorSize2, bool>::type using_array() const
  {
    return true;
  }
  template <std::size_t ArraySize2 = ArraySize, std::size_t MaxVectorSize2 = MaxVectorSize>
  typename std::enable_if<ArraySize2 != MaxVectorSize2, bool>::type using_array() const
  {
    return iSize != USING_VECTOR;
  }
  template <std::size_t ArraySize2 = ArraySize, std::size_t MaxVectorSize2 = MaxVectorSize>
  constexpr typename std::enable_if<ArraySize2 == MaxVectorSize2, bool>::type using_vector() const
  {
    return false;
  }
  template <std::size_t ArraySize2 = ArraySize, std::size_t MaxVectorSize2 = MaxVectorSize>
  typename std::enable_if<ArraySize2 != MaxVectorSize2, bool>::type using_vector() const
  {
    return iSize == USING_VECTOR;
  }
  vector_type& vector()
  {
    return reinterpret_cast<vector_type&>(iAlignedBuffer.iVector);
  }
  const vector_type& vector() const
  {
    return reinterpret_cast<const vector_type&>(iAlignedBuffer.iVector);
  }
  void need(size_type aAmount)
  {
    if (using_array() && size() + aAmount > ArraySize)
      convert(aAmount);
  }
  template <typename Iter>
  void need(size_type aAmount, Iter& aIter)
  {
    static_assert(std::is_same_v<Iter, const_iterator> || std::is_same_v<Iter, iterator>, "bad usage");
    if (using_array() && size() + aAmount > ArraySize)
    {

      if constexpr (std::is_same_v<Iter, const_iterator>)
      {
        size_type index = aIter - cbegin();
        convert(aAmount);
        aIter = cbegin() + index;
      }
      else if constexpr (std::is_same_v<Iter, iterator>)
      {
        size_type index = aIter - begin();
        convert(aAmount);
        aIter = begin() + index;
      }
    }
  }
  void convert(size_type aExtra)
  {
    if (using_array())
    {
      vector_type copy;
      copy.reserve(std::max(ArraySize * 2, size() + aExtra));
      copy.insert(copy.begin(), std::make_move_iterator(begin()), std::make_move_iterator(end()));
      clear();
      new (iAlignedBuffer.iVector) vector_type{ std::move(copy) };
      iSize = USING_VECTOR;
    }
  }
  template <class InputIterator>
  typename std::enable_if<!std::is_same<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value, iterator>::type
  do_insert(const_iterator position, InputIterator first, InputIterator last)
  {
    difference_type n = last - first;
    CheckPolicy::test(size() + n <= MaxVectorSize);
    need(n, position);
    if (using_array())
    {
      auto pos = const_cast<pointer>(position.array_ptr());
      const_iterator theEnd = end();
      difference_type t = theEnd - position;
      if (t > 0)
      {
        if (t > n)
        {
          std::uninitialized_copy(theEnd - n, theEnd, const_cast<pointer>(theEnd.array_ptr()));
          iSize += n;
          std::copy_backward(position, theEnd - n, const_cast<pointer>(theEnd.array_ptr()));
          std::copy(first, last, pos);
        }
        else
        {
          detail::uninitialized_copy2(theEnd - t, theEnd, const_cast<pointer>((theEnd + (n - t)).array_ptr()), first + t, last, const_cast<pointer>(theEnd.array_ptr()));
          iSize += n;
          std::copy(first, first + t, pos);
        }
      }
      else
      {
        std::uninitialized_copy(first, last, pos);
        iSize += n;
      }
      return pos;
    }
    else
    {
      return vector().insert(position.vector_iter(), first, last);
    }
  }
  template <class InputIterator>
  typename std::enable_if<std::is_same<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value, iterator>::type
  do_insert(const_iterator position, InputIterator first, InputIterator last)
  {
    iterator next = position;
    while (first != last)
    {
      next = std::next(insert(next, 1, *first++));
    }
    return next;
  }

private:
  union
  {
    alignas(T) char iData[sizeof(T) * ArraySize];
    char iVector[sizeof(vector_type)];
  } iAlignedBuffer;
  size_type iSize;
  static const size_type USING_VECTOR = static_cast<size_type>(-1);
};
}

namespace neolib
{
template <typename Alloc>
class array_tree
{
public:
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  typedef Alloc allocator_type;
protected:
  class node
  {
    friend array_tree;

  public:
    struct no_left_node : std::logic_error { no_left_node() : std::logic_error("neolib::array_tree::node::no_left_node") {} };
    struct no_right_node : std::logic_error { no_right_node() : std::logic_error("neolib::array_tree::node::no_right_node") {} };

  public:
    enum color_e
    {
      NIL,
      BLACK,
      RED
    };

  public:
    node(color_e aColor = RED) :
        iColor{ aColor }, iParent{ aColor != NIL ? nullptr : this }, iLeft{ aColor != NIL ? nullptr : this }, iRight{ aColor != NIL ? nullptr : this }, iPrevious{ nullptr }, iNext{ nullptr }, iSize { 0 }
    {
    }
    node(const node& aOther) :
        iColor{ aOther.iColor }, iParent{ aOther.iColor != NIL ? nullptr : this }, iLeft{ aOther.iColor != NIL ? nullptr : this }, iRight{ aOther.iColor != NIL ? nullptr : this }, iPrevious{ nullptr }, iNext{ nullptr }, iSize{ 0 }
    {
    }
    ~node()
    {
    }

  public:
    bool is_nil() const
    {
      return iColor == NIL;
    }
    color_e color() const
    {
      return iColor != NIL ? iColor : BLACK;
    }
    void set_color(color_e aColor)
    {
      if (iColor != NIL)
        iColor = aColor;
    }
    node* parent() const
    {
      return iParent;
    }
    void set_parent(node* aParent)
    {
      iParent = aParent;
    }
    node* left() const
    {
      if (iLeft != nullptr)
        return iLeft;
      throw no_left_node();
    }
    void set_left(node* aLeft)
    {
      iLeft = aLeft;
    }
    node* right() const
    {
      if (iRight != nullptr)
        return iRight;
      throw no_right_node();
    }
    void set_right(node* aRight)
    {
      iRight = aRight;
    }
    node* previous() const
    {
      return iPrevious;
    }
    void set_previous(node* aPrevoius)
    {
      iPrevious = aPrevoius;
    }
    node* next() const
    {
      return iNext;
    }
    void set_next(node* aNext)
    {
      iNext = aNext;
    }
    size_type size() const
    {
      return iColor != NIL ? iSize : 0;
    }
    size_type left_size() const
    {
      return left() ? left()->size() : 0;
    }
    size_type right_size() const
    {
      return right() ? right()->size() : 0;
    }
    void set_size(size_type aSize)
    {
      if (!is_nil())
      {
        difference_type difference = aSize - iSize;
        if (difference != 0)
        {
          iSize += difference;
          if (parent() != nullptr && !parent()->is_nil())
            parent()->set_size(parent()->size() + difference);
        }
      }
    }
    void replace(node* aGarbage, node* aNil)
    {
      set_color(aGarbage->color());
      set_parent(aGarbage->parent());
      set_left(aGarbage->left());
      set_right(aGarbage->right());
      if (parent()->left() == aGarbage)
        parent()->set_left(this);
      else if (parent()->right() == aGarbage)
        parent()->set_right(this);
      if (!left()->is_nil())
        left()->set_parent(this);
      if (!right()->is_nil())
        right()->set_parent(this);
      aGarbage->set_parent(nullptr);
      aGarbage->set_left(nullptr);
      aGarbage->set_right(nullptr);
      if (aNil->parent() == aGarbage)
        aNil->set_parent(this);
      if (aNil->left() == aGarbage)
        aNil->set_left(this);
      if (aNil->right() == aGarbage)
        aNil->set_right(this);
    }

  private:
    color_e iColor;
    node* iParent;
    node* iLeft;
    node* iRight;
    node* iPrevious;
    node* iNext;
    size_type iSize;
  };
private:
  typedef typename std::allocator_traits<allocator_type>:: template rebind_alloc<node> node_allocator_type;

public:
  array_tree(const Alloc& aAllocator = Alloc()) :
      iAllocator{ aAllocator },
      iRoot{ nullptr },
      iFront{ nullptr },
      iBack{ nullptr },
      iNil{ nullptr }
  {
    iNil = std::allocator_traits<node_allocator_type>::allocate(iAllocator, 1);
    try
    {
      std::allocator_traits<node_allocator_type>::construct(iAllocator, iNil, node(node::NIL));
    }
    catch (...)
    {
      std::allocator_traits<node_allocator_type>::deallocate(iAllocator, iNil, 1);
      throw;
    }
    set_root_node(iNil);
  }
  array_tree(const array_tree& aOther) :
      iAllocator{ aOther.iAllocator },
      iRoot{ nullptr },
      iFront{ nullptr },
      iBack{ nullptr },
      iNil{ nullptr }
  {
    iNil = std::allocator_traits<node_allocator_type>::allocate(iAllocator, 1);
    try
    {
      std::allocator_traits<node_allocator_type>::construct(iAllocator, iNil, node(node::NIL));
    }
    catch (...)
    {
      std::allocator_traits<node_allocator_type>::deallocate(iAllocator, iNil, 1);
      throw;
    }
    set_root_node(iNil);
  }
  array_tree(array_tree&& other) :
      iAllocator{ std::move(other.iAllocator) },
      iRoot{ nullptr },
      iFront{ nullptr },
      iBack{ nullptr },
      iNil{ nullptr }
  {
    std::swap(iRoot, other.iRoot);
    std::swap(iFront, other.iFront);
    std::swap(iBack, other.iBack);
    std::swap(iNil, other.iNil);
  }
  ~array_tree()
  {
    std::allocator_traits<node_allocator_type>::destroy(iAllocator, iNil);
    std::allocator_traits<node_allocator_type>::deallocate(iAllocator, iNil, 1);
  }

public:
  node* nil_node() const
  {
    return iNil;
  }
  node* root_node() const
  {
    return iRoot;
  }
  void set_root_node(node* aRoot)
  {
    iRoot = aRoot;
  }
  node* front_node() const
  {
    return iFront;
  }
  void set_front_node(node* aFront)
  {
    iFront = aFront;
  }
  node* back_node() const
  {
    return iBack;
  }
  void set_back_node(node* aBack)
  {
    iBack = aBack;
  }
  static size_type size(node* aNode)
  {
    return aNode ? aNode->size() : 0;
  }
  static size_type size_parent(node* aNode)
  {
    return aNode && aNode->parent() ? aNode->parent()->size() : 0;
  }
  static size_type size_left(node* aNode)
  {
    return aNode && aNode->left() ? aNode->left()->size() : 0;
  }
  static size_type size_right(node* aNode)
  {
    return aNode && aNode->right() ? aNode->right()->size() : 0;
  }
  node* find_node(size_type aPosition, size_type& aNodeIndex) const
  {
    node* x = root_node();
    size_type index = size_left(x);
    while (x != nil_node() && (aPosition < index || aPosition >= index + (size(x) - size_left(x) - size_right(x))))
    {
      if (aPosition < index)
      {
        x = x->left();
        index -= (size(x) - size_left(x));
      }
      else
      {
        index += size(x) - size_left(x) - size_right(x) + size_left(x->right());
        x = x->right();
      }
    }
    aNodeIndex = index;
    return x;
  }
  void insert_node(node* aNode, size_type aPosition)
  {
    node* z = aNode;
    node* y = nil_node();
    node* x = root_node();
    size_type index = size_left(x);
    size_type previousIndex = index;
    while (x != nil_node())
    {
      previousIndex = index;
      y = x;
      if (aPosition <= index)
      {
        x = x->left();
        index -= (size(x) - size_left(x));
      }
      else
      {
        index += size(x) - size_left(x) - size_right(x) + size_left(x->right());
        x = x->right();
      }
    }
    z->set_parent(y);
    if (y == nil_node())
      set_root_node(z);
    else
    {
      if (aPosition <= previousIndex)
        y->set_left(z);
      else
        y->set_right(z);
    }
    z->set_left(nil_node());
    z->set_right(nil_node());
    if (z->parent() != nil_node())
      z->parent()->set_size(z->parent()->size() + z->size());
    insert_fixup(z);
  }
  void delete_node(node* aNode)
  {
    node* z = aNode;
    z->set_size(z->left_size() + z->right_size());
    node *y;
    if (z->left() == nil_node() || z->right() == nil_node())
      y = z;
    else
      y = tree_successor(z);
    node* x;
    if (y->left() != nil_node())
      x = y->left();
    else
      x = y->right();
    if (y != z)
    {
      y->parent()->set_size(y->parent()->size() - y->size());
      y->parent()->set_size(y->parent()->size() + x->size());
    }
    x->set_parent(y->parent());
    if (y->parent() == nil_node())
      set_root_node(x);
    else
    {
      if (y == y->parent()->left())
        y->parent()->set_left(x);
      else
        y->parent()->set_right(x);
    }
    bool performDeleteFixup = (y->color() == node::BLACK);
    if (y != z)
    {
      z->parent()->set_size(z->parent()->size() - z->size());
      y->iSize = y->size() - size_left(y) - size_right(y);
      y->replace(z, nil_node());
      if (root_node() == z)
        set_root_node(y);
      y->iSize = y->size() + size_left(y) + size_right(y);
      y->parent()->set_size(y->parent()->size() + y->size());
    }
    if (performDeleteFixup)
      delete_fixup(x);
  }
  void swap(array_tree& aOther)
  {
    std::swap(iAllocator, aOther.iAllocator);
    std::swap(iRoot, aOther.iRoot);
    std::swap(iFront, aOther.iFront);
    std::swap(iBack, aOther.iBack);
    std::swap(iNil, aOther.iNil);
  }

private:
  void insert_fixup(node* aNode)
  {
    node* z = aNode;
    while (z->parent()->color() == node::RED)
    {
      if (z->parent() == z->parent()->parent()->left())
      {
        node* y = z->parent()->parent()->right();
        if (y->color() == node::RED)
        {
          z->parent()->set_color(node::BLACK);
          y->set_color(node::BLACK);
          z->parent()->parent()->set_color(node::RED);
          z = z->parent()->parent();
        }
        else
        {
          if (z == z->parent()->right())
          {
            z = z->parent();
            left_rotate(z);
          }
          z->parent()->set_color(node::BLACK);
          z->parent()->parent()->set_color(node::RED);
          right_rotate(z->parent()->parent());
        }
      }
      else
      {
        node* y = z->parent()->parent()->left();
        if (y->color() == node::RED)
        {
          z->parent()->set_color(node::BLACK);
          y->set_color(node::BLACK);
          z->parent()->parent()->set_color(node::RED);
          z = z->parent()->parent();
        }
        else
        {
          if (z == z->parent()->left())
          {
            z = z->parent();
            right_rotate(z);
          }
          z->parent()->set_color(node::BLACK);
          z->parent()->parent()->set_color(node::RED);
          left_rotate(z->parent()->parent());
        }
      }
    }
    root_node()->set_color(node::BLACK);
  }
  void left_rotate(node* aNode)
  {
    node* x = aNode;
    node* y = x->right();
    x->set_right(y->left());
    if (y->left() != nil_node())
      y->left()->set_parent(x);
    y->set_parent(x->parent());
    if (x->parent() == nil_node())
      set_root_node(y);
    else
    {
      if (x == x->parent()->left())
        x->parent()->set_left(y);
      else
        x->parent()->set_right(y);
    }
    y->set_left(x);
    x->set_parent(y);
    size_type previousSize = y->size();
    /* do not use set_size() as we don't want to propagate to ancestors */
    y->iSize = x->size();
    x->iSize -= previousSize;
    x->iSize += x->right()->size();
  }
  void right_rotate(node* aNode)
  {
    node* y = aNode;
    node* x = y->left();
    y->set_left(x->right());
    if (x->right() != nil_node())
      x->right()->set_parent(y);
    x->set_parent(y->parent());
    if (y->parent() == nil_node())
      set_root_node(x);
    else
    {
      if (y == y->parent()->right())
        y->parent()->set_right(x);
      else
        y->parent()->set_left(x);
    }
    x->set_right(y);
    y->set_parent(x);
    size_type previousSize = x->size();
    /* do not use set_size() as we don't want to propagate to ancestors */
    x->iSize = y->size();
    y->iSize -= previousSize;
    y->iSize += y->left()->size();
  }
  node* tree_minimum(node* aNode)
  {
    node* x = aNode;
    while (x->left() != nil_node())
      x = x->left();
    return x;
  }
  node* tree_successor(node* aNode)
  {
    node* x = aNode;
    if (x->right() != nil_node())
      return tree_minimum(x->right());
    node* y = x->parent();
    while (y != nil_node() && x == y->right())
    {
      x = y;
      y = y->parent();
    }
    return y;
  }
  void delete_fixup(node* aNode)
  {
    node* x = aNode;
    while (x != root_node() && x->color() == node::BLACK)
    {
      if (x == x->parent()->left())
      {
        node* w = x->parent()->right();
        if (w->color() == node::RED)
        {
          w->set_color(node::BLACK);
          x->parent()->set_color(node::RED);
          left_rotate(x->parent());
          w = x->parent()->right();
        }
        if (w->left()->color() == node::BLACK && w->right()->color() == node::BLACK)
        {
          w->set_color(node::RED);
          x = x->parent();
        }
        else
        {
          if (w->right()->color() == node::BLACK)
          {
            w->left()->set_color(node::BLACK);
            w->set_color(node::RED);
            right_rotate(w);
            w = x->parent()->right();
          }
          w->set_color(x->parent()->color());
          x->parent()->set_color(node::BLACK);
          w->right()->set_color(node::BLACK);
          left_rotate(x->parent());
          x = root_node();
        }
      }
      else
      {
        node* w = x->parent()->left();
        if (w->color() == node::RED)
        {
          w->set_color(node::BLACK);
          x->parent()->set_color(node::RED);
          right_rotate(x->parent());
          w = x->parent()->left();
        }
        if (w->right()->color() == node::BLACK && w->left()->color() == node::BLACK)
        {
          w->set_color(node::RED);
          x = x->parent();
        }
        else
        {
          if (w->left()->color() == node::BLACK)
          {
            w->right()->set_color(node::BLACK);
            w->set_color(node::RED);
            left_rotate(w);
            w = x->parent()->left();
          }
          w->set_color(x->parent()->color());
          x->parent()->set_color(node::BLACK);
          w->left()->set_color(node::BLACK);
          right_rotate(x->parent());
          x = root_node();
        }
      }
    }
    x->set_color(node::BLACK);
  }

private:
  node_allocator_type iAllocator;
  node* iRoot;
  node* iFront;
  node* iBack;
  node* iNil;
};
}

namespace neolib
{
template <typename T, std::size_t SegmentSize = 64, typename Alloc = std::allocator<T> >
class segmented_array : private array_tree<Alloc>
{
  typedef segmented_array<T, SegmentSize, Alloc> self_type;
  typedef array_tree<Alloc> base_type;
public:
  typedef T value_type;
  typedef Alloc allocator_type;
  typedef value_type& reference;
  typedef value_type const& const_reference;
  typedef typename std::allocator_traits<allocator_type>::pointer pointer;
  typedef typename std::allocator_traits<allocator_type>::const_pointer const_pointer;
  typedef typename std::allocator_traits<allocator_type>::size_type size_type;
  typedef typename std::allocator_traits<allocator_type>::difference_type difference_type;
private:
  typedef neolib::vecarray<T, SegmentSize, SegmentSize, neolib::nocheck> segment_type;
  class node : public base_type::node
  {
  public:
    node()
    {
    }

  public:
    const segment_type& segment() const
    {
      return iSegment;
    }
    segment_type& segment()
    {
      return iSegment;
    }

  private:
    segment_type iSegment;
  };
  typedef typename std::allocator_traits<allocator_type>:: template rebind_alloc<node> node_allocator_type;
public:
  class iterator
  {
    friend class segmented_array;
    friend class segmented_array::const_iterator;

  public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef typename segmented_array ::value_type value_type;
    typedef typename segmented_array ::difference_type difference_type;
    typedef typename segmented_array ::pointer pointer;
    typedef typename segmented_array ::size_type size_type;
    typedef typename segmented_array ::reference reference;

  public:
    iterator() :
        iContainer(), iNode(), iContainerPosition(), iSegmentPosition()
    {
    }
    iterator(const iterator& aOther) :
        iContainer(aOther.iContainer), iNode(aOther.iNode), iContainerPosition(aOther.iContainerPosition), iSegmentPosition(aOther.iSegmentPosition)
    {
    }
    iterator& operator=(const iterator& aOther)
    {
      iContainer = aOther.iContainer;
      iNode = aOther.iNode;
      iContainerPosition = aOther.iContainerPosition;
      iSegmentPosition = aOther.iSegmentPosition;
      return *this;
    }
  private:
    iterator(segmented_array& aContainer, size_type aContainerPosition) :
        iContainer(&aContainer), iNode(nullptr), iContainerPosition(aContainerPosition), iSegmentPosition(0)
    {
      iNode = iContainer->find_node(aContainerPosition, iSegmentPosition);
      if (iNode->is_nil())
        *this = iContainer->end();
    }
    iterator(segmented_array& aContainer, node* aNode, size_type aContainerPosition, size_type aSegmentPosition) :
        iContainer(&aContainer), iNode(aNode), iContainerPosition(aContainerPosition), iSegmentPosition(aSegmentPosition)
    {
    }

  public:
    iterator& operator++()
    {
      ++iContainerPosition;
      if (++iSegmentPosition == static_cast<node*>(iNode)->segment().size())
      {
        if (iNode != iContainer->back_node())
        {
          iNode = static_cast<node*>(iNode->next());
          iSegmentPosition = 0;
        }
      }
      return *this;
    }
#ifndef _MSC_VER // Internal Compiler Error (VS2022)
    iterator& operator--()
            {
                --iContainerPosition;
                if (iSegmentPosition-- == 0)
                {
                    iNode = static_cast<node*>(iNode->previous());
                    iSegmentPosition = static_cast<node*>(iNode)->segment().size() - 1;
                }
                return *this;
            }
#else // Internal Compiler Error (VS2022) workaround
    iterator& operator--()
    {
      --iContainerPosition;
      if (iSegmentPosition == 0)
      {
        iNode = static_cast<node*>(iNode->previous());
        iSegmentPosition = static_cast<node*>(iNode)->segment().size() - 1;
      }
      else
        --iSegmentPosition;
      return *this;
    }
#endif
    iterator operator++(int) { iterator ret(*this); operator++(); return ret; }
    iterator operator--(int) { iterator ret(*this); operator--(); return ret; }
    iterator& operator+=(difference_type aDifference)
    {
      if (aDifference < 0)
        return operator-=(-aDifference);
      else if (iNode == nullptr || aDifference >= static_cast<difference_type>(segment().size() - iSegmentPosition))
        *this = iterator(*iContainer, iContainerPosition + aDifference);
      else
      {
        iContainerPosition += aDifference;
        iSegmentPosition += aDifference;
      }
      return *this;
    }
    iterator& operator-=(difference_type aDifference)
    {
      if (aDifference < 0)
        return operator+=(-aDifference);
      else if (aDifference > static_cast<difference_type>(iSegmentPosition))
        *this = iterator(*iContainer, iContainerPosition - aDifference);
      else
      {
        iContainerPosition -= aDifference;
        iSegmentPosition -= aDifference;
      }
      return *this;
    }
    iterator operator+(difference_type aDifference) const { iterator result(*this); result += aDifference; return result; }
    iterator operator-(difference_type aDifference) const { iterator result(*this); result -= aDifference; return result; }
    reference operator[](difference_type aDifference) const { return *((*this) + aDifference); }
    difference_type operator-(const iterator& aOther) const { return static_cast<difference_type>(iContainerPosition)-static_cast<difference_type>(aOther.iContainerPosition); }
    reference operator*() const { return segment()[iSegmentPosition]; }
    pointer operator->() const { return &operator*(); }
    bool operator==(const iterator& aOther) const { return iContainerPosition == aOther.iContainerPosition; }
    bool operator!=(const iterator& aOther) const { return iContainerPosition != aOther.iContainerPosition; }
    bool operator<(const iterator& aOther) const { return iContainerPosition < aOther.iContainerPosition; }
    bool operator<=(const iterator& aOther) const { return iContainerPosition <= aOther.iContainerPosition; }
    bool operator>(const iterator& aOther) const { return iContainerPosition > aOther.iContainerPosition; }
    bool operator>=(const iterator& aOther) const { return iContainerPosition >= aOther.iContainerPosition; }

  private:
    segment_type& segment() const { return iNode->segment(); }

  private:
    segmented_array* iContainer;
    node* iNode;
    size_type iContainerPosition;
    size_type iSegmentPosition;
  };
  class const_iterator
  {
    friend class segmented_array;

  public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef typename segmented_array::value_type value_type;
    typedef typename segmented_array::difference_type difference_type;
    typedef typename segmented_array::const_pointer pointer;
    typedef typename segmented_array::size_type size_type;
    typedef typename segmented_array::const_reference reference;

  public:
    const_iterator() :
        iContainer(), iNode(), iContainerPosition(), iSegmentPosition()
    {
    }
    const_iterator(const const_iterator& aOther) :
        iContainer(aOther.iContainer), iNode(aOther.iNode), iContainerPosition(aOther.iContainerPosition), iSegmentPosition(aOther.iSegmentPosition)
    {
    }
    const_iterator(const typename segmented_array::iterator& aOther) :
        iContainer(aOther.iContainer), iNode(aOther.iNode), iContainerPosition(aOther.iContainerPosition), iSegmentPosition(aOther.iSegmentPosition)
    {
    }
    const_iterator& operator=(const const_iterator& aOther)
    {
      iContainer = aOther.iContainer;
      iNode = aOther.iNode;
      iContainerPosition = aOther.iContainerPosition;
      iSegmentPosition = aOther.iSegmentPosition;
      return *this;
    }
    const_iterator& operator=(const typename segmented_array::iterator& aOther)
    {
      iContainer = aOther.iContainer;
      iNode = aOther.iNode;
      iContainerPosition = aOther.iContainerPosition;
      iSegmentPosition = aOther.iSegmentPosition;
      return *this;
    }
  private:
    const_iterator(const segmented_array& aContainer, size_type aContainerPosition) :
        iContainer(&aContainer), iNode(nullptr), iContainerPosition(aContainerPosition), iSegmentPosition(0)
    {
      iNode = iContainer->find_node(aContainerPosition, iSegmentPosition);
      if (iNode->is_nil())
        *this = iContainer->end();
    }
    const_iterator(const segmented_array& aContainer, node* aNode, size_type aContainerPosition, size_type aSegmentPosition) :
        iContainer(&aContainer), iNode(aNode), iContainerPosition(aContainerPosition), iSegmentPosition(aSegmentPosition)
    {
    }

  public:
    const_iterator& operator++()
    {
      ++iContainerPosition;
      if (++iSegmentPosition == static_cast<node*>(iNode)->segment().size())
      {
        if (iNode != iContainer->back_node())
        {
          iNode = static_cast<node*>(iNode->next());
          iSegmentPosition = 0;
        }
      }
      return *this;
    }
#ifndef _MSC_VER  // Internal Compiler Error (VS2022)
    const_iterator& operator--()
            {
                --iContainerPosition;
                if (iSegmentPosition-- == 0)
                {
                    iNode = static_cast<node*>(iNode->previous());
                    iSegmentPosition = static_cast<node*>(iNode)->segment().size() - 1;
                }
                return *this;
            }
#else // Internal Compiler Error (VS2022) workaround
    const_iterator& operator--()
    {
      --iContainerPosition;
      if (iSegmentPosition == 0)
      {
        iNode = static_cast<node*>(iNode->previous());
        iSegmentPosition = static_cast<node*>(iNode)->segment().size() - 1;
      }
      else
        --iSegmentPosition;
      return *this;
    }
#endif
    const_iterator operator++(int) { const_iterator ret(*this); operator++(); return ret; }
    const_iterator operator--(int) { const_iterator ret(*this); operator--(); return ret; }
    const_iterator& operator+=(difference_type aDifference)
    {
      if (aDifference < 0)
        return operator-=(-aDifference);
      else if (iNode == nullptr || aDifference >= static_cast<difference_type>(segment().size() - iSegmentPosition))
        *this = const_iterator(*iContainer, iContainerPosition + aDifference);
      else
      {
        iContainerPosition += aDifference;
        iSegmentPosition += aDifference;
      }
      return *this;
    }
    const_iterator& operator-=(difference_type aDifference)
    {
      if (aDifference < 0)
        return operator+=(-aDifference);
      else if (aDifference > static_cast<difference_type>(iSegmentPosition))
        *this = const_iterator(*iContainer, iContainerPosition - aDifference);
      else
      {
        iContainerPosition -= aDifference;
        iSegmentPosition -= aDifference;
      }
      return *this;
    }
    const_iterator operator+(difference_type aDifference) const { const_iterator result(*this); result += aDifference; return result; }
    const_iterator operator-(difference_type aDifference) const { const_iterator result(*this); result -= aDifference; return result; }
    const_reference operator[](difference_type aDifference) const { return *((*this) + aDifference); }
    friend difference_type operator-(const const_iterator& aLhs, const const_iterator& aRhs) { return static_cast<difference_type>(aLhs.iContainerPosition)-static_cast<difference_type>(aRhs.iContainerPosition); }
    const_reference operator*() const { return segment()[iSegmentPosition]; }
    const_pointer operator->() const { return &operator*(); }
    bool operator==(const const_iterator& aOther) const { return iContainerPosition == aOther.iContainerPosition; }
    bool operator!=(const const_iterator& aOther) const { return iContainerPosition != aOther.iContainerPosition; }
    bool operator<(const const_iterator& aOther) const { return iContainerPosition < aOther.iContainerPosition; }
    bool operator<=(const const_iterator& aOther) const { return iContainerPosition <= aOther.iContainerPosition; }
    bool operator>(const const_iterator& aOther) const { return iContainerPosition > aOther.iContainerPosition; }
    bool operator>=(const const_iterator& aOther) const { return iContainerPosition >= aOther.iContainerPosition; }

  private:
    segment_type& segment() const { return iNode->segment(); }

  private:
    const segmented_array* iContainer;
    node* iNode;
    size_type iContainerPosition;
    size_type iSegmentPosition;
  };
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

public:
  segmented_array(const Alloc& aAllocator = Alloc()) :
      iAllocator{ aAllocator }, iSize{ 0 }
  {
  }
  segmented_array(const size_type aCount, const value_type& aValue, const Alloc& aAllocator = Alloc()) :
      iAllocator{ aAllocator }, iSize{ 0 }
  {
    insert(begin(), aCount, aValue);
  }
  template <typename InputIterator>
  segmented_array(InputIterator aFirst, InputIterator aLast, const Alloc& aAllocator = Alloc()) :
      iAllocator{ aAllocator }, iSize{ 0 }
  {
    insert(begin(), aFirst, aLast);
  }
  segmented_array(const segmented_array& aOther, const Alloc& aAllocator = Alloc()) :
      iAllocator{ aAllocator }, iSize{ 0 }
  {
    insert(begin(), aOther.begin(), aOther.end());
  }
  segmented_array(segmented_array&& aOther) :
      base_type{ std::move(aOther) },
      iAllocator{ std::move(aOther.iAllocator) }, iSize{ aOther.iSize }
  {
    aOther.iSize = 0;
  }
  ~segmented_array()
  {
    erase(begin(), end());
  }
  segmented_array& operator=(segmented_array&& aOther)
  {
    swap(aOther);
    return *this;
  }
  segmented_array& operator=(const segmented_array& aOther)
  {
    segmented_array newContents(aOther);
    newContents.swap(*this);
    return *this;
  }

public:
  size_type size() const
  {
    return iSize;
  }
  bool empty() const
  {
    return iSize == 0;
  }
  const_iterator cbegin() const
  {
    return const_iterator(*this, static_cast<node*>(base_type::front_node()), 0, 0);
  }
  const_iterator begin() const
  {
    return cbegin();
  }
  const_iterator cend() const
  {
    return const_iterator(*this, static_cast<node*>(base_type::back_node()), iSize, base_type::back_node() ? static_cast<node*>(base_type::back_node())->segment().size() : 0);
  }
  const_iterator end() const
  {
    return cend();
  }
  iterator begin()
  {
    return iterator(*this, static_cast<node*>(base_type::front_node()), 0, 0);
  }
  iterator end()
  {
    return iterator(*this, static_cast<node*>(base_type::back_node()), iSize, base_type::back_node() ? static_cast<node*>(base_type::back_node())->segment().size() : 0);
  }
  const_reverse_iterator rbegin() const
  {
    return const_reverse_iterator(end());
  }
  const_reverse_iterator rend() const
  {
    return const_reverse_iterator(begin());
  }
  reverse_iterator rbegin()
  {
    return reverse_iterator(end());
  }
  reverse_iterator rend()
  {
    return reverse_iterator(begin());
  }
  const_iterator citer(const value_type& aValue) const
  {
    return cbegin() + (&aValue - &(*this)[0]);
  }
  const_iterator iter(const value_type& aValue) const
  {
    return citer(aValue);
  }
  iterator iter(const value_type& aValue)
  {
    return begin() + (&aValue - &(*this)[0]);
  }
  const_reference front() const
  {
    return *begin();
  }
  reference front()
  {
    return *begin();
  }
  const_reference back() const
  {
    return *--end();
  }
  reference back()
  {
    return *--end();
  }
  iterator insert(const_iterator aPosition, const value_type& aValue)
  {
    return insert(aPosition, static_cast<size_type>(1), aValue);
  }
  template <typename... Args>
  iterator emplace_insert(const_iterator aPosition, Args&&... aArguments)
  {
    return emplace_insert(aPosition, static_cast<size_type>(1), std::forward<Args>(aArguments)...);
  }
  const_reference operator[](size_type aIndex) const
  {
    return *(begin() + aIndex);
  }
  reference operator[](size_type aIndex)
  {
    return *(begin() + aIndex);
  }
  template <class InputIterator>
  typename std::enable_if<!std::is_integral<InputIterator>::value, iterator>::type
  insert(const_iterator aPosition, InputIterator aFirst, InputIterator aLast)
  {
    return do_insert(aPosition, aFirst, aLast);
  }
  iterator insert(const_iterator aPosition, size_type aCount, const value_type& aValue)
  {
    auto pos = aPosition.iContainerPosition;
    while (aCount > 0)
    {
      aPosition = insert(aPosition, &aValue, &aValue+1);
      ++aPosition;
      --aCount;
    }
    return iterator{*this, pos};
  }
  template <typename... Args>
  iterator emplace_insert(const_iterator aPosition, size_type aCount, Args&&... aArguments)
  {
    auto pos = aPosition.iContainerPosition;
    while (aCount > 0)
    {
      // todo: shouldn't be creating a temporary for emplace
      auto const& temp = value_type{ std::forward<Args>(aArguments)... };
      aPosition = insert(aPosition, &temp, &temp + 1);
      ++aPosition;
      --aCount;
    }
    return iterator{ *this, pos };
  }
  void clear()
  {
    erase(begin(), end());
  }
  void push_front(const value_type& aValue)
  {
    insert(begin(), aValue);
  }
  void push_back(const value_type& aValue)
  {
    insert(end(), aValue);
  }
  template <typename... Args>
  void emplace_front(Args&&... aArguments)
  {
    emplace_insert(begin(), std::forward<Args>(aArguments)...);
  }
  template <typename... Args>
  void emplace_back(Args&&... aArguments)
  {
    emplace_insert(end(), std::forward<Args>(aArguments)...);
  }
  void resize(std::size_t aNewSize, const value_type& aValue = value_type{})
  {
    if (size() < aNewSize)
      insert(end(), aNewSize - size(), aValue);
    else
      erase(begin() + aNewSize, end());
  }
  iterator erase(const_iterator aPosition)
  {
    erase(aPosition, aPosition + 1);
    return iterator{*this, aPosition.iContainerPosition};
  }
  iterator erase(const_iterator aFirst, const_iterator aLast)
  {
    if (aFirst == aLast)
      return iterator{*this, aFirst.iNode, aFirst.iContainerPosition, aFirst.iSegmentPosition};
    segment_type& segmentFirst = aFirst.iNode->segment();
    if (aFirst.iNode == aLast.iNode)
    {
      segmentFirst.erase(segmentFirst.begin() + aFirst.iSegmentPosition, segmentFirst.begin() + aLast.iSegmentPosition);
      iSize -= (aLast.iSegmentPosition - aFirst.iSegmentPosition);
      aFirst.iNode->set_size(aFirst.iNode->size() - (aLast.iSegmentPosition - aFirst.iSegmentPosition));
      if (segmentFirst.empty())
        free_node(aFirst.iNode);
    }
    else
    {
      segment_type& segmentLast = aLast.iNode->segment();
      for (node* inbetweenNode = static_cast<node*>(aFirst.iNode->next()); inbetweenNode != aLast.iNode;)
      {
        node* next = static_cast<node*>(inbetweenNode->next());
        size_type inbetweenRemoved = inbetweenNode->segment().size();
        free_node(inbetweenNode);
        iSize -= inbetweenRemoved;
        inbetweenNode = next;
      }
      size_type firstRemoved = segmentFirst.size() - aFirst.iSegmentPosition;
      size_type secondRemoved = aLast.iSegmentPosition;
      segmentFirst.erase(segmentFirst.begin() + aFirst.iSegmentPosition, segmentFirst.end());
      segmentLast.erase(segmentLast.begin(), segmentLast.begin() + aLast.iSegmentPosition);
      if (segmentFirst.empty())
        free_node(aFirst.iNode);
      else
        aFirst.iNode->set_size(aFirst.iNode->size() - firstRemoved);
      iSize -= firstRemoved;
      if (segmentLast.empty())
        free_node(aLast.iNode);
      else
        aLast.iNode->set_size(aLast.iNode->size() - secondRemoved);
      iSize -= secondRemoved;
    }
    return iterator{*this, aFirst.iContainerPosition};
  }
  void pop_front()
  {
    erase(begin());
  }
  void pop_back()
  {
    erase(--end());
  }
  void swap(segmented_array& aOther)
  {
    base_type::swap(aOther);
    std::swap(iAllocator, aOther.iAllocator);
    std::swap(iSize, aOther.iSize);
  }

private:
  template <class InputIterator>
  typename std::enable_if<!std::is_same<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value, iterator>::type
  do_insert(const_iterator aPosition, InputIterator aFirst, InputIterator aLast)
  {
    size_type count = std::distance(aFirst, aLast);
    if (count == 0)
      return iterator{*this, aPosition.iNode, aPosition.iContainerPosition, aPosition.iSegmentPosition};
    node* before = aPosition.iNode;
    node* after = aPosition.iNode ? static_cast<node*>(aPosition.iNode->next()) : nullptr;
    node* lastNode = aPosition.iNode;
    if (aPosition.iNode != nullptr && count <= static_cast<node*>(aPosition.iNode)->segment().available())
    {
      segment_type& segment = static_cast<node*>(aPosition.iNode)->segment();
      segment.insert(segment.begin() + aPosition.iSegmentPosition, aFirst, aLast);
      iSize += count;
      aPosition.iNode->set_size(aPosition.iNode->size() + count);
    }
    else
    {
      lastNode = allocate_space(aPosition, count);
      if (aPosition.iNode == nullptr)
        aPosition = begin();
      segment_type& segment = aPosition.iNode->segment();
      typename segment_type::const_iterator tailEnd = segment.end();
      typename segment_type::const_iterator tailStart = tailEnd - (segment.size() - aPosition.iSegmentPosition);
      if (tailStart != tailEnd)
      {
        lastNode->segment().insert(lastNode->segment().begin(), tailStart, tailEnd);
        lastNode->set_size(lastNode->size() + (tailEnd - tailStart));
        aPosition.iNode->set_size(aPosition.iNode->size() - (tailEnd - tailStart));
        segment.erase(tailStart, tailEnd);
      }
      for (node* nextNode = aPosition.iNode; count > 0 && nextNode != lastNode; nextNode = static_cast<node*>(nextNode->next()))
      {
        size_type addCount = std::min(count, nextNode->segment().available());
        if (addCount != 0)
        {
          InputIterator stop = aFirst;
          std::advance(stop, addCount);
          nextNode->segment().insert(nextNode->segment().begin() + (nextNode == aPosition.iNode ? aPosition.iSegmentPosition : 0), aFirst, stop);
          aFirst = stop;
          iSize += addCount;
          count -= addCount;
          nextNode->set_size(nextNode->size() + addCount);
        }
      }
      if (count > 0)
      {
        InputIterator stop = aFirst;
        std::advance(stop, count);
        lastNode->segment().insert(lastNode->segment().begin() + (lastNode == aPosition.iNode ? aPosition.iSegmentPosition : 0), aFirst, stop);
        lastNode->set_size(lastNode->size() + count);
        iSize += count;
      }
    }
    size_type index = aPosition.iContainerPosition - aPosition.iSegmentPosition;
    for (node* newNode = aPosition.iNode;; newNode = static_cast<node*>(newNode->next()))
    {
      if (newNode != before && newNode != after)
      {
        base_type::insert_node(newNode, index);
      }
      index += newNode->segment().size();
      if (newNode == lastNode)
        break;
    }
    if (aPosition.iSegmentPosition != aPosition.iNode->segment().size()) // was not end
      return iterator{*this, aPosition.iNode, aPosition.iContainerPosition, aPosition.iSegmentPosition};
    else
      return iterator{*this, static_cast<node*>(aPosition.iNode->next()), aPosition.iContainerPosition, 0};
  }
  template <class InputIterator>
  typename std::enable_if<std::is_same<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value, iterator>::type
  do_insert(const_iterator aPosition, InputIterator aFirst, InputIterator aLast)
  {
    auto pos = aPosition.iContainerPosition;
    while (aFirst != aLast)
    {
      aPosition = insert(aPosition, 1, *aFirst++);
      ++aPosition;
    }
    return iterator{*this, pos};
  }
  node* find_node(size_type aContainerPosition, size_type& aSegmentPosition) const
  {
    size_type nodeIndex = 0;
    node* result = static_cast<node*>(base_type::find_node(aContainerPosition, nodeIndex));
    aSegmentPosition = aContainerPosition - nodeIndex;
    return result;
  }
  node* allocate_space(const_iterator aPosition, size_type aCount)
  {
    if (aCount == 0)
      return aPosition.iNode;
    if (aPosition.iNode)
      aCount -= std::min(aCount, (aPosition.iNode->segment().available()));
    if (aCount == 0)
      return aPosition.iNode;
    node* lastNode = nullptr;
    if (aCount > 0 && aPosition.iNode && aPosition.iNode->next() != nullptr && aCount <= static_cast<node*>(aPosition.iNode->next())->segment().available())
    {
      lastNode = static_cast<node*>(aPosition.iNode->next());
      aCount -= std::min(aCount, lastNode->segment().available());
    }
    node* nextNode = aPosition.iNode;
    while (aCount > 0)
      aCount -= std::min(aCount, (nextNode = allocate_node(nextNode))->segment().available());
    if (aPosition.iNode == nullptr)
      aPosition = begin();
    segment_type& segment = aPosition.iNode->segment();
    if (aPosition.iSegmentPosition < segment.size() && nextNode->segment().available() < segment.size() - aPosition.iSegmentPosition)
      lastNode = allocate_node(nextNode);
    return lastNode ? lastNode : nextNode;
  }
  node* allocate_node(node* aAfter)
  {
    node* newNode = std::allocator_traits<node_allocator_type>::allocate(iAllocator, 1);
    try
    {
      std::allocator_traits<node_allocator_type>::construct(iAllocator, newNode, node());
    }
    catch (...)
    {
      std::allocator_traits<node_allocator_type>::deallocate(iAllocator, newNode, 1);
      throw;
    }
    if (aAfter == nullptr)
    {
      base_type::set_front_node(newNode);
      base_type::set_back_node(newNode);
    }
    else
    {
      newNode->set_previous(aAfter);
      if (aAfter->next() != nullptr)
      {
        newNode->set_next(aAfter->next());
        aAfter->next()->set_previous(newNode);
      }
      aAfter->set_next(newNode);
      if (base_type::back_node() == aAfter)
        base_type::set_back_node(newNode);
    }
    return newNode;
  }
  void free_node(node* aNode)
  {
    if (aNode)
    {
      if (aNode->next())
        aNode->next()->set_previous(aNode->previous());
      if (aNode->previous())
        aNode->previous()->set_next(aNode->next());
      if (base_type::back_node() == aNode)
        base_type::set_back_node(aNode->previous());
      if (base_type::front_node() == aNode)
        base_type::set_front_node(aNode->next());
      base_type::delete_node(aNode);
    }
    std::allocator_traits<node_allocator_type>::destroy(iAllocator, aNode);
    std::allocator_traits<node_allocator_type>::deallocate(iAllocator, aNode, 1);
  }

private:
  node_allocator_type iAllocator;
  size_type iSize;
};

template <typename T, std::size_t SegmentSize, typename Alloc>
inline bool operator==(segmented_array<T, SegmentSize, Alloc> const& lhs, segmented_array<T, SegmentSize, Alloc> const& rhs)
{
  return lhs.size() == rhs.size() && std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
}

template <typename T, std::size_t SegmentSize, typename Alloc>
inline bool operator!=(segmented_array<T, SegmentSize, Alloc> const& lhs, segmented_array<T, SegmentSize, Alloc> const& rhs)
{
  return !(lhs == rhs);
}
}
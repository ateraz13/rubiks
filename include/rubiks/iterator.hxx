#ifndef ITERATOR_HXX
#define ITERATOR_HXX
#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

template <
    typename InputIt, typename Func,
    typename Enabled = typename std::enable_if<std::is_same_v<
        typename decltype(std::function{std::declval<Func>()})::result_type,
        typename std::iterator_traits<InputIt>::value_type>>>
class MappingIterator {
public:
  using value_type = typename std::result_of<Func(
      typename std::iterator_traits<InputIt>::reference)>::type;
  using difference_type =
      typename std::iterator_traits<InputIt>::difference_type;
  using pointer = value_type *;
  using reference = value_type &;
  using iterator_category =
      typename std::iterator_traits<InputIt>::iterator_category;

  MappingIterator(InputIt it, Func func) : m_current(it), m_mapper(func) {}

  value_type operator*() const { return m_mapper(*m_current); }

  MappingIterator &operator++() {
    ++m_current;
    return *this;
  }

  MappingIterator operator++(int) {
    MappingIterator tmp = *this;
    ++(*this);
    return tmp;
  }

  bool operator==(const MappingIterator &other) const {
    return m_current == other.m_current;
  }
  bool operator!=(const MappingIterator &other) const {
    return !(*this == other);
  }

private:
  InputIt m_current;
  Func m_mapper;
};

template <typename InputIt, typename Func>
MappingIterator<InputIt, Func> make_mapping_iterator(InputIt it, Func func) {
  return MappingIterator<InputIt, Func>(it, func);
}

#endif // ITERATOR_HXX

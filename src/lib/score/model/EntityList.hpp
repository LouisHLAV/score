#pragma once
#include <score/tools/Debug.hpp>
#include <score/tools/std/IndirectContainer.hpp>

#include <ossia/detail/algorithms.hpp>

#include <list>

namespace score
{

template <typename T>
class EntityList
{
public:
  // The real interface starts here
  using value_type = T;
  auto begin() const { return score::make_indirect_iterator(m_list.begin()); }
  auto cbegin() const { return score::make_indirect_iterator(m_list.cbegin()); }
  auto end() const { return score::make_indirect_iterator(m_list.end()); }
  auto cend() const { return score::make_indirect_iterator(m_list.cend()); }
  auto size() const { return m_list.size(); }
  bool empty() const { return m_list.empty(); }
  auto& unsafe_list() { return m_list; }
  const auto& list() const { return m_list; }
  const auto& get() const { return m_list.get(); }
  T& at(const Id<T>& id)
  {
    auto it = find(id);
    SCORE_ASSERT(it != m_list.end());
    return *it;
  }
  T& at(const Id<T>& id) const
  {
    auto it = find(id);
    SCORE_ASSERT(it != m_list.end());
    return **it;
  }
  T& at_pos(std::size_t n) const
  {
    SCORE_ASSERT(n < m_list.size());
    auto it = m_list.begin();
    std::advance(it, n);
    return **it;
  }
  auto find(const Id<T>& id) const
  {
    return ossia::find_if(m_list, [&](auto ptr) { return ptr->id() == id; });
  }
  auto find(const Id<T>& id)
  {
    return ossia::find_if(m_list, [&](auto ptr) { return ptr->id() == id; });
  }

  auto index(const Id<T>& id) const
  {
    auto it = ossia::find_if(m_list, [&](auto ptr) { return ptr->id() == id; });
    ;
    SCORE_ASSERT(it != m_list.end());
    return std::distance(m_list.begin(), it);
  }

  // public:
  mutable Nano::Signal<void(T&)> mutable_added;
  mutable Nano::Signal<void(const T&)> added;
  mutable Nano::Signal<void(const T&)> removing;
  mutable Nano::Signal<void(const T&)> removed;
  mutable Nano::Signal<void()> orderChanged;

  void add(T* t)
  {
    SCORE_ASSERT(t);
    unsafe_list().push_back(t);

    mutable_added(*t);
    added(*t);
  }

  void erase(T& elt)
  {
    auto it = ossia::find(m_list, &elt);
    SCORE_ASSERT(it != m_list.end());

    removing(elt);
    m_list.erase(it);
    removed(elt);
  }

  void remove(T& elt)
  {
    erase(elt);
    delete &elt;
  }

  void remove(T* elt) { remove(*elt); }

  void remove(const Id<T>& id)
  {
    auto it = find(id);
    SCORE_ASSERT(it != m_list.end());
    auto& elt = **it;

    removing(elt);
    m_list.erase(it);
    removed(elt);
    delete &elt;
  }

  void clear()
  {
    while(!m_list.empty())
    {
      remove(*m_list.begin());
    }
  }

  void insert_at(std::size_t pos, T* t)
  {
    SCORE_ASSERT(pos <= m_list.size());
    auto it = m_list.begin();
    std::advance(it, pos);

    m_list.insert(it, t);
    mutable_added(*t);
    added(*t);
  }
  void move(const Id<T>& id, std::size_t pos)
  {
    auto new_it = m_list.begin();
    std::advance(new_it, pos);
    auto it1 = find(id);
    m_list.splice(new_it, m_list, it1);

    orderChanged();
  }

private:
  std::list<T*> m_list;
};

}

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <type_traits>
#include <vector>

template <size_t N>
class StackStorage {
 public:
  StackStorage() : begin_(buffer_), buffer_(), ptr_(buffer_), size_(N){};
  void* allocate(size_t n, size_t sz_of, size_t alignment) {
    if (std::align(alignment, sz_of * n, ptr_, size_) != nullptr) {
      void* result = ptr_;
      ptr_ = (char*)ptr_ + sz_of * n;
      size_ -= sz_of * n;
      return result;
    }
    return nullptr;
  }
  StackStorage(const StackStorage&) = delete;

 private:
  char* begin_ = nullptr;
  char buffer_[N];
  void* ptr_;
  std::size_t size_;
};

template <typename T, size_t N>
class StackAllocator {
 public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = std::size_t;
  using difference_type = ptrdiff_t;

  template <class U>
  struct rebind {
    using other = StackAllocator<U, N>;
  };
  struct propagate_on_container_copy_assignment : std::false_type {};

  StackAllocator() : storage_(nullptr){};
  StackStorage<N>* getStorage() const { return storage_; }

  template <typename U>
  constexpr StackAllocator(StackAllocator<U, N> other_alloc) noexcept {
    storage_ = other_alloc.getStorage();
  }
  explicit StackAllocator(StackStorage<N>& storage) : storage_(&storage) {}
  ~StackAllocator() = default;

  template <typename U>
  StackAllocator* operator=(const StackAllocator<U, N>& another) {
    storage_ = another.storage_;
    return *this;
  }

  pointer allocate(size_type n) {
    if (storage_ == nullptr) {
      return nullptr;
    }
    void* result = storage_->allocate(n, sizeof(T), alignof(T));
    if (result == nullptr) {
      return nullptr;
    }
    return reinterpret_cast<T*>(result);
  }

  bool operator==(const StackAllocator& another) const {
    return storage_ == another.storage_;
  }
  bool operator!=(const StackAllocator& another) const {
    return storage_ != another.storage_;
  }
  void deallocate(pointer, size_type) {}

 private:
  StackStorage<N>* storage_ = nullptr;
};

template <typename T, typename Allocator = std::allocator<T>>
class List {
 private:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = std::size_t;
  using difference_type = ptrdiff_t;

  struct BaseNode {
    BaseNode* prev;
    BaseNode* next;
    BaseNode() = default;
    BaseNode(BaseNode* prev, BaseNode* next) : prev(prev), next(next){};
    ~BaseNode() = default;
  };
  struct Node : public BaseNode {
    value_type value;
    Node() = default;
    Node(BaseNode* prev, BaseNode* next) : BaseNode(prev, next){};
    Node(const_reference value, BaseNode* prev, BaseNode* next)
        : BaseNode(prev, next), value(value) {}
    ~Node() = default;
  };

  using NodeAllocator =
      typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
  using NodeAllocTraits =
      typename std::allocator_traits<Allocator>::template rebind_traits<Node>;

  void swap(List<T, Allocator>& other) {
    std::swap(fake_node_, other.fake_node_);
    std::swap(sz_, other.sz_);
  }

  NodeAllocator allocator_;
  BaseNode fake_node_;
  size_type sz_ = 0;

 public:
  NodeAllocator get_allocator() const { return allocator_; }

  List() : fake_node_{&fake_node_, &fake_node_} {}

  List(const Allocator& another_alloc)
      : allocator_(another_alloc),
        fake_node_{&fake_node_, &fake_node_},
        sz_(0) {}

  void push_back(const_reference val) {
    Node* node = NodeAllocTraits::allocate(allocator_, 1);
    NodeAllocTraits::construct(allocator_, node, val, fake_node_.prev,
                               &fake_node_);
    fake_node_.prev->next = node;
    fake_node_.prev = node;
    ++sz_;
  }

  void push_front(const_reference val) {
    Node* node = NodeAllocTraits::allocate(allocator_, 1);
    NodeAllocTraits::construct(allocator_, node, val, &fake_node_,
                               fake_node_.next);
    fake_node_.next->prev = node;
    fake_node_.next = node;
    ++sz_;
  }

  List(size_type n) : fake_node_({&fake_node_, &fake_node_}) {
    size_t i = 0;
    try {
      for (i = 0; i < n; ++i) {
        Node* node =
            static_cast<Node*>(NodeAllocTraits::allocate(allocator_, 1));
        NodeAllocTraits::construct(allocator_, node, fake_node_.prev,
                                   &fake_node_);
        fake_node_.prev->next = node;
        fake_node_.prev = node;
        ++sz_;
      }
    } catch (...) {
      for (size_t j = 0; j < i; ++j) {
        pop_front();
      }
      throw;
    }
  }

  List(size_type n, const Allocator& another_alloc)
      : allocator_(another_alloc), fake_node_({&fake_node_, &fake_node_}) {
    for (size_t i = 0; i < n; ++i) {
      Node* node = NodeAllocTraits::allocate(allocator_, 1);
      NodeAllocTraits::construct(allocator_, node, fake_node_.prev,
                                 &fake_node_);
      fake_node_.prev->next = node;
      fake_node_.prev = node;
      ++sz_;
    }
  }

  List(size_type n, const value_type& value, const Allocator& other_allocator)
      : allocator_(other_allocator), fake_node_({&fake_node_, &fake_node_}) {
    for (size_t i = 0; i < n; ++i) {
      Node* node = NodeAllocTraits::allocate(allocator_, 1);
      NodeAllocTraits::construct(allocator_, node, value, fake_node_.prev,
                                 &fake_node_);
      fake_node_.prev->next = node;
      fake_node_.prev = node;
      ++sz_;
    }
  }
  List(size_type n, const value_type& value)
      : allocator_(get_allocator()), fake_node_({&fake_node_, &fake_node_}) {
    for (size_t i = 0; i < n; ++i) {
      push_back(value);
    }
  }

  List<T, Allocator>(const List<T, Allocator>& another)
      : fake_node_({&fake_node_, &fake_node_}) {
    allocator_ = NodeAllocTraits::select_on_container_copy_construction(
        another.allocator_);
    auto it = another.begin();
    try {
      for (; it != another.end(); ++it) {
        push_back(*it);
      }
    } catch (...) {
      for (auto ti = another.begin(); ti != it; ++ti) {
        pop_front();
      }
      throw;
    }
  }

  List& operator=(const List<value_type, Allocator>& another) {
    List<value_type, Allocator> copy_list = another;
    if (NodeAllocTraits::propagate_on_container_copy_assignment::value &&
        allocator_ != another.allocator_) {
      allocator_ = another.allocator_;
    }
    swap(copy_list);
    //    std::swap(fake_node_, copy_list.fake_node_);
    return *this;
  }

  void pop_front() {
    Node* deleting_elem = static_cast<Node*>(fake_node_.next);
    --sz_;
    fake_node_.next = fake_node_.next->next;
    fake_node_.next->prev = &fake_node_;
    if (sz_ == 0) {
      fake_node_ = {&fake_node_, &fake_node_};
    }
    NodeAllocTraits::destroy(allocator_, deleting_elem);
    NodeAllocTraits::deallocate(allocator_, deleting_elem, 1);
  }

  void pop_back() {
    Node* deleting_elem = static_cast<Node*>(fake_node_.prev);
    --sz_;
    fake_node_.prev = fake_node_.prev->prev;
    fake_node_.prev->next = &fake_node_;
    if (sz_ == 0) {
      fake_node_ = {&fake_node_, &fake_node_};
    }
    NodeAllocTraits::destroy(allocator_, deleting_elem);
    NodeAllocTraits::deallocate(allocator_, deleting_elem, 1);
  }

  size_t size() const { return sz_; }

  template <bool IsConst>
  class common_iterator {
   public:
    using Type = std::conditional_t<IsConst, const T, T>;
    using reference = Type&;
    using pointer = Type*;
    using iterator_type = std::bidirectional_iterator_tag;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::remove_reference_t<reference>;
    using difference_type = ptrdiff_t;

    common_iterator() = default;

    common_iterator(const common_iterator<false>& another)
        : node_ptr_(another.node_ptr_) {}
    common_iterator& operator=(const common_iterator<false>& another) {
      node_ptr_ = another.node_ptr_;
      return *this;
    }

    reference operator*() const { return static_cast<Node*>(node_ptr_)->value; }

    pointer operator->() const {
      return &(static_cast<Node*>(node_ptr_)->value);
    }

    common_iterator& operator++() {
      node_ptr_ = node_ptr_->next;
      return *this;
    }

    common_iterator operator++(int) {
      common_iterator copy_iter = *this;
      node_ptr_ = node_ptr_->next;
      return copy_iter;
    }

    common_iterator& operator--() {
      node_ptr_ = node_ptr_->prev;
      return *this;
    }

    common_iterator operator--(int) {
      common_iterator copy_iter = *this;
      node_ptr_ = node_ptr_->prev;
      return copy_iter;
    }

    bool operator==(const common_iterator<IsConst>& another) const {
      return (this->node_ptr_ == another.node_ptr_);
    }

    bool operator!=(const common_iterator<IsConst>& another) const {
      return !(*this == another);
    }
    common_iterator(BaseNode* ptr) : node_ptr_(ptr) {}

   private:
    friend class List;
    BaseNode* node_ptr_ = nullptr;
  };

  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;

  iterator begin() { return iterator(fake_node_.next); }
  const_iterator begin() const { return const_iterator(fake_node_.next); }
  const_iterator cbegin() const { return const_iterator(fake_node_.next); }

  iterator end() { return iterator(&fake_node_); }
  const_iterator end() const {
    return const_iterator(const_cast<BaseNode*>(&fake_node_));
  }
  const_iterator cend() const {
    return const_iterator(const_cast<BaseNode*>(&fake_node_));
  }

  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  reverse_iterator rbegin() { return reverse_iterator(&fake_node_); }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(const_cast<BaseNode*>(&fake_node_));
  }
  const_reverse_iterator crbegin() const {
    return const_iterator(const_cast<BaseNode*>(&fake_node_));
  }

  reverse_iterator rend() { return reverse_iterator(fake_node_.prev); }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(fake_node_.prev);
  }
  const_reverse_iterator crend() const {
    return const_reverse_iterator(fake_node_.prev);
  }

  void insert(const_iterator iter, const_reference value) {
    Node* inserted_node =
        static_cast<Node*>(NodeAllocTraits::allocate(allocator_, 1));
    NodeAllocTraits::construct(allocator_, inserted_node, value,
                               iter.node_ptr_->prev, iter.node_ptr_);
    iter.node_ptr_->prev->next = inserted_node;
    iter.node_ptr_->prev = inserted_node;
    ++sz_;
  }

  void erase(const_iterator iter) {
    Node* deleted_node = static_cast<Node*>(iter.node_ptr_);
    deleted_node->prev->next = deleted_node->next;
    deleted_node->next->prev = deleted_node->prev;
    NodeAllocTraits::destroy(allocator_, deleted_node);
    NodeAllocTraits::deallocate(allocator_, deleted_node, 1);
    --sz_;
  }
  ~List() {
    while (sz_ != 0) {
      pop_back();
    }
  }
};


#include <iostream>
#include <memory>

template <typename U>
class EnableSharedFromThis;

template <typename T>
class SharedPtr;

template <class T, class Alloc, class... Args>
SharedPtr<T> allocateShared(const Alloc& alloc, Args&&... args);

template <typename Y, typename... Args>
SharedPtr<Y> makeShared(Args&&... args);

struct BaseControlBlock {
  size_t shared_count = 0;
  size_t weak_count = 0;

  BaseControlBlock(size_t shared_count, size_t weak_count)
      : shared_count(shared_count), weak_count(weak_count) {}

  void FreeWeak() {
    --weak_count;
    if (weak_count == 0 && shared_count == 0) {
      Destroy();
    }
  };

  void FreeShared() {
    if (shared_count == 1) {
      Delete();
      --shared_count;
      if (weak_count == 0) {
        Destroy();
      }
    } else {
      --shared_count;
    }
  };

  virtual void Delete() = 0;
  virtual void Destroy() { delete this; }
  virtual ~BaseControlBlock() = default;
};

template <typename T, typename Allocator>
struct SharedBlock : BaseControlBlock {
  T value;
  Allocator alloc;

  SharedBlock(size_t shared_count, size_t weak_count, const Allocator& alloc,
              T&& val)
      : BaseControlBlock(shared_count, weak_count),
        value(std::move(val)),
        alloc(alloc) {}

  SharedBlock(size_t shared_count, size_t weak_count, const Allocator& alloc)
      : BaseControlBlock(shared_count, weak_count), alloc(alloc) {}

  void Delete() override {
    std::allocator_traits<Allocator>::destroy(alloc, &value);
  }

  void Destroy() override {
    typename std::allocator_traits<Allocator>::template rebind_alloc<
        SharedBlock>(alloc)
        .deallocate(this, 1);
  }

  T* get_ptr() { return &value; }
};

template <typename T, typename Deleter = std::default_delete<T>,
          typename Allocator = std::allocator<T>>
struct ControlBlock : BaseControlBlock {
  T* pointer;
  Deleter del;
  Allocator alloc;

  ControlBlock(size_t shared_count, size_t weak_count, T* ptr,
               const Deleter& del, const Allocator& alloc)
      : BaseControlBlock(shared_count, weak_count),
        pointer(ptr),
        del(del),
        alloc(alloc) {}

  void Delete() override { del(pointer); }

  void Destroy() override {
    typename std::allocator_traits<Allocator>::template rebind_alloc<
        ControlBlock>(alloc)
        .deallocate(this, 1);
  }
};
template <typename T>
class WeakPtr;

template <typename T>
class SharedPtr {
 private:
  BaseControlBlock* cb_ptr_ = nullptr;
  T* ptr_ = nullptr;

  template <typename Deleter, typename Allocator>
  auto allocate_construct_cb(T* pointer, const Deleter& del,
                             const Allocator& alloc) {
    auto ptr = typename std::allocator_traits<Allocator>::template rebind_alloc<
                   ControlBlock<T, Deleter, Allocator>>(alloc)
                   .allocate(1);
    new (ptr) ControlBlock<T, Deleter, Allocator>(1, 0, pointer, del, alloc);
    return ptr;
  }

  SharedPtr(const WeakPtr<T>& weak_ptr)
      : cb_ptr_(weak_ptr.cb_ptr_), ptr_(weak_ptr.ptr_) {
    if (cb_ptr_ != nullptr) {
      ++cb_ptr_->shared_count;
    }
  }

  template <typename Allocator>
  SharedPtr(SharedBlock<T, Allocator>* cb_pointer)
      : cb_ptr_(cb_pointer), ptr_(cb_pointer->get_ptr()) {
    if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
      cb_pointer->value.weak_ptr_ = *this;
    }
  }

 public:
  SharedPtr() : cb_ptr_(nullptr), ptr_(nullptr){};

  template <typename U, typename Deleter, typename Allocator>
  SharedPtr(U* pointer, const Deleter& del, const Allocator& alloc)
      : cb_ptr_(allocate_construct_cb(pointer, del, alloc)), ptr_(pointer) {
    if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
      pointer->weak_ptr_ = *this;
    }
  }

  template <typename U, typename Deleter>
  SharedPtr(U* pointer, const Deleter& del)
      : SharedPtr(pointer, del, std::allocator<T>()) {}

  template <typename U>
  SharedPtr(U* pointer)
      : SharedPtr(pointer, std::default_delete<T>(), std::allocator<T>()) {}

  SharedPtr(const SharedPtr& other_ptr)
      : cb_ptr_(other_ptr.cb_ptr_), ptr_(other_ptr.ptr_) {
    if (cb_ptr_ != nullptr) {
      ++cb_ptr_->shared_count;
    }
  }

  template <typename U>
  SharedPtr(const SharedPtr<U>& other_ptr)
      : cb_ptr_(other_ptr.cb_ptr_), ptr_(other_ptr.ptr_) {
    if (cb_ptr_ != nullptr) {
      ++cb_ptr_->shared_count;
    }
  }

  template <typename U>
  SharedPtr(SharedPtr<U>&& other_ptr)
      : cb_ptr_(other_ptr.cb_ptr_), ptr_(other_ptr.ptr_) {
    other_ptr.cb_ptr_ = nullptr;
    other_ptr.ptr_ = nullptr;
  }

  void swap(SharedPtr& other) {
    std::swap(cb_ptr_, other.cb_ptr_);
    std::swap(ptr_, other.ptr_);
  }

  SharedPtr& operator=(const SharedPtr& other_ptr) {
    auto tmp = SharedPtr<T>(other_ptr);
    swap(tmp);
    return *this;
  }

  template <typename U>
  SharedPtr& operator=(SharedPtr<U>&& other_ptr) {
    SharedPtr<T>(std::move(other_ptr)).swap(*this);
    return *this;
  }

  template <typename Y>
  void reset(Y* ptr) {
    SharedPtr<Y>(ptr).swap(*this);
  }

  void reset() { SharedPtr().swap(*this); }

  size_t use_count() const {
    return (cb_ptr_ == nullptr ? 0 : cb_ptr_->shared_count);
  }

  ~SharedPtr() {
    if (cb_ptr_ != nullptr) {
      cb_ptr_->FreeShared();
    }
  }

  T& operator*() const { return *ptr_; }

  T* operator->() const { return ptr_; }

  T* get() const { return ptr_; }

 private:
  template <typename U>
  friend class SharedPtr;

  template <typename U>
  friend class WeakPtr;

  template <typename Y, typename Alloc, typename... Args>
  friend SharedPtr<Y> allocateShared(const Alloc& alloc, Args&&... args);

  template <typename Y, typename... Args>
  friend SharedPtr<Y> makeShared(Args&&... args);
};

template <class T, class Alloc, class... Args>
SharedPtr<T> allocateShared(const Alloc& alloc, Args&&... args) {
  using Block = SharedBlock<T, Alloc>;
  using BlockAlloc =
      typename std::allocator_traits<Alloc>::template rebind_alloc<Block>;
  using BlockTraits = std::allocator_traits<BlockAlloc>;

  BlockAlloc block_alloc(alloc);
  auto block = BlockTraits::allocate(block_alloc, 1);

  try {
    BlockTraits::construct(block_alloc, block, 1, 0, alloc,
                           std::forward<Args>(args)...);
  } catch (...) {
    BlockTraits::deallocate(block_alloc, block, 1);
    throw;
  }
  return SharedPtr<T>(block);
}

template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
  return allocateShared<T, std::allocator<T>, Args...>(
      std::allocator<T>(), std::forward<Args>(args)...);
}

template <typename T>
class WeakPtr {
 private:
  BaseControlBlock* cb_ptr_ = nullptr;
  T* ptr_ = nullptr;

  template <typename U>
  friend class WeakPtr;

  template <typename U>
  friend class SharedPtr;

 public:
  WeakPtr() : cb_ptr_(nullptr), ptr_(nullptr){};

  template <typename U>
  WeakPtr(SharedPtr<U>& other_ptr)
      : cb_ptr_(other_ptr.cb_ptr_), ptr_(other_ptr.ptr_) {
    if (cb_ptr_ != nullptr) {
      ++cb_ptr_->weak_count;
    }
  }

  template <typename U>
  WeakPtr(WeakPtr<U>& other_ptr)
      : cb_ptr_(other_ptr.cb_ptr_), ptr_(other_ptr.ptr_) {
    if (cb_ptr_ != nullptr) {
      ++cb_ptr_->weak_count;
    }
  }

  template <typename U>
  WeakPtr(WeakPtr<U>&& other_ptr)
      : cb_ptr_(other_ptr.cb_ptr_), ptr_(other_ptr.ptr_) {
    other_ptr.cb_ptr_ = nullptr;
    other_ptr.ptr_ = nullptr;
  }

  template <typename U>
  void swap(WeakPtr<U>& other_ptr) {
    std::swap(cb_ptr_, other_ptr.cb_ptr_);
    std::swap(ptr_, other_ptr.ptr_);
  }

  template <typename U>
  WeakPtr& operator=(WeakPtr<U>& other) {
    auto tmp = WeakPtr<T>(other);
    swap(tmp);
    return *this;
  }

  template <typename U>
  WeakPtr& operator=(SharedPtr<U>& other) {
    WeakPtr<T>(other).swap(*this);
    return *this;
  }

  template <typename U>
  WeakPtr& operator=(WeakPtr&& other) {
    WeakPtr<T>(std::move(other)).swap(*this);
    return *this;
  }

  ~WeakPtr() {
    if (cb_ptr_ != nullptr) {
      cb_ptr_->FreeWeak();
    }
  }

  size_t use_count() const {
    return (cb_ptr_ == nullptr ? 0 : cb_ptr_->shared_count);
  }
  bool expired() const { return use_count() == 0; }

  SharedPtr<T> lock() const {
    return (expired() ? SharedPtr<T>() : SharedPtr<T>(*this));
  }
};

template <typename T>
class EnableSharedFromThis {
 private:
  WeakPtr<T> weak_ptr_;

  template <typename U>
  friend class SharedPtr;

 protected:
  EnableSharedFromThis() = default;

 public:
  SharedPtr<T> shared_from_this() {
    if (weak_ptr_.expired()) {
      throw std::bad_weak_ptr();
    }
    return weak_ptr_.lock();
  }
};
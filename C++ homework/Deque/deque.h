#include <iostream>
template <typename T>
class Deque {
 private:
  static const size_t chunk_size_ = 32;
  template <typename U>
  struct CellIndex;
  T** arr_ = nullptr;
  size_t sz_ = 0;
  size_t cap_ = 0;
  CellIndex<size_t> b_pos_ = {0, 0};
  CellIndex<size_t> e_pos_ = {0, 0};
  void swap(Deque<T>& another);
  CellIndex<size_t> get_position_by_index(size_t index) const;
  void swap_with_unitianalized_deque(size_t new_size);

 public:
  Deque() : arr_(nullptr), sz_(0), cap_(0){};
  Deque(const Deque<T>& another);
  Deque(size_t new_size);
  void init();
  void clear_init();
  void clear_block();
  size_t size() const;
  T& operator[](size_t index);
  const T& operator[](size_t index) const;
  Deque(size_t new_size, const T& value);
  Deque<T>& operator=(const Deque<T>& another);
  T& at(size_t index);
  const T& at(size_t index) const;
  void push_back(const T& value);
  void push_front(const T& value);
  void pop_back();
  void pop_front();
  template <bool IsConst>
  class common_iterator {
   private:
    T** iter_ = nullptr;
    T* chunk_begin_ = nullptr;
    CellIndex<size_t> iter_pos_;

    T* get_chunk_begin() const {
      return (iter_ == nullptr ? nullptr : iter_[iter_pos_.row]);
    }

    explicit common_iterator(T** point_arr, size_t row, size_t col)
        : iter_(point_arr), iter_pos_(CellIndex<size_t>(row, col)) {
      chunk_begin_ = get_chunk_begin();
    };

    friend Deque;

   public:
    using value_type = std::conditional_t<IsConst, const T, T>;
    using reference = value_type&;
    using pointer = value_type*;
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = ptrdiff_t;

    common_iterator() = default;

    common_iterator(const common_iterator<false>& another)
        : iter_(another.iter_), iter_pos_(another.iter_pos_) {
      chunk_begin_ = another.get_chunk_begin();
    }

    common_iterator& operator=(const common_iterator<false>& another) {
      iter_ = another.iter_;
      iter_pos_ = another.iter_pos_;
      chunk_begin_ = another.get_chunk_begin();
      return *this;
    }

    common_iterator& operator++() {
      ++iter_pos_;
      chunk_begin_ = get_chunk_begin();
      return *this;
    }

    common_iterator operator++(int) {
      common_iterator copy_iter = *this;
      ++iter_pos_;
      chunk_begin_ = get_chunk_begin();
      return copy_iter;
    }

    common_iterator& operator--() {
      --iter_pos_;
      chunk_begin_ = get_chunk_begin();
      return *this;
    }

    common_iterator operator--(int) {
      common_iterator copy_iter = *this;
      --iter_pos_;
      chunk_begin_ = get_chunk_begin();
      return copy_iter;
    }

    common_iterator& operator-=(int shift) {
      if (shift > 0) {
        if (shift > static_cast<int>(iter_pos_.col)) {
          shift -= iter_pos_.col;
          iter_pos_.row -= (shift / chunk_size_ + 1);
          iter_pos_.col = 0;
          iter_pos_.col = chunk_size_ - (shift % chunk_size_);
        } else {
          iter_pos_.row -= shift / chunk_size_;
          iter_pos_.col -= (shift % chunk_size_);
        }
      } else {
        iter_pos_.row += (-shift + iter_pos_.col) / chunk_size_;
        iter_pos_.col = (-shift + iter_pos_.col) % chunk_size_;
      }
      chunk_begin_ = get_chunk_begin();
      return *this;
    }

    common_iterator& operator+=(int shift) {
      (*this) -= (-shift);
      return *this;
    }

    common_iterator<IsConst> operator+(int shift) const {
      common_iterator<IsConst> copy = (*this);
      copy += shift;
      return copy;
    }

    friend common_iterator<IsConst> operator+(
        int shift, const common_iterator<IsConst>& iter) {
      common_iterator<IsConst> copy = iter;
      copy += shift;
      return copy;
    }

    common_iterator<IsConst> operator-(int shift) const {
      common_iterator<IsConst> copy = (*this);
      copy -= shift;
      return copy;
    }

    friend common_iterator<IsConst> operator-(
        int shift, const common_iterator<IsConst>& iter) {
      common_iterator<IsConst> copy = iter;
      copy -= shift;
      return copy;
    }

    difference_type operator-(const common_iterator<IsConst>& another) const {
      return (iter_pos_.row - another.iter_pos_.row) * chunk_size_ +
             iter_pos_.col - another.iter_pos_.col;
    }

    template <bool AnotherConst>
    common_iterator& operator=(const common_iterator<AnotherConst>& another) {
      *iter_ = *another.iter_;
      iter_pos_ = another.iter_pos_;
      chunk_begin_ = another.get_chunk_begin();
      return *this;
    }

    bool operator==(const common_iterator<IsConst>& another) const {
      return (*this - another == 0) || (another - *this == 0) ||
             (another.iter_ == iter_ && another.iter_pos_ == iter_pos_);
    }

    bool operator!=(const common_iterator<IsConst>& another) const {
      return !(*this == another);
    }

    bool operator<(const common_iterator<IsConst>& another) const {
      size_t true_coord_this = (iter_pos_.col + iter_pos_.row * chunk_size_);
      size_t true_coord_another =
          (another.iter_pos_.col + another.iter_pos_.row * chunk_size_);
      return (true_coord_this < true_coord_another);
    }

    bool operator>(const common_iterator<IsConst>& another) const {
      return another < *this;
    }

    bool operator>=(const common_iterator<IsConst>& another) const {
      return !(*this < another);
    }

    bool operator<=(const common_iterator<IsConst>& another) const {
      return !(*this > another);
    }

    reference operator*() const { return *(chunk_begin_ + iter_pos_.col); }
    pointer operator->() const { return &(operator*()); }
  };

  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;

  iterator begin() { return iterator(arr_, b_pos_.row, b_pos_.col); }
  iterator end() { return iterator(arr_, e_pos_.row, e_pos_.col); }

  const_iterator cbegin() const {
    const_iterator result(arr_, b_pos_.row, b_pos_.col);
    return result;
  }

  const_iterator cend() const { return cbegin() + sz_; }

  const_iterator begin() const { return cbegin(); }

  const_iterator end() const { return cend(); }

  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  reverse_iterator rbegin() { return reverse_iterator(end()); }

  reverse_iterator rend() { return reverse_iterator(begin()); }

  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(rbegin());
  }

  const_reverse_iterator crend() const {
    return const_reverse_iterator(rend());
  }

  reverse_iterator rbegin() const { return crbegin(); }

  reverse_iterator rend() const { return crend(); }

  void insert(const iterator& iter, const T& value) {
    T helper = value;
    for (iterator it = iter; it != end(); ++it) {
      std::swap(helper, *it);
    }
    push_back(helper);
  }

  void erase(const iterator& iter) {
    for (iterator it = iter; it != end(); ++it) {
      std::swap(*it, *(it + 1));
    }
    pop_back();
  }

  ~Deque();
};

template <typename T>
Deque<T>::~Deque() {
  clear_init();
  clear_block();
}

template <typename T>
void Deque<T>::pop_front() {
  begin()->~T();
  --sz_;
  ++b_pos_;
}

template <typename T>
void Deque<T>::pop_back() {
  (end() - 1)->~T();
  --e_pos_;
  --sz_;
}

template <typename T>
void Deque<T>::push_front(const T& value) {
  if (sz_ == 0 || (b_pos_.col == 0 && b_pos_.row == 0)) {
    ++sz_;
    Deque<T> new_dq;
    new_dq.swap_with_unitianalized_deque(sz_);
    CellIndex<size_t> new_end_index = new_dq.get_position_by_index(0);
    new (new_dq.arr_[new_end_index.row] + new_end_index.col) T(value);
    for (size_t i = 1; i < sz_; ++i) {
      CellIndex<size_t> new_index = new_dq.get_position_by_index(i);
      new (new_dq.arr_[new_index.row] + new_index.col) T((*this)[i - 1]);
    }
    *this = new_dq;
  } else {
    try {
      --b_pos_;
      new (arr_[b_pos_.row] + b_pos_.col) T(value);
      ++sz_;
    } catch (...) {
      ++b_pos_;
      throw;
    }
  };
}
template <typename T>
void Deque<T>::push_back(const T& value) {
  ++e_pos_;
  ++sz_;
  if (e_pos_.row < cap_ && sz_ > 1) {
    try {
      --e_pos_;
      new (arr_[e_pos_.row] + e_pos_.col) T(value);
      ++e_pos_;
    } catch (...) {
      --sz_;
      throw;
    }
  } else {
    --e_pos_;
    Deque<T> new_dq;
    new_dq.swap_with_unitianalized_deque(sz_);
    CellIndex<size_t> new_end_index = new_dq.get_position_by_index(sz_ - 1);
    --new_dq.e_pos_;
    new (new_dq.arr_[new_end_index.row] + new_end_index.col) T(value);
    ++new_dq.e_pos_;
    for (size_t i = 0; i < sz_ - 1; ++i) {
      CellIndex<size_t> new_index = new_dq.get_position_by_index(i);
      new (new_dq.arr_[new_index.row] + new_index.col) T((*this)[i]);
    }
    *this = new_dq;
  }
}

template <typename T>
const T& Deque<T>::at(size_t index) const {
  if (index >= 0 && index < sz_) {
    return (*this)[index];
  } else {
    throw std::out_of_range("");
  }
}

template <typename T>
T& Deque<T>::at(size_t index) {
  if (index >= 0 && index < sz_) {
    return (*this)[index];
  } else {
    throw std::out_of_range("");
  }
}

template <typename T>
Deque<T>& Deque<T>::operator=(const Deque<T>& another) {
  if (&another == this) {
    return *this;
  }
  Deque<T> copy(another);
  swap(copy);
  return *this;
}

template <typename T>
size_t Deque<T>::size() const {
  return sz_;
}

template <typename T>
void Deque<T>::swap(Deque<T>& another) {
  std::swap(arr_, another.arr_);
  std::swap(sz_, another.sz_);
  std::swap(cap_, another.cap_);
  std::swap(b_pos_, another.b_pos_);
  std::swap(e_pos_, another.e_pos_);
}

template <typename T>
Deque<T>::Deque(const Deque<T>& another) {
  sz_ = another.sz_;
  cap_ = another.cap_;
  b_pos_ = another.b_pos_;
  e_pos_ = another.e_pos_;
  arr_ = new T*[cap_];
  for (size_t i = 0; i < cap_; ++i) {
    arr_[i] = reinterpret_cast<T*>(new char[chunk_size_ * sizeof(T)]);
  }
  CellIndex<size_t> copy_b_pos = b_pos_;
  while (copy_b_pos != e_pos_) {
    try {
      new (arr_[copy_b_pos.row] + copy_b_pos.col)
          T(another.arr_[copy_b_pos.row][copy_b_pos.col]);
      ++copy_b_pos;
    } catch (...) {
      CellIndex<size_t> new_b_pos = b_pos_;
      while (new_b_pos != copy_b_pos) {
        (arr_[new_b_pos.row] + new_b_pos.col)->~T();
        ++new_b_pos;
      }
      throw;
    }
  }
}

template <typename T>
void Deque<T>::init() {
  try {
    for (size_t i = 0; i < cap_; ++i) {
      arr_[i] = reinterpret_cast<T*>(new char[chunk_size_ * sizeof(T)]);
    }
  } catch (...) {
    throw;
  }
}

template <typename T>
void Deque<T>::clear_init() {
  if (arr_ == nullptr || sz_ == 0) {
    return;
  }
  for (iterator it = begin(); it != end(); ++it) {
    try {
      it->~T();
    } catch (...) {
      throw "zxc";
    }
  }
}

template <typename T>
void Deque<T>::clear_block() {
  if (arr_ == nullptr) {
    return;
  }
  delete[] arr_;
}

template <typename T>
void Deque<T>::swap_with_unitianalized_deque(size_t new_size) {
  Deque<T> new_dq;
  new_dq.sz_ = new_size;
  new_dq.cap_ = 2 * (new_size / chunk_size_ + 1);
  new_dq.b_pos_ = {new_dq.cap_ / 2 - (new_dq.sz_ / 2 / chunk_size_ + 1),
                   chunk_size_ - (new_dq.sz_ / 2 % chunk_size_ + 1)};
  new_dq.e_pos_ = new_dq.b_pos_;
  try {
    new_dq.arr_ = new T*[new_dq.cap_];
  } catch (...) {
    delete[] new_dq.arr_;
    throw;
  }
  try {
    new_dq.init();
  } catch (...) {
    new_dq.clear_block();
    throw;
  }

  for (size_t i = 0; i < new_dq.size(); ++i) {
    ++new_dq.e_pos_;
  }

  new_dq.swap(*this);
}

template <typename T>
Deque<T>::Deque(size_t new_size) {
  swap_with_unitianalized_deque(new_size);
  for (iterator it = begin(); it != end(); ++it) {
    try {
      new (&*it) T();
    } catch (...) {
      for (iterator remove_iter = begin(); remove_iter != it; ++remove_iter) {
        remove_iter->~T();
      }
      throw;
    }
  }
}

template <typename T>
T& Deque<T>::operator[](size_t ind) {
  return *(begin() + ind);
}

template <typename T>
const T& Deque<T>::operator[](size_t ind) const {
  return *(cbegin() + ind);
}

template <typename T>
Deque<T>::CellIndex<size_t> Deque<T>::get_position_by_index(
    size_t index) const {
  size_t i = b_pos_.row + (b_pos_.col + index) / chunk_size_;
  size_t j = (b_pos_.col + index) % chunk_size_;
  return Deque<T>::CellIndex<size_t>(i, j);
}

template <typename T>
Deque<T>::Deque(size_t new_size, const T& value) {
  swap_with_unitianalized_deque(new_size);
  for (size_t i = 0; i < static_cast<size_t>(new_size); ++i) {
    CellIndex<size_t> new_index = get_position_by_index(i);
    try {
      new (arr_[new_index.row] + new_index.col) T(value);
    } catch (...) {
      for (size_t j = 0; j < i; ++j) {
        (arr_[new_index.row] + new_index.col)->~T();
      }
      throw;
    }
  }
}

template <typename T>
template <typename U>
struct Deque<T>::CellIndex {
  U row;
  U col;
  CellIndex() = default;

  CellIndex(std::pair<U, U> ind) : row(ind.first), col(ind.second) {}

  CellIndex(U row, U col) : row(row), col(col) {}

  CellIndex(const CellIndex& another) : row(another.row), col(another.col) {}

  CellIndex& operator=(const CellIndex& another) {
    row = another.row;
    col = another.col;
    return *this;
  }

  CellIndex operator++(int) {
    CellIndex copy_cell = *this;
    col++;
    if (col >= chunk_size_) {
      col = 0;
      row++;
    }
    return copy_cell;
  }

  CellIndex& operator++() {
    col++;
    if (col >= chunk_size_) {
      col = 0;
      row++;
    }
    return *this;
  }

  CellIndex operator--(int) {
    CellIndex copy_cell = *this;
    if (col == 0) {
      col = chunk_size_ - 1;
      row--;
    } else {
      col--;
    }
    return copy_cell;
  }

  CellIndex& operator--() {
    if (col == 0) {
      col = chunk_size_ - 1;
      row--;
    } else {
      col--;
    }
    return *this;
  }

  bool operator!=(const CellIndex& another) const = default;

  bool operator==(const CellIndex& another) const = default;
};


#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <new>
#include <string>

// Live-block counter: +1 for every array block new[] hands out, -1 for every
// block delete[] gives back. A correct program ends with 0.
static int g_live = 0;

void* operator new[](std::size_t n) {
    void* p = std::malloc(n ? n : 1);
    if (!p) throw std::bad_alloc();
    ++g_live;
    return p;
}
void operator delete[](void* p) noexcept {
    if (p) { --g_live; std::free(p); }
}
void operator delete[](void* p, std::size_t) noexcept {
    if (p) { --g_live; std::free(p); }
}

class IntBuffer {
public:
  explicit IntBuffer(std::size_t n) : data_(new int[n]()), size_(n) {}

  int get(std::size_t i) const { return data_[i]; }
  void set(std::size_t i, int v) { data_[i] = v; }
  std::size_t size() const { return size_; }

  // TODO: destructor
  ~IntBuffer() {
    delete[] data_;
  }

  // TODO: copy constructor (builds a new IntBuffer from an existing one)
  IntBuffer(const IntBuffer& other)
    :data_(new int[other.size_]),
    size_(other.size_) {

    std::copy(other.data_, other.data_ + other.size_, data_);
  }

  // TODO: copy assignment operator (overwrites an existing IntBuffer)
  IntBuffer& operator=(const IntBuffer& other) {
    if(this != &other) {
      delete[] data_;
      data_ = new int[other.size_];
      size_ = other.size_;
      std::copy(other.data_, other.data_ + other.size_, data_);
    }
    return *this;
  }

private:
  int* data_;
  std::size_t size_;
};

int main() {
    IntBuffer* slot[3] = {nullptr, nullptr, nullptr};  // slots a, b, c
    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "make") {                 // make <slot> <n> <n values>
            char s;
            std::size_t n;
            std::cin >> s >> n;
            delete slot[s - 'a'];
            slot[s - 'a'] = new IntBuffer(n);
            for (std::size_t i = 0; i < n; ++i) {
                int v;
                std::cin >> v;
                slot[s - 'a']->set(i, v);
            }
        } else if (cmd == "clone") {         // clone <slot>
            char s;
            std::cin >> s;
            IntBuffer t(*slot[s - 'a']);     // copy-construct a temporary
            if (t.size() > 0) t.set(0, 100);
            long long sum = 0;
            for (std::size_t i = 0; i < t.size(); ++i) sum += t.get(i);
            std::cout << "clone sum=" << sum << "\n";
        } else if (cmd == "assign") {        // assign <dest> <src>
            char d, s;
            std::cin >> d >> s;
            *slot[d - 'a'] = *slot[s - 'a'];
        } else if (cmd == "set") {           // set <slot> <index> <value>
            char s;
            std::size_t i;
            int v;
            std::cin >> s >> i >> v;
            slot[s - 'a']->set(i, v);
        } else if (cmd == "show") {          // show <slot>
            char s;
            std::cin >> s;
            const IntBuffer& b = *slot[s - 'a'];
            std::cout << s << "[" << b.size() << "]:";
            for (std::size_t i = 0; i < b.size(); ++i) std::cout << " " << b.get(i);
            std::cout << "\n";
        } else if (cmd == "live") {
            std::cout << "live=" << g_live << "\n";
        }
    }
    for (int k = 0; k < 3; ++k) delete slot[k];
    std::cout << "live=" << g_live << "\n";
    return 0;
}


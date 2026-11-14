#include <cstdlib>
#include <iostream>
#include <map>
#include <new>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// Given: counters for the exercise. `live` counts blocks obtained with new[]
// that have not been released with delete[] yet.
static int live = 0;
static int copies = 0;
static int moves = 0;

void* operator new[](std::size_t n) {
    void* p = std::malloc(n ? n : 1);
    if (!p) throw std::bad_alloc();
    ++live;
    return p;
}
void operator delete[](void* p) noexcept { if (p) { --live; std::free(p); } }
void operator delete[](void* p, std::size_t) noexcept { if (p) { --live; std::free(p); } }

class IntBuffer {
public:
    IntBuffer() : data_(nullptr), size_(0) {}
    explicit IntBuffer(int n) : data_(n > 0 ? new int[n]() : nullptr), size_(n > 0 ? n : 0) {}
    ~IntBuffer() { delete[] data_; }

    IntBuffer(const IntBuffer& other)
        : data_(other.size_ > 0 ? new int[other.size_] : nullptr), size_(other.size_) {
        ++copies;
        for (int i = 0; i < size_; ++i) data_[i] = other.data_[i];
    }
    IntBuffer& operator=(const IntBuffer& other) {
        ++copies;
        if (this == &other) return *this;
        delete[] data_;
        size_ = other.size_;
        data_ = size_ > 0 ? new int[size_] : nullptr;
        for (int i = 0; i < size_; ++i) data_[i] = other.data_[i];
        return *this;
    }

    // TODO: the move constructor IntBuffer(IntBuffer&&) and the move assignment
    // operator=(IntBuffer&&). Both must do `++moves;` as their first
    // statement, the way the copy members do `++copies;`.
    // Declaring the copy members suppressed the implicit moves, so until you
    // write these every "move" below quietly becomes a copy.

  //IntBuffer(const IntBuffer&) = delete;
  //IntBuffer& operator= (const IntBuffer&) = delete;
  IntBuffer(IntBuffer&& other) noexcept {
    ++moves;

    data_ = other.size_ > 0 ? new int[other.size_] : nullptr;
    for (int i = 0; i < size_; ++i)
      data_[i] = other.data_[i];

    size_ = other.size_;

    delete[] other.data_;
    other.data_ = nullptr;
    other.size_ = 0;
  }

  IntBuffer& operator= (IntBuffer&& other) noexcept {
    ++moves;
    if (this == &other) 
      return *this;

    delete[] data_;
    size_ = other.size_;
    data_ = size_ > 0 ? new int[size_] : nullptr;
    for (int i = 0; i < size_; ++i) data_[i] = other.data_[i];
    
    delete[] other.data_;
    other.data_ = nullptr;
    other.size_ = 0;
    return *this;
  }

    void fill(const std::vector<int>& values) {
        delete[] data_;
        size_ = static_cast<int>(values.size());
        data_ = size_ > 0 ? new int[size_] : nullptr;
        for (int i = 0; i < size_; ++i) data_[i] = values[i];
    }
    int size() const { return size_; }
    int at(int i) const { return data_[i]; }

private:
    int* data_;
    int size_;
};

static std::string show(const IntBuffer& b) {
    std::string out = "[";
    for (int i = 0; i < b.size(); ++i) {
        if (i > 0) out += ", ";
        out += std::to_string(b.at(i));
    }
    return out + "]";
}

static void report(const std::string& label) {
    std::cout << label << ": copies=" << copies << " moves=" << moves;
}

static void run() {
    std::map<std::string, IntBuffer> slots;
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream in(line);
        std::string cmd;
        if (!(in >> cmd)) continue;
        copies = 0;
        moves = 0;
        if (cmd == "make") {
            std::string s;
            int n;
            in >> s >> n;
            std::vector<int> values(n);
            for (int i = 0; i < n; ++i) in >> values[i];
            slots[s].fill(values);
        } else if (cmd == "show") {
            std::string s;
            in >> s;
            std::cout << s << " = " << show(slots[s]) << "\n";
        } else if (cmd == "push_copy") {
            std::string s;
            int n;
            in >> s >> n;
            std::vector<IntBuffer> bag;
            for (int i = 0; i < n; ++i) bag.push_back(slots[s]);
            report("push_copy");
            std::cout << "\n";
        } else if (cmd == "push_move") {
            std::string s;
            in >> s;
            std::vector<IntBuffer> bag;
            //bag.push_back(slots[s]);  // TODO: hand the buffer over, do not copy it
            bag.push_back(std::move(slots[s]));  // TODO: hand the buffer over, do not copy it
            report("push_move");
            std::cout << " src=" << slots[s].size() << "\n";
        } else if (cmd == "grow") {
            int n;
            in >> n;
            std::vector<IntBuffer> bag;
            for (int i = 0; i < n; ++i) bag.push_back(IntBuffer(2));
            report("grow " + std::to_string(n));
            std::cout << "\n";
        } else if (cmd == "take") {
            std::string from, to;
            in >> from >> to;
            //slots[to] = slots[from];  // TODO: hand the buffer over, do not copy it
            slots[to] = std::move(slots[from]);  // TODO: hand the buffer over, do not copy it
            report("take");
            std::cout << " src=" << slots[from].size() << "\n";
        } else if (cmd == "swap") {
            std::string x, y;
            in >> x >> y;
            std::swap(slots[x], slots[y]);
            report("swap");
            std::cout << "\n";
        }
    }
}

int main() {
    run();
    std::cout << "live=" << live << "\n";
    return 0;
}


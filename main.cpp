#include <iostream>
#include <string>
#include <cstdlib>
#include <cstddef>

// ---- GIVEN, do not edit: replaces the library's new[] and delete[] so every block is counted ----
static int g_live = 0;
static void* g_blocks[256];

void* operator new[](std::size_t n) {
    void* p = std::malloc(n ? n : 1);
    if (!p) std::exit(3);               // allocation can fail; this program just stops
    for (int i = 0; i < 256; ++i) {
        if (!g_blocks[i]) { g_blocks[i] = p; break; }
    }
    ++g_live;
    return p;
}
static void release_block(void* p) {
    if (!p) return;                      // delete[] on nullptr does nothing
    for (int i = 0; i < 256; ++i) {
        if (g_blocks[i] == p) { g_blocks[i] = nullptr; --g_live; std::free(p); return; }
    }
    std::cout << "double delete[] detected" << std::endl;
    std::abort();
}
void operator delete[](void* p) noexcept { release_block(p); }
void operator delete[](void* p, std::size_t) noexcept { release_block(p); }
// ---- end of given code ----

struct IntArray {
    int* data;   // nullptr while cap == 0
    int size;    // elements in use
    int cap;     // elements the block can hold
};

// Append value. A full block (size == cap) must first grow: capacity 0 -> 1, then doubles.
void push(IntArray& a, int value) {
  // TODO: grow the block when it is full, then store value
  bool resizeNeeded = false;
  if(a.size + 1 > a.cap) {
    a.cap = a.cap == 0 ? 1 : a.cap*2;
    resizeNeeded=true;
  }

  if(resizeNeeded) {
    int* bigger = new int[a.cap];
    for(int i=0; i<a.size; ++i) {
      bigger[i] = a.data[i];
    }
    delete[] a.data, a.size;
    a.data = bigger;
  }
  a.data[a.size] = value;
  a.size += 1;
}

// Make cap equal to size. With nothing stored, release the block entirely.
void shrink(IntArray& a) {
  // TODO: give back the unused room
  if(a.cap > a.size) {
    int* smaller = new int[a.size];
    for(int i=0; i<a.size; ++i) {
      smaller[i] = a.data[i];
    }
    delete[] a.data, a.cap;
    a.data = smaller;
    a.cap = a.size;
  }
}

// Release the block and leave a valid empty array behind.
void destroy(IntArray& a) {
  // TODO: release the block

  //if(a.size > 0) {
    delete[] a.data, a.size;
    a.data = nullptr;
    a.size = 0;
    a.cap = 0;
  //}
}

static void status(const IntArray& a) {
    std::cout << "size=" << a.size << " cap=" << a.cap << " live=" << g_live << "\n";
}

int main() {
    IntArray a = {nullptr, 0, 0};
    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "push") {
            int x;
            std::cin >> x;
            push(a, x);
            std::cout << "push " << x << " -> ";
            status(a);
        } else if (cmd == "pop") {
            if (a.size == 0) {
                std::cout << "pop -> empty\n";
            } else {
                a.size -= 1;
                std::cout << "pop -> " << a.data[a.size] << " size=" << a.size << " cap=" << a.cap << "\n";
            }
        } else if (cmd == "get") {
            int i;
            std::cin >> i;
            if (i < 0 || i >= a.size) std::cout << "get " << i << " -> out of range\n";
            else std::cout << "get " << i << " -> " << a.data[i] << "\n";
        } else if (cmd == "print") {
            std::cout << "[";
            for (int i = 0; i < a.size; ++i) std::cout << (i ? " " : "") << a.data[i];
            std::cout << "] size=" << a.size << " cap=" << a.cap << "\n";
        } else if (cmd == "shrink") {
            shrink(a);
            std::cout << "shrink -> ";
            status(a);
        } else if (cmd == "clear") {
            destroy(a);
            std::cout << "clear -> ";
            status(a);
        }
    }
    destroy(a);
    std::cout << "live=" << g_live << "\n";
    return 0;
}


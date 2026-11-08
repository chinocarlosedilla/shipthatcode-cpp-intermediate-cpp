#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

class Resource {
public:
    explicit Resource(const std::string& name) : name_(name) {
        std::cout << "open " << name_ << "\n";
    }
    ~Resource() { std::cout << "close " << name_ << "\n"; }
    const std::string& name() const { return name_; }

private:
    std::string name_;
};

// Hands a fresh, solely-owned Resource to the caller.
std::unique_ptr<Resource> build(const std::string& name) {
  return std::make_unique<Resource>(name);
}

int main() {
  std::map<std::string, std::shared_ptr<Resource>> handles;
  std::map<std::string, std::weak_ptr<Resource>> observers;

  std::string line;
  while (std::getline(std::cin, line)) {
    std::istringstream in(line);
    std::string cmd, a, b;
    in >> cmd >> a >> b;   // unused words stay empty

    if (cmd == "open") {
      // TODO: refuse a name that is already open, otherwise build it and take ownership
      handles[a] = build(a);
    } else if (cmd == "share") {
      // TODO: b joins the ownership of a
      handles[b] = handles[a];
    } else if (cmd == "drop") {
      // TODO: release the handle named a
      handles.erase(a);
    } else if (cmd == "watch") {
      // TODO: observer b watches the object behind a, without owning it
      observers[b] = handles[a];
    } else if (cmd == "peek") {
      // TODO: report what observer a sees
      if(observers[a].lock() == nullptr) {
        std::cout << "expired" << std::endl;
      }
      else {
        std::cout << "alive " << observers[a].lock()->name() << " owners=" << observers[a].use_count()-1 << std::endl;
      }
    } else if (cmd == "owners") {
      // TODO: report how many handles own a
      std::cout << "owners " << a << "=" << handles[a].use_count() << std::endl;
    }
  }

  // TODO: release every remaining handle, in sorted-name order
  return 0;
}


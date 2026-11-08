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
      if(handles.count(a)) {
        std::cout << "error " << a << " exists" << std::endl;
        continue;
      }
      handles[a] = build(a);
    } else if (cmd == "share") {
      // TODO: b joins the ownership of a
      if(handles.count(a) == 0 ) {
        std::cout << "error " << a << " missing" << std::endl;
        continue;
      }
      if(handles.count(b)) {
        std::cout << "error " << b << " exists" << std::endl;
        continue;
      }
      handles[b] = handles[a];
    } else if (cmd == "drop") {
      // TODO: release the handle named a
      if(handles.count(a) == 0) {
        std::cout << "error " << a << " missing" << std::endl;
        continue;
      }
      handles.erase(a);
    } else if (cmd == "watch") {
      // TODO: observer b watches the object behind a, without owning it
      if(handles.count(a) == 0) {
        std::cout << "error " << a << " missing" << std::endl;
        continue;
      }
      
      observers[b] = handles[a];
    } else if (cmd == "peek") {
      // TODO: report what observer a sees
      if(observers.count(a) == 0) {
        std::cout << "error " << a << " unknown" << std::endl;
      }
      else if(observers[a].lock() == nullptr) {
        std::cout << "expired" << std::endl;
      }
      else {
        std::cout << "alive " << observers[a].lock()->name() << " owners=" << observers[a].use_count()-1 << std::endl;
      }
    } else if (cmd == "owners") {
      // TODO: report how many handles own a
      int count = handles.count(a) == 0 ? 0 : handles[a].use_count();
      std::cout << "owners " << a << "=" << count << std::endl;
    }
  }

  // TODO: release every remaining handle, in sorted-name order
  //std::map<std::string, std::shared_ptr<Resource>> handles;
  //for(std::map<std::string, std::shared_ptr<Resource>>::iterator i = handles.begin();
  //  i < handles.end(); ++i) {
  //  i->reset();
  //}
  //handles.clear();
  for(auto &[key, ptr] : handles) {
    ptr.reset();
  }
  return 0;
}


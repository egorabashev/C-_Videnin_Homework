#include <algorithm>
#include <cctype>
#include <exception>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;
class FleetError : public exception {
public:
    explicit FleetError(string msg) : msg_(move(msg)) {}
    const char* what() const noexcept override { return msg_.c_str(); }
private:
    string msg_;
};
class InspectionError : public FleetError {
public:
    InspectionError() : FleetError("inspection rejected") {}
};
class ArgumentError : public FleetError {
public:
    explicit ArgumentError(const string& msg) : FleetError(msg) {}
};
struct ModelSpec {
    string kind;
    int wheels;
};
class InventoryItem {
public:
    virtual ~InventoryItem() = default;
    virtual string inventoryId() const = 0;
    virtual string name() const = 0;
    virtual string describe() const = 0;
    virtual double energyPerDay() const { return 0.0; }
    virtual bool isNoviceFriendly() const { return false; }
};
ostream& operator<<(ostream& os, const InventoryItem& item) {
    return os << item.inventoryId() << ' ' << item.name();
}
bool operator<(const InventoryItem& a, const InventoryItem& b) {
    return a.inventoryId() < b.inventoryId();
}
class EnergyConsumer {
public:
    virtual ~EnergyConsumer() = default;
    virtual double dailyEnergy() const = 0;
};
class Transport : public InventoryItem {
public:
    Transport(string id, string name, int score, shared_ptr<const ModelSpec> spec)
        : id_(move(id)), name_(move(name)), score_(score), spec_(move(spec)) {}
    bool isNoviceFriendly() const override { return score_ >= 6; }
    string inventoryId() const override { return id_; }
    string name() const override { return name_; }
    int score() const { return score_; }
protected:
    string id_, name_;
    int score_;
    shared_ptr<const ModelSpec> spec_;
};
class Thing : public InventoryItem {
public:
    Thing(string id, string name) : id_(move(id)), name_(move(name)) {}
    string inventoryId() const override { return id_; }
    string name() const override { return name_; }
protected:
    string id_, name_;
};
class ElectricScooter : public Transport, public EnergyConsumer {
public:
    ElectricScooter(string id, string name, double energy, int score, shared_ptr<const ModelSpec> spec)
        : Transport(move(id), move(name), score, move(spec)), energy_(energy) {}
    double dailyEnergy() const override { return energy_; }
    double energyPerDay() const override { return dailyEnergy(); }
    string describe() const override { return "Scooter " + name_; }
private:
    double energy_;
};
class ElectricBike : public Transport, public EnergyConsumer {
public:
    ElectricBike(string id, string name, double energy, int score, shared_ptr<const ModelSpec> spec)
        : Transport(move(id), move(name), score, move(spec)), energy_(energy) {}
    double dailyEnergy() const override { return energy_; }
    double energyPerDay() const override { return dailyEnergy(); }
    string describe() const override { return "eBike " + name_; }
private:
    double energy_;
};
class Bike : public Transport {
public:
    Bike(string id, string name, int score, shared_ptr<const ModelSpec> spec)
        : Transport(move(id), move(name), score, move(spec)) {}
    string describe() const override { return "Bike " + name_; }
};
class Monowheel : public Transport, public EnergyConsumer {
public:
    Monowheel(string id, string name, double energy, int score, shared_ptr<const ModelSpec> spec)
        : Transport(move(id), move(name), score, move(spec)), energy_(energy) {}
    double dailyEnergy() const override { return energy_; }
    double energyPerDay() const override { return dailyEnergy(); }
    string describe() const override { return "Monowheel " + name_; }
private:
    double energy_;
};
class Helmet : public Thing {
public:
    Helmet(string id, string name) : Thing(move(id), move(name)) {}
    string describe() const override { return "Helmet " + name_; }
};
class Dock : public Thing, public EnergyConsumer {
public:
    Dock(string id, string name, double energy)
        : Thing(move(id), move(name)), energy_(energy) {}
    double dailyEnergy() const override { return energy_; }
    double energyPerDay() const override { return dailyEnergy(); }
    string describe() const override { return "Dock " + name_; }
private:
    double energy_;
};
template <typename T, int Cap>
class Registry {
public:
    void add(const T& x) {
        if (static_cast<int>(items_.size()) == Cap) items_.erase(items_.begin());
        items_.push_back(x);
    }
    int size() const { return static_cast<int>(items_.size()); }
    const T& operator[](int i) const { return items_.at(i); }

    friend ostream& operator<<(ostream& os, const Registry& r) {
        for (const auto& x : r.items_) os << x << '\n';
        return os;
    }
private:
    vector<T> items_;
};
template <typename Container>
double totalEnergy(const Container& c) {
    double sum = 0.0;
    for (const auto& p : c) sum += p->energyPerDay();
    return sum;
}
class ServiceCenter {
public:
    static void inspect(const InventoryItem& item) {
        string lower = item.name();
        for (char& ch : lower) ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
        if (lower.find("broken") != string::npos) throw InspectionError();
    }
};
class Fleet {
public:
    Fleet() {
        specs_["scooter"]   = make_shared<ModelSpec>(ModelSpec{"scooter", 2});
        specs_["ebike"]     = make_shared<ModelSpec>(ModelSpec{"ebike", 2});
        specs_["bike"]      = make_shared<ModelSpec>(ModelSpec{"bike", 2});
        specs_["monowheel"] = make_shared<ModelSpec>(ModelSpec{"monowheel", 1});
    }
    shared_ptr<const ModelSpec> spec(const string& kind) const { return specs_.at(kind); }
    void addTransport(unique_ptr<InventoryItem> item) {
        checkNew(*item);
        ServiceCenter::inspect(*item);
        inspectionLog_.add("accepted " + item->inventoryId());
        push(move(item));
    }
    void addThing(unique_ptr<InventoryItem> item) {
        checkNew(*item);
        push(move(item));
    }
    void remove(const string& id) {
        auto it = find_if(entries_.begin(), entries_.end(),
                          [&](const Entry& e) { return e.item->inventoryId() == id; });
        if (it == entries_.end()) throw ArgumentError("no item " + id);
        entries_.erase(it);
    }
    void focus(const string& id) {
        for (const auto& e : entries_)
            if (e.item->inventoryId() == id) { focus_ = e.token; return; }
        throw ArgumentError("no item " + id);
    }
    void showFocus(ostream& os) const {
        if (auto t = focus_.lock()) os << **t << '\n';
        else os << (focusWasSet() ? "removed" : "no focus") << '\n';
    }
    void list(ostream& os) const {
        vector<const InventoryItem*> v = pointers();
        sort(v.begin(), v.end(), [](const InventoryItem* a, const InventoryItem* b) { return *a < *b; });
        for (const auto* p : v) os << *p << '\n';
    }
    void novice(ostream& os) const {
        for (const auto* p : pointers())
            if (p->isNoviceFriendly()) os << *p << '\n';
    }
    double energy() const { return totalEnergy(pointers()); }
    int count() const { return static_cast<int>(entries_.size()); }
    void maxEnergy(ostream& os) const {
        const InventoryItem* best = nullptr;
        for (const auto* p : pointers())
            if (!best || p->energyPerDay() > best->energyPerDay()) best = p;
        if (!best) os << "empty\n";
        else os << *best << ' ' << fixed << setprecision(1) << best->energyPerDay() << '\n';
    }
    const Registry<string, 10>& inspectionLog() const { return inspectionLog_; }
private:
    struct Entry {
        unique_ptr<InventoryItem> item;
        shared_ptr<const InventoryItem*> token;
    };
    void checkNew(const InventoryItem& item) const {
        if (item.inventoryId().empty()) throw ArgumentError("empty id");
        for (const auto& e : entries_)
            if (e.item->inventoryId() == item.inventoryId())
                throw ArgumentError("duplicate id " + item.inventoryId());
    }
    void push(unique_ptr<InventoryItem> item) {
        auto token = make_shared<const InventoryItem*>(item.get());
        entries_.push_back(Entry{move(item), move(token)});
    }
    vector<const InventoryItem*> pointers() const {
        vector<const InventoryItem*> v;
        for (const auto& e : entries_) v.push_back(e.item.get());
        return v;
    }
    bool focusWasSet() const {
        weak_ptr<const InventoryItem*> empty;
        return focus_.owner_before(empty) || empty.owner_before(focus_);
    }
    vector<Entry> entries_;
    weak_ptr<const InventoryItem*> focus_;
    unordered_map<string, shared_ptr<ModelSpec>> specs_;
    Registry<string, 10> inspectionLog_;
};
int parseScore(const string& s) {
    int v;
    try { size_t pos; v = stoi(s, &pos); if (pos != s.size()) throw 0; }
    catch (...) { throw ArgumentError("bad score " + s); }
    if (v < 1 || v > 10) throw ArgumentError("score must be 1..10");
    return v;
}
double parseEnergy(const string& s) {
    double v;
    try { size_t pos; v = stod(s, &pos); if (pos != s.size()) throw 0; }
    catch (...) { throw ArgumentError("bad energy " + s); }
    if (v < 0) throw ArgumentError("energy must be >= 0");
    return v;
}
void needArgs(const vector<string>& w, size_t n) {
    if (w.size() != n) throw ArgumentError("wrong number of arguments");
}
void handleAdd(Fleet& fleet, const vector<string>& w) {
    if (w.size() < 2) throw ArgumentError("add: missing type");
    const string& type = w[1];
    if (type == "scooter") {
        needArgs(w, 6);
        fleet.addTransport(make_unique<ElectricScooter>(w[2], w[3], parseEnergy(w[4]), parseScore(w[5]), fleet.spec("scooter")));
    } else if (type == "ebike") {
        needArgs(w, 6);
        fleet.addTransport(make_unique<ElectricBike>(w[2], w[3], parseEnergy(w[4]), parseScore(w[5]), fleet.spec("ebike")));
    } else if (type == "monowheel") {
        needArgs(w, 6);
        fleet.addTransport(make_unique<Monowheel>(w[2], w[3], parseEnergy(w[4]), parseScore(w[5]), fleet.spec("monowheel")));
    } else if (type == "bike") {
        needArgs(w, 5);
        fleet.addTransport(make_unique<Bike>(w[2], w[3], parseScore(w[4]), fleet.spec("bike")));
    } else if (type == "helmet") {
        needArgs(w, 4);
        fleet.addThing(make_unique<Helmet>(w[2], w[3]));
    } else if (type == "dock") {
        needArgs(w, 5);
        fleet.addThing(make_unique<Dock>(w[2], w[3], parseEnergy(w[4])));
    } else {
        throw ArgumentError("unknown type " + type);
    }
}
int main() {
    Fleet fleet;
    string line;
    while (getline(cin, line)) {
        istringstream ss(line);
        vector<string> w;
        for (string t; ss >> t;) w.push_back(t);
        if (w.empty()) continue;
        const string& cmd = w[0];
        try {
            if (cmd == "quit") break;
            else if (cmd == "add") handleAdd(fleet, w);
            else if (cmd == "energy") { needArgs(w, 1); cout << fixed << setprecision(1) << fleet.energy() << '\n'; }
            else if (cmd == "novice") { needArgs(w, 1); fleet.novice(cout); }
            else if (cmd == "list") { needArgs(w, 1); fleet.list(cout); }
            else if (cmd == "focus") { needArgs(w, 2); fleet.focus(w[1]); }
            else if (cmd == "show_focus") { needArgs(w, 1); fleet.showFocus(cout); }
            else if (cmd == "remove") { needArgs(w, 2); fleet.remove(w[1]); }
            else if (cmd == "report") {
                needArgs(w, 1);
                cout << fleet.count() << ' ' << fixed << setprecision(1) << fleet.energy() << '\n';
            }
            else if (cmd == "max_energy") { needArgs(w, 1); fleet.maxEnergy(cout); }
            else if (cmd == "log") { needArgs(w, 1); cout << fleet.inspectionLog(); }
            else throw ArgumentError("unknown command " + cmd);
        } catch (const FleetError& e) {
            cout << "error: " << e.what() << '\n';
        }
    }
}

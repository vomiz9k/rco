#include <vector>
#include <string>
#include <set>
#include <map>
#include <fstream>

class Order {
public:
    Order() = default;

private:
    enum class OrderType { BUY, SELL };

    OrderType getType() { return order_type; }
    double getPrice() { return price; }
    size_t getCount() { return count; }
    size_t getId() { return id; }

    void decreaseCount(size_t diff) { count -= diff; }

    Order(std::vector<std::string>& tokens);

    size_t id;
    double price;
    OrderType order_type;
    size_t count;

    friend class Exchange;
};

class Exchange {
public:
    Exchange(const std::string& input, const std::string& output) 
    : input(input), output(output) {}

    void start();

private:
    
    template<typename Iter>
    void performTrades(Iter& iter, Iter& iter_end, Order& order, char type);
    void removeOrder(size_t id);
    void tryResolveOrder(Order& order);
    void insertOrder(Order& order);

    std::map<size_t, Order> orders;
    std::map<double, std::set<size_t>> sell_orders;
    std::map<double, std::set<size_t>> buy_orders;
    std::ofstream output;
    std::ifstream input;
    size_t trades_num = 1;
};





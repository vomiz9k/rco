#include <vector>
#include <string>
#include <set>
#include <map>
#include <fstream>
#include "Exchange.h"

Order::Order(std::vector<std::string>& tokens) {
    id = std::atoi(tokens[1].c_str());
    if (tokens[2] == "B") {
        order_type = OrderType::BUY;
    } else {
        order_type = OrderType::SELL;
    }
    count = std::atoi(tokens[3].c_str());
    price = std::stod(tokens[4].c_str());
}

void Exchange::start() {
    std::string s;
    while (input >> s) {
        std::vector<std::string> tokens;
        int pos;
        while ((pos = s.find(",")) != std::string::npos) {
            tokens.push_back(s.substr(0, pos));
            s.erase(0, pos + 1);
        }
        tokens.push_back(s);

        if (tokens[0] == "C") {
            removeOrder(std::atoi(tokens[1].c_str()));
        } else {
            Order curr_order(tokens);

            tryResolveOrder(curr_order);
            if (curr_order.getCount() > 0) {
                insertOrder(curr_order);
            }              
        }
    }
}

void Exchange::removeOrder(size_t id) {
    auto removed_order_iter = orders.find(id);
    if (removed_order_iter == orders.end()) {
        return;
    }
    std::map<double, std::set<size_t>>* needed_orders;

    if (removed_order_iter->second.getType() == Order::OrderType::BUY) {
        needed_orders = &buy_orders;
    } else {
        needed_orders = &sell_orders;
    }
    
    (*needed_orders)[removed_order_iter->second.getPrice()].erase(removed_order_iter->first);

    if ((*needed_orders)[removed_order_iter->second.getPrice()].empty()) {
        needed_orders->erase(removed_order_iter->second.getPrice());
    }

    orders.erase(removed_order_iter);        
}

template<typename Iter>
void Exchange::performTrades(Iter& iter, Iter& iter_end, Order& order, char type) {
    std::vector<size_t> orders_to_remove;

    while (iter != iter_end && order.getCount() > 0 
        && (iter->first >= order.getPrice() && type == 'B' 
        || iter->first <= order.getPrice() && type == 'S')) {
        for(auto id: iter->second) {
            Order& offering_order = orders[id];

            size_t count = offering_order.getCount() > order.getCount() ? 
                order.getCount() : offering_order.getCount();

            output << "T," << trades_num << "," << type << ","  << offering_order.getId() << "," << 
                order.getId() << "," << count << "," << offering_order.getPrice() << std::endl;

            ++trades_num;
            
            order.decreaseCount(count);
            offering_order.decreaseCount(count);

            if (offering_order.getCount() == 0) {
                orders_to_remove.push_back(offering_order.getId());
            }
            if (order.getCount() == 0) {
                break;
            }
        }
        ++iter;
    }

    for(auto id: orders_to_remove) {
        removeOrder(id);
    }        
}

void Exchange::tryResolveOrder(Order& order) {        
    if (order.getType() == Order::OrderType::BUY) {
        auto iter_start = sell_orders.begin();
        auto iter_end = sell_orders.end();
        performTrades(iter_start, iter_end, order, 'S');            
    } else {
        auto iter_start = buy_orders.rbegin();
        auto iter_end = buy_orders.rend();
        performTrades(iter_start, iter_end, order, 'B');  
    }
}

void Exchange::insertOrder(Order& order) {
    orders[order.getId()] = order;
    if (order.getType() == Order::OrderType::BUY) {
        buy_orders[order.getPrice()].insert(order.getId());
    } else {
        sell_orders[order.getPrice()].insert(order.getId());
    }
}

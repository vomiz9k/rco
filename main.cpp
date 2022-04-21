#include "Exchange.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return 1;
    }

    Exchange exchange(argv[1], argv[2]);
    exchange.start();    
}
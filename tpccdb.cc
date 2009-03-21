#include <cassert>

#include "tpccdb.h"

void Address::copy(char* street1, char* street2, char* city, char* state, char* zip,
        const char* src_street1, const char* src_street2, const char* src_city,
        const char* src_state, const char* src_zip) {
    // TODO: Just do one copy since all the fields should be contiguous?
    memcpy(street1, src_street1, MAX_STREET+1);
    memcpy(street2, src_street2, MAX_STREET+1);
    memcpy(city, src_city, MAX_CITY+1);
    memcpy(state, src_state, STATE+1);
    memcpy(zip, src_zip, ZIP+1);
}

// Non-integral constants must be defined in a .cc file. Needed for Mac OS X.
// http://www.research.att.com/~bs/bs_faq2.html#in-class
const float Item::MIN_PRICE;
const float Item::MAX_PRICE;
const float Warehouse::MIN_TAX;
const float Warehouse::MAX_TAX;
const float Warehouse::INITIAL_YTD;
const float District::MIN_TAX;
const float District::MAX_TAX;
const float District::INITIAL_YTD;  // different from Warehouse
const float Customer::MIN_DISCOUNT;
const float Customer::MAX_DISCOUNT;
const float Customer::INITIAL_BALANCE;
const float Customer::INITIAL_CREDIT_LIM;
const float Customer::INITIAL_YTD_PAYMENT;
const char Customer::GOOD_CREDIT[] = "GC";
const char Customer::BAD_CREDIT[] = "BC";
const float OrderLine::MIN_AMOUNT;
const float OrderLine::MAX_AMOUNT;
const char NewOrderOutput::INVALID_ITEM_STATUS[] = "Item number is not valid";
const float History::INITIAL_AMOUNT;

TPCCDB::WarehouseSet TPCCDB::newOrderRemoteWarehouses(int32_t home_warehouse,
        const std::vector<NewOrderItem>& items) {
    WarehouseSet out;
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i].ol_supply_w_id != home_warehouse) {
            out.insert(items[i].ol_supply_w_id);
        }
    }
    return out;
}

void TPCCDB::newOrderCombine(const std::vector<NewOrderItem>& items, int32_t warehouse_id,
        const std::vector<int32_t>& remote_quantities, NewOrderOutput* output) {
    assert(remote_quantities.size() == output->items.size());
    assert(remote_quantities.size() == items.size());
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i].ol_supply_w_id != warehouse_id) {
            assert(remote_quantities[i] == 0);
        } else {
            assert(output->items[i].s_quantity == 0);
            output->items[i].s_quantity = remote_quantities[i];
        }
    }
}

// TODO: These macros are copied from tpcctables.cc. Is there a way to share them?
#define COPY_ADDRESS(src, dest, prefix) \
    Address::copy( \
            dest->prefix ## street_1, dest->prefix ## street_2, dest->prefix ## city, \
            dest->prefix ## state, dest->prefix ## zip,\
            src.prefix ## street_1, src.prefix ## street_2, src.prefix ## city, \
            src.prefix ## state, src.prefix ## zip)

#define COPY_STRING(dest, src, field) memcpy(dest->field, src.field, sizeof(src.field))

// Copy the customer fields from remote into local
void TPCCDB::paymentCombine(const PaymentOutput& remote, PaymentOutput* home) {
    home->c_credit_lim = remote.c_credit_lim;
    home->c_discount = remote.c_discount;
    home->c_balance = remote.c_balance;
    COPY_STRING(home, remote, c_first);
    COPY_STRING(home, remote, c_middle);
    COPY_STRING(home, remote, c_last);
    COPY_ADDRESS(remote, home, c_);
    COPY_STRING(home, remote, c_phone);
    COPY_STRING(home, remote, c_since);
    COPY_STRING(home, remote, c_credit);
    COPY_STRING(home, remote, c_data);
}

#undef COPY_STRING
#undef COPY_ADDRESS

#include<iostream>
#include<vector>
#include<string>
#include<sqlite3.h>

using namespace std;

class Item {
private:
    string name;
    double price;
public:
    Item(const string& itemName, double itemPrice) : name(itemName), price(itemPrice) {}
    string getName() const {
        return name;
    }
    double getPrice() const {
        return price;
    }
};
class ShoppingCart {
private:
    vector<Item> items;
    string custName;
public:
    ShoppingCart(const string& customerName) : custName(customerName) {}
    void addItem(const Item& item) {
        items.push_back(item);
    }

    void removeItem(const string& itemName) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->getName() == itemName) {
                items.erase(it);
                break;
            }
        }
    }

    void displayCart() const {
        if (items.empty()) {
            cout << "Your Cart is Empty.!!" << endl;
        }
        else {
            cout << "Items in the Cart :\n";
            for (const auto& item : items) {
                cout << " - " << item.getName() << ": $" << item.getPrice() << endl;
            }

        }
    }
    const vector<Item>& getItems() const {
        return items;
    }

    bool isEmpty() const {
        return items.empty();
    }

    string getCustName() const {
        return custName;
    }
};

class DatabaseHelper {
private:
    sqlite3* db;
    char* errMsg;
public:
    DatabaseHelper() {
        if (sqlite3_open("cart.db", &db) != SQLITE_OK) {
            cerr << "Error Opening Database :" << sqlite3_errmsg(db) << endl;
            sqlite3_close(db);
        }
        else {
            if (sqlite3_open("cart.db", &db) == SQLITE_OK) {
                cout << "DataBase Created Successfully" << endl;
            }
            else {
                cerr << "Error Creating Database:" << sqlite3_errmsg(db) << endl;
            }
        }
    }

    ~DatabaseHelper() {
        sqlite3_close(db);
    }

    static int saveCallback(void*, int, char**, char**) {
        return 0;
    }

void saveCart(const ShoppingCart& cart) {
    string sql = "CREATE TABLE IF NOT EXISTS Cart (CustName TEXT, Name TEXT, Price REAL);";
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "SQL Error: " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    for (const auto& item : cart.getItems()) {
        // Check if the item already exists in the cart
        sql = "SELECT COUNT(*) FROM Cart WHERE CustName = '" + cart.getCustName() + "' AND Name = '" + item.getName() + "';";
        sqlite3_stmt* stmt;
        int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (result == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int count = sqlite3_column_int(stmt, 0);
                if (count == 0) {
                    // Item does not exist, insert it into the cart
                    sql = "INSERT INTO Cart(CustName, Name, Price) VALUES ('" + cart.getCustName() + "', '" + item.getName() + "', " + to_string(item.getPrice()) + ");";
                    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
                        cerr << "SQL Error: " << errMsg << endl;
                        sqlite3_free(errMsg);
                    }
                }
            }
            sqlite3_finalize(stmt);
        } else {
            cerr << "SQL Error: " << sqlite3_errmsg(db) << endl;
        }
    }
}



    ShoppingCart loadCart(const string& customerName) {
        ShoppingCart cart(customerName);
        string sql = "SELECT Name, Price FROM Cart WHERE CustName = '" + customerName + "';";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                double price = sqlite3_column_double(stmt, 1);
                cart.addItem(Item(name, price));
            }
            sqlite3_finalize(stmt);
        }
        else {
            cerr << "SQL Error :" << sqlite3_errmsg(db) << endl;
        }

        return cart;
    }

};

class ShoppingCartDB {
private:
    DatabaseHelper dbhelper;
public:
    void addToCart(ShoppingCart& cart, const Item& item) {
        cart.addItem(item);
        dbhelper.saveCart(cart);
    }

    void removeFromCart(ShoppingCart& cart, const string& itemName) {
        cart.removeItem(itemName);
        dbhelper.saveCart(cart);
    }

    ShoppingCart loadCart(const string& customerName) {
        return dbhelper.loadCart(customerName);
    }

};

char userUI() {
    cout << "\n=== Shopping Cart ===" << endl;
    cout << "1. View Cart" << endl;
    cout << "2. Add Item" << endl;
    cout << "3. Remove Item" << endl;
    cout << "4. CheckOut" << endl;
    cout << "5. Exit" << endl;
    cout << "Choose an option: ";

    char choice;
    cin >> choice;
    return choice;

}

void checkOut(const ShoppingCart& cart, const string customerName) {

    cout << "\n=== Checkout ===" << endl;
    cout << "Customer Name: " << customerName << endl;
    cout << "Items in Cart: " << endl;
    for (const auto& item : cart.getItems()) {
        cout << "- " << item.getName() << ": $" << item.getPrice() << endl;
    }
    float total = 0.0;
    for (const auto& item : cart.getItems()) {
        total += item.getPrice();
    }
    cout << "Total Cost: $" << total << endl;

}

int main() {
    ShoppingCartDB cartService;
    ShoppingCart cart("");
    char choice;
    string itemName;
    double itemPrice;
    string customerName;
    cout << "Enter your Name: ";
    cin.ignore();
    getline(cin, customerName);
    do {
        choice = userUI();
        switch (choice)
        {
        case '1':
            cart = cartService.loadCart(customerName);
            if (cart.isEmpty()) {
                cout << "Your cart is empty." << endl;
            }
            else {
                cart.displayCart();
            }
            break;
        case '2':
            cout << "Enter item name: ";
            cin >> itemName;
            cout << "Enter item price: ";
            cin >> itemPrice;
            cartService.addToCart(cart, Item(itemName, itemPrice));
            cout << "Item added to cart." << endl;
            break;
        case '3':
            if (cart.isEmpty()) {
                cout << "Cart is empty. Nothing to remove." << endl;
                break;
            }
            cout << "Enter Item name to be removed: ";
            cin >> itemName;
            cartService.removeFromCart(cart, itemName);
            cout << "Item removed from cart" << endl;
            break;
        case '4':
            if (cart.isEmpty()) {
                cout << "Cart is empty!! Cannot Proceed to checkout." << endl;
            }
            else {
                checkOut(cart, customerName);
            }
            break;
        case '5':
            cout << "Exiting..." << endl;
            break;
        default:
            cout << "Invalid Choice !!" << endl;
            break;
        }
    } while (choice != '5');
    return 0;
}

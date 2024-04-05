#include <iostream>
#include <vector>
#include <string>
#include <sqlite3.h>

using namespace std;

class Item {
private:
    string name;
    double price;
    int quantity;
public:
    Item(const string& itemName, double itemPrice, int itemQuantity) : name(itemName), price(itemPrice), quantity(itemQuantity) {}

   
    string getName() const {
        return name;
    }

    double getPrice() const {
        return price;
    }

    int getQuantity() const {
        return quantity;
    }

   
    void setQuantity(int newQuantity) {
        quantity = newQuantity;
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
        bool itemRemoved = false;
        for (auto it = items.begin(); it != items.end();) {
            if (it->getName() == itemName) {
                it = items.erase(it);
                itemRemoved = true;
            } else {
                ++it;
            }
        }
        if (itemRemoved) {
            cout << "Item '" << itemName << "' removed from cart." << endl;
        } else {
            cout << "Item '" << itemName << "' not found in cart." << endl;
        }
    }

    void displayCart() const {
        if (items.empty()) {
            cout << "Your Cart is Empty.!!" << endl;
        }
        else {
            cout << "Items in the Cart :\n";
            for (const auto& item : items) {
                cout << " - " << item.getName() << " (Quantity: " << item.getQuantity() << "): $" << item.getPrice() * item.getQuantity() << endl; 
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

class ItemsDatabaseHelper {
private:
    sqlite3* db;
    char* errMsg;
public:
    ItemsDatabaseHelper() {
        if (sqlite3_open("items.db", &db) != SQLITE_OK) {
            cerr << "Error Opening Items Database :" << sqlite3_errmsg(db) << endl;
            sqlite3_close(db);
        }
        else {
            if (sqlite3_open("items.db", &db) == SQLITE_OK) {
                cout << endl;
            }
            else {
                cerr << "Error Creating Items Database:" << sqlite3_errmsg(db) << endl;
            }
        }
    }

    ~ItemsDatabaseHelper() {
        sqlite3_close(db);
    }

    vector<Item> getMenuItems() {
        vector<Item> items;
        string sql = "SELECT Name, Price, Quantity FROM Items;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                double price = sqlite3_column_double(stmt, 1);
                int quantity = sqlite3_column_int(stmt, 2);
                items.push_back(Item(name, price, quantity));
            }
            sqlite3_finalize(stmt);
        }
        else {
            cerr << "SQL Error: " << sqlite3_errmsg(db) << endl;
        }

        return items;
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
                cout << endl;
            }
            else {
                cerr << "Error Creating Cart Database:" << sqlite3_errmsg(db) << endl;
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
        
        string createTableSQL = "CREATE TABLE IF NOT EXISTS Cart (CustName TEXT, Name TEXT, Price REAL, Quantity INTEGER);";
        if (sqlite3_exec(db, createTableSQL.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            cerr << "SQL Error: " << errMsg << endl;
            sqlite3_free(errMsg);
            return;
        }

        
        string removeSQL = "DELETE FROM Cart WHERE CustName = '" + cart.getCustName() + "';";
        if (sqlite3_exec(db, removeSQL.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            cerr << "SQL Error: " << errMsg << endl;
            sqlite3_free(errMsg);
            return;
        }

        
        for (const auto& item : cart.getItems()) {
            string insertSQL = "INSERT INTO Cart(CustName, Name, Price, Quantity) VALUES ('" + cart.getCustName() + "', '" + item.getName() + "', " + to_string(item.getPrice()) + ", " + to_string(item.getQuantity()) + ");";
            if (sqlite3_exec(db, insertSQL.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
                cerr << "SQL Error: " << errMsg << endl;
                sqlite3_free(errMsg);
                return;
            }
        }
    }

    ShoppingCart loadCart(const string& customerName) {
        ShoppingCart cart(customerName);
        string sql = "SELECT Name, Price, Quantity FROM Cart WHERE CustName = '" + customerName + "';";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                double price = sqlite3_column_double(stmt, 1);
                int quantity = sqlite3_column_int(stmt, 2);
                cart.addItem(Item(name, price, quantity));
            }
            sqlite3_finalize(stmt);
        }
        else {
            cerr << "SQL Error :" << sqlite3_errmsg(db) << endl;
        }

        return cart;
    }

    void clearUserCart(const string& customerName) {
        string sql = "DELETE FROM Cart WHERE CustName = '" + customerName + "';";
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            cerr << "SQL Error: " << errMsg << endl;
            sqlite3_free(errMsg);
        }
        else {
            cout<< endl;
        }
    }
};

class ShoppingCartDB {
private:
    DatabaseHelper dbhelper;
public:
    
    void addToCart(ShoppingCart&& cart, const Item& item) {
        cart.addItem(item);
        dbhelper.saveCart(cart);
    }

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

    void clearUserCart(const string& customerName) {
        dbhelper.clearUserCart(customerName);
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

void checkOut(ShoppingCart& cart, const string& customerName) {
    cout << "\n=== Checkout ===" << endl;
    cout << "Customer Name: " << customerName << endl;
    cout << "Items in Cart: " << endl;
    cart.displayCart();
    float total = 0.0;
    for (const auto& item : cart.getItems()) {
        total += item.getPrice() * item.getQuantity(); 
    }
    cout << "Total Cost: $" << total << endl;
    
    ShoppingCartDB cartService;
    cartService.clearUserCart(customerName);
}

int main() {
    ItemsDatabaseHelper itemsDbHelper;
    ShoppingCartDB cartService;
    char choice;
    string customerName;
    cout << "Enter your Name: ";
    cin>>customerName;
    do {
        choice = userUI();
        switch (choice)
        {
        case '1': {
            ShoppingCart cart = cartService.loadCart(customerName);
            if (cart.isEmpty()) {
                cout << "\nYour cart is empty." << endl;
            }
            else {
                cout << "\nItems in Your Cart:\n";
                cart.displayCart();
            }
            break;
        }
        case '2': {
            vector<Item> menuItems = itemsDbHelper.getMenuItems();
            if (menuItems.empty()) {
                cout << "\nNo items available in the menu." << endl;
            }
            else {
                cout << "\nChoose an item from the menu:\n";
                for (size_t i = 0; i < menuItems.size(); ++i) {
                    cout << i + 1 << ". " << menuItems[i].getName() << ": $" << menuItems[i].getPrice() << endl;
                }
                int choice;
                cout << "\nEnter your choice: ";
                cin >> choice;
                if (choice >= 1 && choice <= menuItems.size()) {
                    // Get quantity
                    int quantity;
                    cout << "Enter quantity: ";
                    cin >> quantity;
                    Item selected_item = menuItems[choice - 1];
                    selected_item.setQuantity(quantity);
                    cartService.addToCart(cartService.loadCart(customerName), selected_item);
                    cout << "Item added to cart." << endl;
                }
                else {
                    cout << "Invalid choice." << endl;
                }
            }
            break;
        }
        case '3': {
            ShoppingCart cart = cartService.loadCart(customerName);
            if (cart.isEmpty()) {
                cout << "\nCart is empty. Nothing to remove." << endl;
            }
            else {
                cart.displayCart();
                string itemName;
                cout << "\nEnter Item name to be removed: ";
                cin >> itemName;
                cartService.removeFromCart(cart, itemName);
            }
            break;
        }
        case '4': {
            ShoppingCart cart = cartService.loadCart(customerName);
            if (cart.isEmpty()) {
                cout << "Cart is empty!! Cannot Proceed to checkout." << endl;
            }
            else {
                checkOut(cart, customerName);
            }
            break;
        }
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

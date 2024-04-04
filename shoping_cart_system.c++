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
        Item(const string& itemName,double itemPrice) : name(itemName) , price(itemPrice) {}
        string getName() const{
            return name;
        }
        double getPrice() const{
            return price;
        }
};

class ShoppingCart {
    private:
        vector<Item> items;
    public:
        void addItem(const Item& item) {
            items.push_back(item);
        }

        void removeItem(const string& itemName) {
            for (auto it = items.begin(); it != items.end(); ++it) {
                if(it->getName() == itemName) {
                    items.erase(it);
                    break;
                }
            }
            
        }

        void displayCart() const {
            if(items.empty()) {
                cout<<"Your Cart is Empty.!!"<<endl;
            } else {
                cout<<"Items in the Cart :\n";
                for (const auto& item:items) {
                    cout<<" - "<<item.getName()<<": $"<<item.getPrice()<<endl;
                }
                
            }
        }
        const vector<Item>& getItems() const {
            return items;
        }
};

class DatabaseHelper {
    private:
        sqlite3* db;
        char* errMsg;
    public:
        DatabaseHelper() {
            if(sqlite3_open("cart.db",&db) != SQLITE_OK){
                cerr<<"Error Opening Database :"<<sqlite3_errmsg(db)<<endl;
                sqlite3_close(db);
            } else {
                if(sqlite3_open("cart.db",&db) == SQLITE_OK) {
                    cout<<"DataBase Created Successfully"<<endl;
                } else {
                    cerr<<"Error Creating Database:"<<sqlite3_errmsg(db)<<endl;
                } 
            }
        }

        ~DatabaseHelper() {
            sqlite3_close(db);
        }

        static int saveCallback(void* , int , char** , char**) {
            return 0;
        }

        void saveCart(const ShoppingCart& cart) {
            string sql = "CREATE TABLE IF NOT EXISTS Cart (Name TEXT, Price REAL);";
            if(sqlite3_exec(db,sql.c_str(),nullptr,nullptr,&errMsg)!= SQLITE_OK) {
                cerr<<"SQL Error :"<<errMsg<<endl;
                sqlite3_free(errMsg);
            }

            for (const auto& item : cart.getItems()) {
                sql = "INSERT INTO Cart(Name, Price) VALUES ('"+item.getName()+"',"+to_string(item.getPrice())+");";
                if(sqlite3_exec(db,sql.c_str(),saveCallback,nullptr,&errMsg) != SQLITE_OK) {
                    cerr<<"SQL Error :"<<errMsg<<endl;
                    sqlite3_free(errMsg);
                }
            }
        }
        ShoppingCart loadCart() {
            ShoppingCart cart;
            string sql = "SELECT Name, Price FROM Cart;";
            sqlite3_stmt* stmt;
            if(sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr) == SQLITE_OK) {
                while (sqlite3_step(stmt) == SQLITE_ROW)
                {
                    string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt,0));
                    double price = sqlite3_column_double(stmt,1);
                    cart.addItem(Item(name,price));
                }
                sqlite3_finalize(stmt);
            } else {
                cerr<<"SQL Error :"<<sqlite3_errmsg(db)<<endl;
            }

            return cart;
        }

};




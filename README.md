<center>

<h1 style="font-size: 36px;">🛒 <strong>Shopping Cart Readme</strong></h1>

</center>
### Description ℹ️
This C++ program implements a simple shopping cart system with database functionality. Users can add items to their cart, remove items, view their cart, and proceed to checkout. It utilizes SQLite for database operations, storing both menu items and user carts.

### Features ✨
1. **View Cart**: Users can view items currently in their shopping cart.
2. **Add Item**: Users can add items from a pre-defined menu to their cart.
3. **Remove Item**: Users can remove items from their cart.
4. **Checkout**: Users can proceed to checkout, displaying the items in their cart and the total cost.
5. **Database Storage**: Menu items and user carts are stored using SQLite databases.

### Usage 🚀
1. **Compile**: Compile the code using a C++ compiler. Make sure to link against SQLite.

🔧 Compilation Command
`g++ .\shopping_cart_system.cpp .\sqlite3\sqlite3.o -Ipath\to\sqlite3`

2. **Run**: Execute the compiled program.
3. **Options**:
   - Press `1` to view the cart.
   - Press `2` to add an item to the cart.
   - Press `3` to remove an item from the cart.
   - Press `4` to proceed to checkout.
   - Press `5` to exit.
4. **Checkout**: During checkout, the program displays the items in the cart and the total cost. The cart is then cleared.

For adding more items in the menu , edit the items.db Database.

### Dependencies 📦
- SQLite3: Ensure SQLite3 library is installed and linked during compilation.

### Database Schema 🗃️
1. **Items Table**: Stores menu items with columns for name, price, and quantity.
2. **Cart Table**: Stores user carts with columns for customer name, item name, price, and quantity.

### Compatibility 🖥️
- The program is written in C++ and should run on any platform with a compatible compiler.
- Ensure SQLite3 is available and compatible with the platform.

### Contributions 🤝
Contributions are welcome! Feel free to fork the repository, make improvements, and create a pull request.

### Support 💬
For any questions or support, please open an issue in the GitHub repository.

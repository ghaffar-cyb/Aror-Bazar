#include <iostream>
#include <fstream>
#include <array>
#include <string>
#include <sstream>
#include <thread> //Used to create and run multiple threads at the same time.
#include <chrono>//Used to work with time like delays and measuring time.
#include <conio.h>//Used for keyboard input functions, like _getch(), mostly in Windows.
#include <iomanip> // Used for formatting output, like setting width, precision, alignment.
#include <limits> // Gives information about data type limits, like max/min values.

using namespace std;


const int MAX_PRODUCTS = 100;
const int MAX_USERS = 100;
const int MAX_PURCHASES = 500; // Limit for tracking purchases

const string USER_DATA_FILE = "users.csv";
const string PRODUCT_DATA_FILE = "products.csv";
const string PURCHASE_DATA_FILE = "purchases.csv"; // New file for purchase history

const string ADMIN_NAME = "Ghaffar";
const string ADMIN_PASS = "1122"; 

struct UserInfo {
    string name;
    string pass;
    string address; 
};

struct Product {
    int id;
    string category; 
    string name;
    string description;
    int price;
};

struct Purchase {
    string userName;
    string productName;
    int price;
    string paymentMethod;
    string deliveryAddress;
};

// Global Arrays
array<UserInfo, MAX_USERS> users;
int numUsers = 0; 

array<Product, MAX_PRODUCTS> products;
int numProducts = 0; 

array<Purchase, MAX_PURCHASES> purchases;
int numPurchases = 0;

int loggedInUserIndex = -1;

// --- FUNCTION PROTOTYPES ---

void logo(const string& text, int num);
void logodisplay();
void displayMenu();
void loginAdmin();
void loginUser();
void registerUser();
void forget();
void adminMenu();
void viewUsers();
// NEW ADMIN FUNCTIONS
void viewProducts();
void viewPurchases();
// END NEW ADMIN FUNCTIONS
void userMenu();
void shoppingMenu();
void displayProducts(const string& category);
void processOrder(const Product& selectedProduct);

// Admin Product Management
void loadProducts();
void saveProducts();
void addProduct();
void removeProduct();
void updateProduct();

// User and Purchase CSV Helpers
void loadUsers();
void saveUsers();
void loadPurchases();
void savePurchase(const Purchase& newPurchase); // Only saves one new purchase

// --- MAIN FUNCTION ---

int main() {
    loadUsers();    
    loadProducts(); 
    loadPurchases(); // Load purchases on startup
    logodisplay();
    displayMenu();
    return 0;
}

// --- CORE UTILITY FUNCTIONS ---

void logo(const string& text, int num=0) {
    for (char c : text) {
        cout << c;
        this_thread::sleep_for(chrono::milliseconds(num));
    }
}

void logodisplay() {
    system("CLS");
    logo(" _______________________________________________________ \n", 1);
    logo("|                                                       |\n", 1);
    logo("|    _____________________________________* * * * * *   |\n", 1);
    logo("|   |                                       * * * * *   |\n", 1);
    logo("|   |                W E L C O M E            * * * *   |\n", 1);
    logo("|   |                                           * * *   |\n", 1);
    logo("|   |                     T O                     * *   |\n", 1);
    logo("|   |                                               *   |\n", 1);
    logo("|   |             A R O R - B A Z A R               |   |\n", 1);
    logo("|   |                                               |   |\n", 1);
    logo("|   |                                               |   |\n", 1);
    logo("|   |         ____________________________          |   |\n", 1);
    logo("|   *        |                            |         |   |\n", 1);
    logo("|   * *      | Matrix of digital delights |         |   |\n", 1);
    logo("|   * * *    |____________________________|         |   |\n", 1);
    logo("|   * * * *                                         |   |\n", 1);
    logo("|   * * * * *                                       |   |\n", 1);
    logo("|   * * * * * *_____________________________________|   |\n", 1);
    logo("|                                                       |\n", 1);
    logo("|_______________________________________________________|\n", 1);
    logo("                      _|       |_\n", 1);
    logo("                     / |_______| \\\n", 1);
    logo("                    /_____________\\\n", 1);
    logo("\nPress any Key to go to the Registration page---\n\n", 1);
    getch();
}

// --- FILE I/O (CSV) FUNCTIONS ---

void loadUsers() {
    ifstream file(USER_DATA_FILE);
    if (!file.is_open()) {
        numUsers = 0;
        return;
    }

    string line;
    numUsers = 0;
    while (numUsers < MAX_USERS && getline(file, line)) {
        stringstream ss(line);
        if (getline(ss, users[numUsers].name, ',') &&
            getline(ss, users[numUsers].pass, ',') &&
            getline(ss, users[numUsers].address)) {
            numUsers++;
        }
    }
    file.close();
}

void saveUsers() {
    ofstream file(USER_DATA_FILE);
    if (!file.is_open()) {
        cout << "Error: Could not open user file for saving!\n";
        return;
    }
    for (int i = 0; i < numUsers; ++i) {
        file << users[i].name << "," << users[i].pass << "," << users[i].address << endl;
    }
    file.close();
}

void loadProducts() {
    ifstream file(PRODUCT_DATA_FILE);
    if (!file.is_open()) {
        numProducts = 0;
        return;
    }

    string line;
    numProducts = 0;
    getline(file, line); // Skip header line 

    while (numProducts < MAX_PRODUCTS && getline(file, line)) {
        stringstream ss(line);
        string segment;

        if (getline(ss, segment, ',')) {
            products[numProducts].id = stoi(segment);
        } else continue;
        if (!getline(ss, products[numProducts].category, ',')) continue;
        if (!getline(ss, products[numProducts].name, ',')) continue;
        if (!getline(ss, products[numProducts].description, ',')) continue;
        if (getline(ss, segment)) {
            try {
                products[numProducts].price = stoi(segment);
            } catch (...) {
                products[numProducts].price = 0; // Default price on error
            }
        } else continue;
        
        numProducts++;
    }
    file.close();
}

void saveProducts() {
    ofstream file(PRODUCT_DATA_FILE);
    if (!file.is_open()) {
        cout << "Error: Could not open product file for saving!\n";
        return;
    }
    file << "ID,Category,Name,Description,Price\n";

    for (int i = 0; i < numProducts; ++i) {
        file << products[i].id << ","
             << products[i].category << ","
             << products[i].name << ","
             << products[i].description << ","
             << products[i].price << endl;
    }
    file.close();
}

void loadPurchases() {
    ifstream file(PURCHASE_DATA_FILE);
    if (!file.is_open()) {
        numPurchases = 0;
        return;
    }

    string line;
    numPurchases = 0;
    getline(file, line); // Skip header line 

    while (numPurchases < MAX_PURCHASES && getline(file, line)) {
        stringstream ss(line);
        string segment;
        
        // Format: userName,productName,price,paymentMethod,deliveryAddress
        if (getline(ss, purchases[numPurchases].userName, ',') &&
            getline(ss, purchases[numPurchases].productName, ',') &&
            getline(ss, segment, ',')) {
            try {
                purchases[numPurchases].price = stoi(segment);
            } catch (...) {
                purchases[numPurchases].price = 0;
            }
        } else continue;
        if (!getline(ss, purchases[numPurchases].paymentMethod, ',')) continue;
        if (!getline(ss, purchases[numPurchases].deliveryAddress)) continue;
        
        numPurchases++;
    }
    file.close();
}

void savePurchase(const Purchase& newPurchase) {
    // Open in append mode (ios::app)
    ofstream file(PURCHASE_DATA_FILE, ios::app); 
    if (!file.is_open()) {
        cout << "Error: Could not log purchase to file!\n";
        return;
    }
    
    // If the file is empty (first line will be at pos 0), write the header first
    if (file.tellp() == 0) { 
        file << "User,Product,Price,Method,Address\n";
    }

    // Write the new record
    file << newPurchase.userName << ","
         << newPurchase.productName << ","
         << newPurchase.price << ","
         << newPurchase.paymentMethod << ","
         << newPurchase.deliveryAddress << endl;
    
    file.close();
    
    // Update in-memory array
    if (numPurchases < MAX_PURCHASES) {
        purchases[numPurchases] = newPurchase;
        numPurchases++;
    }
}


// --- ADMIN FUNCTIONS ---

void loginAdmin() {
    system("CLS");
    string name, pass;

    cout << "\n=======================================================================\n";
    cout << "                         ADMIN LOGIN                                   \n";
    cout << "=======================================================================\n";
    
    cout << "Enter Admin Name: ";
    cin >> name;

    cout << "Enter Admin Password: ";
    cin >> pass;

    if (name == ADMIN_NAME && pass == ADMIN_PASS) {
        adminMenu();
    } else {
        cout << "\nInvalid Admin Name or Password. Please try again\n\n";
        cout << "Press any key to continue...";
        getch();
        displayMenu();
    }
}

void adminMenu() {
    system("CLS");
    cout << "\n=======================================================================\n";
    cout << "                   Welcome to the Admin Page                             \n";
    cout << "=======================================================================\n";
    cout << "\n1. | View All Users      |";
    cout << "\n2. | View All Products   |"; // New option
    cout << "\n3. | View Purchases      |"; // New option
    cout << "\n4. | Add New Product     |";
    cout << "\n5. | Remove Product      |";
    cout << "\n6. | Update Product      |";
    cout << "\n7. | Logout              |";
    cout << "\n0. | Exit                |";
    cout << "\n=======================================================================\n";
    cout << "\nPlease Enter your choice : ";
    int choice;
    if (!(cin >> choice)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number.\n"; getch(); adminMenu(); return;
    }

    switch (choice) {
        case 1: viewUsers(); break;
        case 2: viewProducts(); break; // New call
        case 3: viewPurchases(); break; // New call
        case 4: addProduct(); break;
        case 5: removeProduct(); break;
        case 6: updateProduct(); break;
        case 7: displayMenu(); break;
        case 0: cout << "\nExiting the program. Goodbye!\n"; exit(0);
        default:
            cout << "Invalid choice. Please try again.\n";
            getch();
            adminMenu();
    }
}

void viewUsers() {
    system("CLS");
    cout << "\n=======================================================================\n";
    cout << "                   Total Registered Accounts                             \n";
    cout << "=======================================================================\n";
    
    if (numUsers == 0) {
        cout << "No users registered yet.\n";
    } else {
        // Output in column format
        cout << left << setw(5) << "No." << setw(20) << "USERNAME" << setw(20) << "PASSWORD" << "ADDRESS" << endl;
        cout << string(65, '-') << endl;
        for (int i = 0; i < numUsers; ++i) {
            cout << left << setw(5) << i + 1 
                 << setw(20) << users[i].name 
                 << setw(20) << users[i].pass 
                 << users[i].address 
                 << endl;
        }
    }

    cout << "\nPress any key to continue...." << endl;
    getch();
    adminMenu();
}

void viewProducts() {
    system("CLS");
    cout << "\n=================================================================================================\n";
    cout << "                              All Available Products (Total: " << numProducts << ")                     \n";
    cout << "=================================================================================================\n";
    
    if (numProducts == 0) {
        cout << "No products currently available.\n";
    } else {
        // Use setw for clean column formatting
        cout << left << setw(5) << "ID" << setw(20) << "CATEGORY" << setw(30) << "NAME" << setw(10) << "PRICE" << "DESCRIPTION" << endl;
        cout << string(95, '-') << endl;
        
        for (int i = 0; i < numProducts; ++i) {
            cout << left << setw(5) << products[i].id 
                 << setw(20) << products[i].category 
                 << setw(30) << products[i].name 
                 << setw(10) << products[i].price
                 << products[i].description 
                 << endl;
        }
    }

    cout << "\nPress any key to continue...." << endl;
    getch();
    adminMenu();
}

void viewPurchases() {
    system("CLS");
    // Reload purchases to ensure we have the latest data before viewing
    loadPurchases(); 
    
    cout << "\n================================================================================================================\n";
    cout << "                                         All Customer Purchases (Total: " << numPurchases << ")                          \n";
    cout << "================================================================================================================\n";
    
    if (numPurchases == 0) {
        cout << "No purchases have been logged yet.\n";
    } else {
        // Use setw for clean column formatting
        cout << left << setw(5) << "No." << setw(15) << "USER" << setw(30) << "PRODUCT" << setw(10) << "PRICE" << setw(15) << "METHOD" << "ADDRESS" << endl;
        cout << string(105, '-') << endl;
        
        for (int i = 0; i < numPurchases; ++i) {
            cout << left << setw(5) << i + 1 
                 << setw(15) << purchases[i].userName 
                 << setw(30) << purchases[i].productName 
                 << setw(10) << purchases[i].price
                 << setw(15) << purchases[i].paymentMethod
                 << purchases[i].deliveryAddress 
                 << endl;
        }
    }

    cout << "\nPress any key to continue...." << endl;
    getch();
    adminMenu();
}


// --- PRODUCT MANAGEMENT (CRUD) FUNCTIONS (As before) ---
void addProduct() {
    system("CLS");
    cout << "\n=======================================================================\n";
    cout << "                     ADMIN: ADD NEW PRODUCT                              \n";
    cout << "=======================================================================\n";

    if (numProducts >= MAX_PRODUCTS) {
        cout << "\nMaximum product limit reached. Cannot add new product.\n";
        getch();
        adminMenu();
        return;
    }

    Product newProduct;
    newProduct.id = (numProducts > 0) ? products[numProducts - 1].id + 1 : 1; 

    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

    cout << "Enter Category (e.g., LAPTOPS, MOBILES): ";
    getline(cin, newProduct.category);
    cout << "Enter Product Name: ";
    getline(cin, newProduct.name);
    cout << "Enter Product Description (Use '-' instead of spaces for simplicity): ";
    getline(cin, newProduct.description);
    cout << "Enter Price (Rs.): ";
    
    string priceStr;
    getline(cin, priceStr);
    try {
        newProduct.price = stoi(priceStr);
    } catch (...) {
        cout << "\nInvalid price format. Product addition failed.\n";
        getch();
        adminMenu();
        return;
    }

    products[numProducts] = newProduct;
    numProducts++;
    saveProducts();

    cout << "\nProduct added successfully! ID: " << newProduct.id << endl;
    cout << "Press any key to return to Admin Menu...";
    getch();
    adminMenu();
}

void removeProduct() {
    system("CLS");
    cout << "\n=======================================================================\n";
    cout << "                     ADMIN: REMOVE PRODUCT                               \n";
    cout << "=======================================================================\n";

    if (numProducts == 0) {
        cout << "No products currently available.\n";
        getch();
        adminMenu();
        return;
    }

    int idToRemove;
    cout << "Enter the ID of the product to remove: ";
    if (!(cin >> idToRemove)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input.\n"; getch(); adminMenu(); return;
    }

    int indexToRemove = -1;
    for (int i = 0; i < numProducts; ++i) {
        if (products[i].id == idToRemove) {
            indexToRemove = i;
            break;
        }
    }

    if (indexToRemove != -1) {
        for (int i = indexToRemove; i < numProducts - 1; ++i) {
            products[i] = products[i + 1];
        }
        numProducts--;
        saveProducts();
        cout << "\nProduct with ID " << idToRemove << " removed successfully.\n";
    } else {
        cout << "\nProduct with ID " << idToRemove << " not found.\n";
    }

    cout << "Press any key to return to Admin Menu...";
    getch();
    adminMenu();
}

void updateProduct() {
    system("CLS");
    cout << "\n=======================================================================\n";
    cout << "                     ADMIN: UPDATE PRODUCT                               \n";
    cout << "=======================================================================\n";

    if (numProducts == 0) {
        cout << "No products currently available.\n";
        getch();
        adminMenu();
        return;
    }

    int idToUpdate;
    cout << "Enter the ID of the product to update: ";
    if (!(cin >> idToUpdate)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input.\n"; getch(); adminMenu(); return;
    }

    int indexToUpdate = -1;
    for (int i = 0; i < numProducts; ++i) {
        if (products[i].id == idToUpdate) {
            indexToUpdate = i;
            break;
        }
    }
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (indexToUpdate != -1) {
        cout << "\nFound Product: " << products[indexToUpdate].name << endl;
        cout << "Enter NEW Product Name (or press Enter to keep current): ";
        string tempName;
        getline(cin, tempName);
        if (!tempName.empty()) {
            products[indexToUpdate].name = tempName;
        }

        cout << "Enter NEW Description (or press Enter to keep current): ";
        string tempDesc;
        getline(cin, tempDesc);
        if (!tempDesc.empty()) {
            products[indexToUpdate].description = tempDesc;
        }

        cout << "Enter NEW Price (Rs.) (or enter 0 to keep current): ";
        string priceStr;
        getline(cin, priceStr);
        if (!priceStr.empty() && priceStr != "0") {
            try {
                products[indexToUpdate].price = stoi(priceStr);
            } catch (...) {
                cout << "\nInvalid price format. Price update skipped.\n";
            }
        }
        
        saveProducts();
        cout << "\nProduct with ID " << idToUpdate << " updated successfully.\n";
    } else {
        cout << "\nProduct with ID " << idToUpdate << " not found.\n";
    }

    cout << "Press any key to return to Admin Menu...";
    getch();
    adminMenu();
}

// --- USER MANAGEMENT FUNCTIONS (As before) ---
void registerUser() {
    system("CLS");
    cout << "\n=======================================================================\n";
    cout << "\nPlease enter your data to register your account\n\n";
    cout << "=======================================================================\n";

    if (numUsers >= MAX_USERS) {
        cout << "\nMaximum user limit reached. Cannot register new user.\n";
        getch();
        displayMenu();
        return;
    }

    string newuser, newpass, newaddress;
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

    cout << "\nEnter the username   : ";
    getline(cin, newuser);
    cout << "Enter the password : ";
    getline(cin, newpass);
    cout << "Enter your Address (Use '-' instead of spaces for simplicity): ";
    getline(cin, newaddress);

    users[numUsers].name = newuser;
    users[numUsers].pass = newpass;
    users[numUsers].address = newaddress;
    numUsers++;
    
    saveUsers(); 

    cout << "\nYour Account Created Successfully\n\n";
    cout << "\nPress any Key to go to the main menu---\n\n";
    getch();
    displayMenu();
}

void loginUser() {
    // ... (unchanged)
    int count = 0;
    string user, pass;
    system("cls");
    cout << "Please Enter the following Details.\n" << endl;
    cout << "USERNAME :";
    cin >> user;
    cout << endl;
    cout << "PASSWORD :";
    cin >> pass;

    loggedInUserIndex = -1;
    
    for (int i = 0; i < numUsers; ++i) {
        if (users[i].name == user && users[i].pass == pass) {
            count = 1;
            loggedInUserIndex = i;
            break;
        }
    }

    system("cls");
    if (count == 1) {
        cout << "\n\nHELLO Mr " << user << " LOGIN SUCCESSFULLY\n\nWe are glad that you are here.\nThanks for logging in\n\n";
        cout << "Press any key to go to the main menu.\n";
        getch();
        userMenu();
    } else {
        cout << "\nPlease check your username and password\n";
        cout << "Press any Key to Continue------\n\n";
        getch();
        displayMenu();
    }
}

void forget() {
    // ... (unchanged)
    int choice;
    system("cls");
    cout << "=======================================================================\n";
    cout << "\nForgotten? Don't worry, We are here for help\n\n";
    cout << "=======================================================================\n";

    cout << "\n1. Search your id by username\n";
    cout << "2. Search your id by password\n";
    cout << "3. Main menu\n";
    cout << "\nEnter your choice :";
    if (!(cin >> choice)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Returning to main menu.\n"; getch(); displayMenu(); return;
    }

    switch (choice) {
        case 1: {
            string search;
            cout << "\nEnter your remembered username :";
            cin >> search;

            int foundIndex = -1;
            for (int i = 0; i < numUsers; ++i) {
                if (users[i].name == search) {
                    foundIndex = i;
                    break;
                }
            }

            if (foundIndex != -1) {
                cout << "\nFound Successfully\n\n";
                cout << "\nMr " << users[foundIndex].name << " Your password is : " << users[foundIndex].pass;
                cout << "\n\nPlease remember your password \n\nPress any key to go to login---";
                getch();
                displayMenu();
            } else {
                cout << "\nSorry, Your userID is not found\n";
                cout << "Kindly contact your system administrator for more details \n";
                cout << "\nPress any Key to Continue ---\n\n";
                getch();
                displayMenu();
            }
            break;
        }
        case 2: {
            string search;
            cout << "\nEnter the remembered password :";
            cin >> search;

            int foundIndex = -1;
            for (int i = 0; i < numUsers; ++i) {
                if (users[i].pass == search) {
                    foundIndex = i;
                    break;
                }
            }

            if (foundIndex != -1) {
                cout << "\nFound Successfully\n\n";
                cout << "Your User name is : " << users[foundIndex].name;
                cout << "\n\nPlease remember your User name\n\nPress any key to go to login page ";
                getch();
                displayMenu();
                system("cls");
            } else {
                cout << "\n\nSorry, We cannot find your password in our database \n";
                cout << "Kindly contact your administrator for more information\n";
                cout << "\nPress any Key to Continue -------\n";
                getch();
                displayMenu();
            }
            break;
        }
        case 3: {
            system("cls");
            displayMenu();
            break;
        }
        default: {
            cout << "Invalid choice. Returning to Forget Menu.\n";
            forget();
        }
    }
}

// --- USER MENU FUNCTIONS (Unchanged except where noted) ---
void displayMenu() {
    // ... (unchanged structure)
    system("cls");
    cout << "\n=======================================================================\n";
    cout << "                 Welcome to Registration page                          \n";
    cout << "=======================================================================\n";
    cout << "\n1. | Admin Account |";
    cout << "\n2. | SignUp (User) |";
    cout << "\n3. | Login         |";
    cout << "\n4. | Forget I'D    |";
    cout << "\n0. | Exit          |";
    cout << "\n=======================================================================\n";
    int choice;
    cout << "\nEnter your choice : ";
    if (!(cin >> choice)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number.\n"; getch(); displayMenu(); return;
    }

    switch (choice) {
        case 1: loginAdmin(); break;
        case 2: registerUser(); break;
        case 3: loginUser(); break;
        case 4: forget(); break;
        case 0: cout << "\nExiting the program. Goodbye!\n"; exit(0); break;
        default: cout << "Invalid choice. Please try again.\n"; getch(); displayMenu();
    }
}

void userMenu() {
    // ... (unchanged structure)
    system("cls");
    cout << "\n=======================================================================\n";
    cout << "                  Welcome to the TECH-MARKET                             \n";
    cout << "=======================================================================\n";
    cout << "\n1. | Go to Shopping      |";
    cout << "\n2. | Logout              |";
    cout << "\n0. | Exit                |";
    cout << "\n=======================================================================\n";
    cout << "\nPlease enter your choice : ";
    int choice;
    if (!(cin >> choice)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number.\n"; getch(); userMenu(); return;
    }

    switch (choice) {
        case 1: shoppingMenu(); break;
        case 2: loggedInUserIndex = -1; displayMenu(); break;
        case 0: cout << "\nExiting the program. Goodbye!\n"; exit(0); return;
        default: cout << "Invalid choice. Please try again.\n"; getch(); userMenu();
    }
}

void shoppingMenu() {
    // ... (unchanged structure)
    system("cls");
    cout << "\n=======================================================================\n";
    cout << "                      Welcome to the Shopping Area                       \n";
    cout << "=======================================================================\n";
    cout << "\n1. | LAPTOPS             |";
    cout << "\n2. | MOBILES             |";
    cout << "\n3. | OTHER ACCESSORIES   |";
    cout << "\n0. | BACK                |";
    cout << "\n=======================================================================\n";

    int choice;
    cout << "\nPlease Enter Your Choice : ";
    if (!(cin >> choice)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number.\n"; getch(); shoppingMenu(); return;
    }

    string category;
    switch (choice) {
        case 1: category = "LAPTOPS"; break;
        case 2: category = "MOBILES"; break;
        case 3: category = "ACCESSORIES"; break;
        case 0: userMenu(); return;
        default: cout << "Invalid choice. Please try again.\n"; getch(); shoppingMenu(); return;
    }

    displayProducts(category);
}

void displayProducts(const string& category) {
    // ... (unchanged structure)
    system("CLS");
    cout << "\n=======================================================================\n";
    cout << "                      " << category << " CATALOGUE                     \n";
    cout << "=======================================================================\n";
    
    int productCount = 0;
    std::array<int, MAX_PRODUCTS> displayedIndices; 
    int displayedCount = 0;

    for (int i = 0; i < numProducts; ++i) {
        if (products[i].category == category) {
            cout << "\n" << ++productCount << ". ID: " << products[i].id << " - " << products[i].name << "\n";
            cout << "   Description: " << products[i].description << "\n";
            cout << "   Price: Rs." << products[i].price << "\n";
            
            if (displayedCount < MAX_PRODUCTS) {
                 displayedIndices[displayedCount++] = i;
            }
        }
    }
    
    if (productCount == 0) {
        cout << "\nNo products found in this category.\n";
        cout << "Press any key to go back to the shopping menu...";
        getch();
        shoppingMenu();
        return;
    }

    int selection;
    cout << "\n=======================================================================\n";
    cout << "Enter the number (1-" << productCount << ") to buy, or 0 to go back: ";
    
    if (!(cin >> selection)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\nInvalid input. Going back to menu.\n"; getch(); shoppingMenu(); return;
    }

    if (selection == 0) {
        shoppingMenu();
        return;
    } else if (selection >= 1 && selection <= productCount) {
        const Product& selectedProduct = products[displayedIndices[selection - 1]];
        processOrder(selectedProduct);
    } else {
        cout << "\nPlease enter a valid choice.\n";
        getch();
        displayProducts(category);
    }
}

void processOrder(const Product& selectedProduct) {
    system("CLS");
    cout << "\n=======================================================================\n";
    cout << "                      CHECKOUT - " << selectedProduct.name << "\n";
    cout << "=======================================================================\n";
    
    cout << "\nSelected Product: " << selectedProduct.name;
    cout << "\nPrice: Rs." << selectedProduct.price;

    cout << "\n\nPlease enter transaction method:\n";
    cout << "1. Bank Account (Easypaisa/JazzCash)\n2. Cash on Delivery\n";
    
    int choice;
    cout << "\nEnter your choice: ";
    if (!(cin >> choice)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\nInvalid input. Returning to shopping menu.\n"; getch(); shoppingMenu(); return;
    }

    const int DELIVERY_CHARGE = 200;
    int totalAmount = selectedProduct.price + DELIVERY_CHARGE;
    string deliveryMethod;
    
    string userName = "Guest";
    string userAddress = "Unknown Location";

    if (loggedInUserIndex != -1) {
        userName = users[loggedInUserIndex].name;
        userAddress = users[loggedInUserIndex].address;
    }

    switch (choice) {
        case 1: {
            deliveryMethod = "Bank Account";
            long long accountNumber;
            cout << "Please enter your account number: ";
            cin >> accountNumber;
            cout << "Wait for your OTP code..." << endl;
            cout << "\nAuto Fill...";
            this_thread::sleep_for(chrono::seconds(3));
            srand(time(0));
            cout << rand() % 10000 << endl;
            cout << "Press any key to confirm account...";
            getch();
            break;
        }
        case 2: {
            deliveryMethod = "Cash on Delivery";
            break;
        }
        default:
            cout << "\nInvalid choice! Please try again.\n";
            getch();
            processOrder(selectedProduct);
            return;
    }

    cout << "\n\n--- Order Summary ---" << endl;
    cout << left << setw(20) << "Product Price" << ": Rs." << selectedProduct.price << endl;
    cout << left << setw(20) << "Delivery Charges" << ": Rs." << DELIVERY_CHARGE << endl;
    cout << left << setw(20) << "Total Payable" << ": Rs." << totalAmount << endl;
    cout << "Press any key to confirm your order....";
    getch();

    // Log the successful purchase before displaying the final message
    Purchase newPurchase = {userName, selectedProduct.name, totalAmount, deliveryMethod, userAddress};
    savePurchase(newPurchase);

    // CUSTOMIZED SUCCESS MESSAGE
    cout << "\n\n---------------------- ORDER CONFIRMED ----------------------\n\n";
    
    string displayAddress = userAddress;
    size_t pos = displayAddress.find('-');
    while(pos != string::npos) {
        displayAddress.replace(pos, 1, " ");
        pos = displayAddress.find('-', pos + 1);
    }

    cout << "Thank you for shopping " << userName << "! Your product:\n";
    cout << ">> " << selectedProduct.name << "\n";
    cout << "will soon be delivered to " << displayAddress << ".\n";
    cout << "Payment method: " << deliveryMethod << endl;
    
    cout << "\n\nPress any key to go back to the menu...";
    getch();
    shoppingMenu();
}

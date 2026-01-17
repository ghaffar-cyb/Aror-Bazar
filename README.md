# Aror Bazar - Console E-Commerce System

**Aror Bazar** is a console-based C++ application that simulates a digital marketplace. It features a complete login system, product management for administrators, and a shopping experience for users with data persistence using CSV files.

## 🚀 Features

### 👤 User Panel
* **Registration & Login:** Secure account creation and authentication.
* **Password Recovery:** "Forget ID" functionality to retrieve credentials.
* **Shopping:** Browse products by category (Laptops, Mobiles, Accessories).
* **Checkout:** Simulates payment processing (Bank/Cash) and tracks delivery details.

### 🛠 Admin Panel
* **Dashboard:** View all registered users and inventory status.
* **Product Management:** Add, Update, and Remove products dynamically.
* **Sales Tracking:** View a log of all customer purchases.

### 💾 Data Persistence
The system uses file handling to store data permanently:
* `users.csv`: Stores user credentials.
* `products.csv`: Stores inventory details.
* `purchases.csv`: Stores order history.

## 🔧 Technologies Used
* **Language:** C++
* **Concepts:** File Handling (`fstream`), Structures, Arrays, Threading (for animations).
* **Libraries:** `iostream`, `conio.h` (Windows-specific UI), `thread`, `iomanip`.

## ⚙️ How to Run

1.  **Compile:** Use a C++ compiler (like g++ or via Dev-C++/Visual Studio).
    ```bash
    g++ "Aror Bazar.cpp" -o aror_bazar
    ```
2.  **Run:** Execute the generated `.exe` file.
3.  **Files:** Ensure the program has permission to create/read `.csv` files in the same directory.

## 🔑 Default Admin Credentials
To access the Admin Panel, use the credentials hardcoded in the source:
* **Username:** `Ghaffar`
* **Password:** `1122`

## 📝 Note
This project utilizes `<conio.h>` and `system("CLS")`, making it optimized for **Windows** environments.

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include <limits> // For numeric_limits
#include <unordered_map> // For storing credentials
using namespace std;

// Base Class: Person
class Person {
protected:
    string name;
    int id;

public:
    Person() : name(""), id(0) {};
    Person(const string& n, int i) : name(n), id(i) {}
    virtual ~Person() = default;

    virtual void displayDetails() {
        cout << "Person Type in this Restaurant :" << endl;
    }

    int getID() const {
        return id;
    }

    string getName() const {
        return name;
    }
};

// Derived Class: Customer
class Customer : public Person {
public:
	Customer() : Person("",0 ) {}
    Customer(const string& n, int i) : Person(n, i) {}

    void displayDetails() override {
        cout << "Customer Name: " << name << ", ID: " << id << endl << endl;
    }
};

// Derived Class: Employee
class Employee : public Person {
protected:
    string position;

public:
    Employee(const string& n, int i, const string& pos) : Person(n, i), position(pos) {}

    void displayDetails() override {
        cout << "Employee Name: " << name << ", ID: " << id << ", Position: " << position << endl << endl;
    }
};

// Derived Class: Admin
class Admin : public Employee {
public:
    Admin(const string& n, int i) : Employee(n, i, "Admin") {}

    void displayDetails() override {
        cout << "Admin Name: " << name << ", ID: " << id << endl << endl;
    }
};

// Item Class
class Item {
private:
    int itemID;
    string itemName;
    double itemPrice;

public:
    Item(int id, const string& name, double price) : itemID(id), itemName(name), itemPrice(price) {}

    void displayDetails() const {
        cout << itemID << ". " << itemName << " - Rs. " << itemPrice << endl;
    }

    double getPrice() const {
        return itemPrice;
    }

    int getID() const {
        return itemID;
    }

    string getName() const {
        return itemName;
    }
};

// Base Class: Payment
class Payment {
protected:
    double amount;

public:
    Payment(double amt) : amount(amt) {}
    virtual ~Payment() = default;
    virtual void processPayment() const = 0; // Pure virtual function
};

// Derived Classes: Cash and Card
class Cash : public Payment {
public:
    Cash(double amt) : Payment(amt) {}

    void processPayment() const override {
        cout << "Payment of Rs. " << amount << " made in cash.\n";
    }
};

class Card : public Payment {
private:
    string cardNumber;

public:
    Card(double amt, const string& cardNum) : Payment(amt), cardNumber(cardNum) {}

    void processPayment() const override {
        cout << "Payment of Rs. " << amount << " made using Card Number: " << cardNumber << ".\n";
    }
};

// Order Class (Composite with Payment and Item)
class Order {
private:
    int tableNumber;
    vector<unique_ptr<Item>> items;
    unique_ptr<Payment> payment;
    double totalAmount;

public:
    Order(int tableNumber) : tableNumber(tableNumber), totalAmount(0) {}

    void addItem(unique_ptr<Item> item) {
        totalAmount += item->getPrice();
        items.push_back(move(item));
    }

    void addPayment(unique_ptr<Payment> pay) {
        payment = move(pay);
    }

    double getTotalAmount() const {
        return totalAmount;
    }

    void displayOrder() const {
        cout << "\n--- Table " << tableNumber << " Order ---\n";
        cout << "Items:\n";
        for (const auto& item : items) {
            item->displayDetails();
        }
        cout << "Total Amount: Rs. " << totalAmount << endl;
        if (payment) {
            payment->processPayment();
        } else {
            cout << "Payment not processed yet.\n";
        }
    }
};

// Menu Class (Composite with Order)
class Menu {
private:
    vector<unique_ptr<Item>> items;
    vector<unique_ptr<Order>> orders;
    unordered_set<int> bookedTables;
    const int totalTables = 5; // Total number of tables

public:
    void addMenuItem(int id, const string& name, double price) {
        items.push_back(make_unique<Item>(id, name, price));
    }

    void displayMenu() const {
        cout << "\n--- Menu ---\n";
        for (const auto& item : items) {
            item->displayDetails();
        }
    }

    Item* findItem(int id) {
        for (const auto& item : items) {
            if (item->getID() == id) {
                return item.get();
            }
        }
        return nullptr;
    }

    bool isItemNameExists(const string& name) {
        for (const auto& item : items) {
            if (item->getName() == name) {
                return true;
            }
        }
        return false;
    }

    bool isTableBooked(int tableNumber) const {
        return bookedTables.find(tableNumber) != bookedTables.end();
    }

    void bookTable(int tableNumber) {
        bookedTables.insert(tableNumber);
    }

    bool areAllTablesBooked() const {
        return bookedTables.size() >= totalTables;
    }

    void createOrder(int tableNumber) {
        auto newOrder = make_unique<Order>(tableNumber);
        int itemID;
        while (true) {
            displayMenu();
            cout << "Enter Item ID to add to order (0 to finish): ";
            while (!(cin >> itemID)) {
                cout << "Invalid input. Please enter a valid Item ID: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            if (itemID == 0) break;
            Item* item = findItem(itemID);
            if (item) {
                newOrder->addItem(make_unique<Item>(item->getID(), item->getName(), item->getPrice()));
            } else {
                cout << "Invalid Item ID.\n";
            }
        }
        int paymentChoice;
        cout << "Choose Payment Method (1 for Cash, 2 for Card): ";
        while (!(cin >> paymentChoice) || (paymentChoice != 1 && paymentChoice != 2)) {
            cout << "Invalid input. Please choose 1 for Cash or 2 for Card: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        if (paymentChoice == 1) {
            newOrder->addPayment(make_unique<Cash>(newOrder->getTotalAmount()));
        } else {
            string cardNumber;
            cout << "Enter Card Number: ";
            cin.ignore();
            getline(cin, cardNumber);
            newOrder->addPayment(make_unique<Card>(newOrder->getTotalAmount(), cardNumber));
        }
        orders.push_back(move(newOrder));
        cout << "Order placed successfully.\n";
    }

    void displayOrders() const {
        cout << "\n--- All Orders ---\n";
        for (const auto& order : orders) {
            order->displayOrder();
        }
    }
};

// RMS Class (Composition with Menu)
class RMS {
private:
    int rmsID;
    string name, location;
    Menu menu;
    vector<shared_ptr<Person>> persons; // Aggregation of Person objects

    unordered_map<string, string> credentials = {
        {"admin", "admin123"},
        {"manager", "manager123"}
    };

    bool authenticate(const string& role) {
        string username, password;
        cout << "Enter " << role << " Username: ";
        cin >> username;
        cout << "Enter Password: ";
        cin >> password;

        if (credentials.find(username) != credentials.end() && credentials[username] == password) {
            return true;
        }
        cout << "Invalid credentials. Access denied.\n";
        return false;
    }

public:
    RMS(int id, const string& n, const string& loc) : rmsID(id), name(n), location(loc) {}

    void display() {
        cout << "RMS ID: " << rmsID << endl;
        cout << "Restaurant Name: " << name << endl;
        cout << "Location: " << location << endl;
    }

    void addPerson(shared_ptr<Person> person) {
        persons.push_back(person);
    }

    void initializeMenu() {
        menu.addMenuItem(1, "Burger", 250);
        menu.addMenuItem(2, "Pizza", 500);
        menu.addMenuItem(3, "Pasta", 300);
    }

    void adminPanel() {
        if (!authenticate("Admin")) return;
        int choice;
        do {
            cout << "\n--- Admin Panel ---\n";
            cout << "1. Add Menu Item\n";
            cout << "2. View Menu\n";
            cout << "3. Exit\n";
            cout << "Enter your choice: ";
            while (!(cin >> choice) || (choice < 1 || choice > 3)) {
                cout << "Invalid input. Please choose a valid option: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            switch (choice) {
                case 1: {
                    int id;
                    string name;
                    double price;
                    while (true) {
                        cout << "Enter Item ID: ";
                        while (!(cin >> id)) {
                            cout << "Invalid input. Please enter a valid Item ID: ";
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        }
                        if (menu.findItem(id)) {
                            cout << "Item ID already exists. Please enter a different ID.\n";
                            continue;
                        }
                        cin.ignore();
                        cout << "Enter Item Name: ";
                        getline(cin, name);
                        if (menu.isItemNameExists(name)) {
                            cout << "Item Name already exists. Please enter a different name.\n";
                            continue;
                        }
                        cout << "Enter Item Price: ";
                        while (!(cin >> price) || price <= 0) {
                            cout << "Invalid input. Please enter a valid price: ";
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        }
                        menu.addMenuItem(id, name, price);
                        cout << "Item added successfully.\n";
                        break;
                    }
                    break;
                }
                case 2:
                    menu.displayMenu();
                    break;
                case 3:
                    cout << "Exiting Admin Panel...\n";
                    break;
            }
        } while (choice != 3);
    }

    void managerPanel() {
        if (!authenticate("Manager")) return;
        cout << "\n--- Manager Panel ---\n";
        menu.displayOrders();
    }

    void customerPanel() {
        if (menu.areAllTablesBooked()) {
            cout << "All tables are booked. Please wait.\n";
            return;
        }

        string customerName;
        int customerID;

        cout << "Enter your name: ";
        cin.ignore();
        getline(cin, customerName);

        cout << "Enter your ID: ";
        while (!(cin >> customerID) || customerID <= 0) {
            cout << "Invalid input. Please enter a valid ID: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        // Create Customer Object and Display Details
        Customer customer(customerName, customerID);
        customer.displayDetails();

        int tableNumber;
        while (true) {
            cout << "Enter Table Number (1-5): ";
            while (!(cin >> tableNumber) || tableNumber < 1 || tableNumber > 5) {
                cout << "Invalid input. Please enter a valid table number (1-5): ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            if (menu.isTableBooked(tableNumber)) {
                cout << "Table " << tableNumber << " is already booked. Please choose another table.\n";
            } else {
                menu.bookTable(tableNumber);
                break;
            }
        }

        cout << "Table " << tableNumber << " booked successfully for " << customerName << " (ID: " << customerID << ")!\n";
        menu.createOrder(tableNumber);
    }

    void rolePanel() {
        int choice;
        do {
            cout << "\n--- Select Role ---\n";
            cout << "1. Admin\n";
            cout << "2. Manager\n";
            cout << "3. Customer\n";
            cout << "4. Exit\n";
            cout << "Enter your choice: ";
            while (!(cin >> choice) || (choice < 1 || choice > 4)) {
                cout << "Invalid input. Please choose a valid option: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            switch (choice) {
                case 1:
                    adminPanel();
                    break;
                case 2:
                    managerPanel();
                    break;
                case 3:
                    customerPanel();
                    break;
                case 4:
                    cout << "Exiting...\n";
                    break;
            }
        } while (choice != 4);
    }
};

// Main Function
int main() {
    // Create Person and Derived Class Objects
    Person person;
    Admin admin("John Doe", 101);
    Customer customer;
    Employee employee("Jane Smith", 102, "Manager");
    RMS rms(1, "Cheezious", "Main Street" );
    person.displayDetails();
    admin.displayDetails();
    customer.displayDetails();
    employee.displayDetails();

    rms.display();
    rms.initializeMenu();
    rms.rolePanel();

    return 0;
}

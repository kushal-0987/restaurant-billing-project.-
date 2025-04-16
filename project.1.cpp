// creating ITP Restaurant Billing System
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
using namespace std;

// this is Base class for products
class Product {
protected:
    int id;
    string name;
    double price;
    string type;
public:
    Product(int i, const string& n, double p, const string& t) : id(i), name(n), price(p), type(t) {}
    virtual ~Product() {}
    virtual double calculatePrice() const { return price; }
    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    string getType() const { return type; }
};

// Derived class for food items
class Food : public Product {
private:
    double taxRate;
public:
    Food(int i, const string& n, double p) : Product(i, n, p, "Food"), taxRate(0.0) {}
    double calculatePrice() const override { return price; }
};

// Derived class for beverages which is drinks item
class Beverage : public Product {
public:
    Beverage(int i, const string& n, double p) : Product(i, n, p, "Beverage") {}
    double calculatePrice() const override { return price; }
};

// Customer class
class Customer {
private:
    int id;
    string name;
public:
    Customer(int i, const string& n) : id(i), name(n) {}
    int getId() const { return id; }
    string getName() const { return name; }
};

// Invoice class
class Invoice {
private:
    int invoiceId;
    Customer customer;
    vector<pair<Product*, int>> items;
    string date;
public:
    Invoice(int id, const Customer& c, const string& d) : invoiceId(id), customer(c), date(d) {}
    void addItem(Product* p, int qty) {
        for (auto& item : items) {
            if (item.first->getId() == p->getId()) {
                item.second = qty;
                return;
            }
        }
        items.push_back({p, qty});
    }
    double calculateTotal() const {
        double subTotal = 0.0;
        for (const auto& item : items) {
            subTotal += item.first->calculatePrice() * item.second;
        }
        double discount = 0.0;
        for (const auto& item : items) {
            string name = item.first->getName();
            if (name == "Chicken Biryani" || name == "Chinese Rice" || name == "Chicken Palao") {
                discount += 0.1 * (item.first->calculatePrice() * item.second);
            } else {
                discount += 0.05 * (item.first->calculatePrice() * item.second);
            }
        }
        return subTotal - discount;
    }
    void display() const {
        cout << "\n\n\t    ITP Restaurant\n";
        cout << "\t   -----------------\n";
        cout << "Date: " << date << endl;
        cout << "Invoice To: " << customer.getName() << endl;
        cout << "---------------------------------------\n";
        cout << left << setw(20) << "Items" << setw(10) << "Qty" << setw(10) << "Total" << endl;
        cout << "---------------------------------------\n\n";
        double subTotal = 0.0;
        for (const auto& item : items) {
            double total = item.first->calculatePrice() * item.second;
            cout << left << setw(20) << item.first->getName() << setw(10) << item.second
                 << setw(10) << fixed << setprecision(2) << total << endl;
            subTotal += total;
        }
        double discount = 0.0;
        for (const auto& item : items) {
            string name = item.first->getName();
            if (name == "Chicken Biryani" || name == "Chinese Rice" || name == "Chicken Palao") {
                discount += 0.1 * (item.first->calculatePrice() * item.second);
            } else {
                discount += 0.05 * (item.first->calculatePrice() * item.second);
            }
        }
        double netTotal = subTotal - discount;
        cout << "\n---------------------------------------\n";
        cout << setw(20) << "Sub Total" << setw(20) << "" << "₹" << subTotal << endl;
        cout << setw(20) << "Discount" << setw(20) << "" << "₹" << discount << endl;
        cout << setw(20) << "" << setw(20) << "" << "-------" << endl;
        cout << setw(20) << "Grand Total" << setw(20) << "" << "₹" << netTotal << endl;
        cout << "---------------------------------------\n";
    }
    int getInvoiceId() const { return invoiceId; }
    string getCustomerName() const { return customer.getName(); }
    string getDate() const { return date; }
    const vector<pair<Product*, int>>& getItems() const { return items; }
};

// FileHandler class 
class FileHandler {
public:
    static void saveProducts(const vector<Product*>& products) {
        ofstream file("products.txt");
        if (!file) {
            cout << "Error opening products file!\n";
            return;
        }
        for (const auto& p : products) {
            file << p->getType() << "," << p->getId() << "," << p->getName() << ","
                 << p->getPrice() << endl;
        }
        file.close();
    }
    static void loadProducts(vector<Product*>& products) {
        ifstream file("products.txt");
        if (!file) return;
        string type, name;
        int id;
        double price;
        while (getline(file, type, ',')) {
            file >> id;
            file.ignore();
            getline(file, name, ',');
            file >> price;
            file.ignore();
            if (type == "Food") {
                products.push_back(new Food(id, name, price));
            } else if (type == "Beverage") {
                products.push_back(new Beverage(id, name, price));
            }
        }
        file.close();
    }
    static void saveCustomers(const vector<Customer>& customers) {
        ofstream file("customers.txt");
        if (!file) {
            cout << "Error opening customers file!\n";
            return;
        }
        for (const auto& c : customers) {
            file << c.getId() << "," << c.getName() << endl;
        }
        file.close();
    }
    static void loadCustomers(vector<Customer>& customers) {
        ifstream file("customers.txt");
        if (!file) return;
        int id;
        string name;
        while (file >> id) {
            file.ignore();
            getline(file, name);
            customers.emplace_back(id, name);
        }
        file.close();
    }
    static void saveInvoices(const vector<Invoice>& invoices) {
        ofstream file("invoices.txt");
        if (!file) {
            cout << "Error opening invoices file!\n";
            return;
        }
        for (const auto& inv : invoices) {
            file << inv.getInvoiceId() << "," << inv.getCustomerName() << ","
                 << inv.getDate() << endl;
            for (const auto& item : inv.getItems()) {
                file << item.first->getId() << "," << item.second << endl;
            }
            file << "END\n";
        }
        file.close();
    }
    static void loadInvoices(vector<Invoice>& invoices, const vector<Customer>& customers,
                            const vector<Product*>& products) {
        ifstream file("invoices.txt");
        if (!file) return;
        int invId;
        string custName, date;
        while (file >> invId) {
            file.ignore();
            getline(file, custName, ',');
            getline(file, date);
            Customer cust(0, custName);
            for (const auto& c : customers) {
                if (c.getName() == custName) {
                    cust = c;
                    break;
                }
            }
            Invoice inv(invId, cust, date);
            string line;
            while (getline(file, line) && line != "END") {
                int prodId, qty;
                sscanf(line.c_str(), "%d,%d", &prodId, &qty);
                for (const auto& p : products) {
                    if (p->getId() == prodId) {
                        inv.addItem(p, qty);
                        break;
                    }
                }
            }
            invoices.push_back(inv);
        }
        file.close();
    }
};

// Restaurant class managing the billing system
class Restaurant {
private:
    vector<Product*> products;
    vector<Customer> customers;
    vector<Invoice> invoices;
    int nextProductId;
    int nextCustomerId;
    int nextInvoiceId;
    string getCurrentDate() {
        time_t now = time(0);
        char buf[50];
        strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
        return string(buf);
    }
public:
    Restaurant() : nextProductId(1), nextCustomerId(1), nextInvoiceId(1) {
        products.push_back(new Food(nextProductId++, "Chicken Biryani", 260.0));
        products.push_back(new Food(nextProductId++, "Chicken Palao", 195.0));
        products.push_back(new Food(nextProductId++, "Chinese Rice", 325.0));
        products.push_back(new Food(nextProductId++, "Chicken Burgar", 130.0));
        products.push_back(new Food(nextProductId++, "Nawabi Pizza", 585.0));
        products.push_back(new Beverage(nextProductId++, "2.5 Litre Coke", 227.5));
        FileHandler::loadProducts(products);
        FileHandler::loadCustomers(customers);
        FileHandler::loadInvoices(invoices, customers, products);
        for (const auto& p : products) {
            if (nextProductId <= p->getId()) {
                nextProductId = p->getId() + 1;
            }
        }
        for (const auto& c : customers) {
            if (nextCustomerId <= c.getId()) {
                nextCustomerId = c.getId() + 1;
            }
        }
        for (const auto& i : invoices) {
            if (nextInvoiceId <= i.getInvoiceId()) {
                nextInvoiceId = i.getInvoiceId() + 1;
            }
        }
    }
    ~Restaurant() {
        for (auto p : products) {
            delete p;
        }
    }
    void addProduct() {
        int type;
        string name;
        double price;
        cout << "Enter product type (1: Food, 2: Beverage): ";
        cin >> type;
        cin.ignore();
        cout << "Enter product name: ";
        getline(cin, name);
        cout << "Enter price: ";
        cin >> price;
        if (type == 1) {
            products.push_back(new Food(nextProductId++, name, price));
        } else if (type == 2) {
            products.push_back(new Beverage(nextProductId++, name, price));
        } else {
            cout << "Invalid type!\n";
            return;
        }
        cout << "Product added!\n";
    }
    void addCustomer() {
        string name;
        cout << "Enter customer name: ";
        cin.ignore();
        getline(cin, name);
        customers.emplace_back(nextCustomerId++, name);
        cout << "Customer added!\n";
    }
    void createInvoice() {
        string custName;
        cout << "Enter customer name: ";
        cin.ignore();
        getline(cin, custName);
        Customer* cust = nullptr;
        for (auto& c : customers) {
            if (c.getName() == custName) {
                cust = &c;
                break;
            }
        }
        if (!cust) {
            cout << "Customer not found! Please add customer first.\n";
            return;
        }
        Invoice inv(nextInvoiceId++, *cust, getCurrentDate());
        cout << "Enter number of items: ";
        int n;
        cin >> n;
        for (int i = 0; i < n; i++) {
            cout << "\nItem " << i + 1 << ":\n";
            cout << "Enter product ID (0 to list products): ";
            int prodId;
            cin >> prodId;
            if (prodId == 0) {
                displayProducts();
                cout << "Enter product ID: ";
                cin >> prodId;
            }
            Product* prod = nullptr;
            for (const auto& p : products) {
                if (p->getId() == prodId) {
                    prod = p;
                    break;
                }
            }
            if (!prod) {
                cout << "Product not found!\n";
                i--;
                continue;
            }
            cout << "Enter quantity: ";
            int qty;
            cin >> qty;
            if (qty <= 0) {
                cout << "Invalid quantity!\n";
                i--;
                continue;
            }
            inv.addItem(prod, qty);
        }
        inv.display();
        cout << "Save invoice? [y/n]: ";
        char save;
        cin >> save;
        if (save == 'y') {
            invoices.push_back(inv);
            cout << "Invoice saved!\n";
        }
    }
    void displayProducts() const {
        cout << "\nProducts:\n";
        for (const auto& p : products) {
            cout << "ID: " << p->getId() << ", " << p->getName() << ", ₹"
                 << fixed << setprecision(2) << p->getPrice() << " (" << p->getType() << ")\n";
        }
    }
    void displayCustomers() const {
        cout << "\nCustomers:\n";
        for (const auto& c : customers) {
            cout << "ID: " << c.getId() << ", Name: " << c.getName() << "\n";
        }
    }
    void showAllInvoices() const {
        cout << "\nAll Invoices:\n";
        for (const auto& i : invoices) {
            i.display();
        }
        if (invoices.empty()) {
            cout << "No invoices found.\n";
        }
    }
    void searchInvoice() {
        string name;
        cout << "Enter customer name: ";
        cin.ignore();
        getline(cin, name);
        bool found = false;
        for (const auto& i : invoices) {
            if (i.getCustomerName() == name) {
                i.display();
                found = true;
            }
        }
        if (!found) {
            cout << "No invoices found for " << name << ".\n";
        }
    }
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
    void saveData() {
        FileHandler::saveProducts(products);
        FileHandler::saveCustomers(customers);
        FileHandler::saveInvoices(invoices);
    }
    void run() {
        while (true) {
            clearScreen();
            cout << "\t============ITP RESTAURANT============\n\n";
            cout << "1. Add Product\n";
            cout << "2. Add Customer\n";
            cout << "3. Generate Invoice\n";
            cout << "4. Show Products\n";
            cout << "5. Show Customers\n";
            cout << "6. Show All Invoices\n";
            cout << "7. Search Invoice\n";
            cout << "8. Exit\n";
            cout << "Choice: ";
            int opt;
            cin >> opt;
            switch (opt) {
                case 1: addProduct(); break;
                case 2: addCustomer(); break;
                case 3: createInvoice(); break;
                case 4: displayProducts(); break;
                case 5: displayCustomers(); break;
                case 6: showAllInvoices(); break;
                case 7: searchInvoice(); break;
                case 8:
                    saveData();
                    cout << "\n\t\tThank You for Visiting ITP Restaurant :)\n\n";
                    return;
                default:
                    cout << "Invalid option!\n";
            }
            cout << "Press Enter to continue...";
            cin.ignore();
            cin.get();
        }
    }
};

// Main function
int main() {
    Restaurant restaurant;
    restaurant.run();
    return 0;
    // ending my invoice system.
}









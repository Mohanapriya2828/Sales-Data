
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <filesystem>
#include <limits>
#include <random>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

struct Sale {
    string date;
    int id;
    string itemName;
    int quantity;
    double price;
};

bool isFileEmpty(const string& filename) {
    ifstream file(filename);
    return file.peek() == ifstream::traits_type::eof();
}

int generateRandomId() {
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<> dis(1000, 9999);
    return dis(gen);
}

bool isValidDate(const string& date) {
    if (date.size() != 10) return false;
    if (date[2] != '/' || date[5] != '/') return false;

    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!isdigit(date[i])) return false;
    }

    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));

    if (year < 1000 || year > 9999) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;

    return true;
}

void appendSaleToCSV(const string& filename, const Sale& s) {
    bool writeHeader = !filesystem::exists(filename) || isFileEmpty(filename);

    ofstream file(filename, ios::app);
    if (!file) {
        cerr << "Error: Cannot open " << filename << " for writing.\n";
        return;
    }

    if (writeHeader) {
        file << "date,saleID,item name,item quantity,price\n";
    }

    file << s.date << ","
         << s.id << ","
         << s.itemName << ","
         << s.quantity << ","
         << fixed << setprecision(2) << s.price << "\n";

    file.close();
}

vector<Sale> loadSales(const string& filename) {
    vector<Sale> sales;
    ifstream file(filename);
    if (!file) return sales;

    string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        Sale s;
        string temp;

        getline(ss, s.date, ',');
        getline(ss, temp, ',');
        s.id = stoi(temp);
        getline(ss, s.itemName, ',');
        getline(ss, temp, ',');
        s.quantity = stoi(temp);
        getline(ss, temp, ',');
        s.price = stod(temp);

        sales.push_back(s);
    }

    return sales;
}

void saveSales(const string& filename, const vector<Sale>& sales) {
    ofstream file(filename);
    if (!file) {
        cerr << "Error: Cannot open " << filename << " for writing.\n";
        return;
    }

    file << "date,saleID,item name,item quantity,price\n";

    for (const auto& s : sales) {
        file << s.date << ","
             << s.id << ","
             << s.itemName << ","
             << s.quantity << ","
             << fixed << setprecision(2) << s.price << "\n";
    }

    file.close();
}

void deleteSaleById(vector<Sale>& sales, int targetId) {
    auto it = remove_if(sales.begin(), sales.end(), [targetId](const Sale& s) {
        return s.id == targetId;
    });

    if (it != sales.end()) {
        sales.erase(it, sales.end());
    }
}

bool compareDates(const Sale& a, const Sale& b) {
    // Date format is DD/MM/YYYY. We will compare the string lexicographically.
    return a.date < b.date;
}

void createSortedTempFile(const string& filename) {
    vector<Sale> sales = loadSales(filename);
    if (sales.empty()) {
        cout << "No records found to sort.\n";
        return;
    }

    // Sort sales by date
    sort(sales.begin(), sales.end(), compareDates);

    // Save sorted sales to temp.csv
    const string tempFilename = "temp.csv";
    saveSales(tempFilename, sales);

    cout << "Sales records have been sorted and saved to temp.csv.\n";
}

int main() {
    const string filename = "sales.csv";
    char choice;
    do {
        Sale s;
        s.id = generateRandomId();

        // Validate date
        do {
            cout << "Enter date (DD/MM/YYYY): ";
            cin >> s.date;
            if (!isValidDate(s.date))
                cout << "Invalid date, try again.\n";
        } while (!isValidDate(s.date));

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter item name: ";
        getline(cin, s.itemName);

        cout << "Enter item quantity: ";
        cin >> s.quantity;

        cout << "Enter unit price: ";
        cin >> s.price;

        appendSaleToCSV(filename, s);

        cout << "Record saved to sales.csv.\n";
        cout << "Add another record? (y/n): ";
        cin >> choice;

    } while (choice == 'y' || choice == 'Y');

    cout << "Do you want to update any record? (y/n): ";
    cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        vector<Sale> sales = loadSales(filename);
        if (sales.empty()) {
            cout << "No records found.\n";
            return 0;
        }

        int targetId;
        cout << "Enter sale ID to update: ";
        cin >> targetId;
        bool found = false;

        for (auto& s : sales) {
            if (s.id == targetId) {
                found = true;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                do {
                    cout << "Enter new date (DD/MM/YYYY): ";
                    cin >> s.date;
                    if (!isValidDate(s.date))
                        cout << "Invalid date, try again.\n";
                } while (!isValidDate(s.date));

                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << "Enter new item name: ";
                getline(cin, s.itemName);

                cout << "Enter new quantity: ";
                cin >> s.quantity;

                cout << "Enter new unit price: ";
                cin >> s.price;

                break;
            }
        }

        if (found) {
            saveSales(filename, sales);
            cout << "Record updated successfully.\n";
        } else {
            cout << "Sale ID not found.\n";
        }
    }

    cout << "Do you want to delete any record? (y/n): ";
    cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        vector<Sale> sales = loadSales(filename);
        if (sales.empty()) {
            cout << "No records found.\n";
            return 0;
        }

        int targetId;
        bool found = false;

        do {
            cout << "Enter sale ID to delete: ";
            cin >> targetId;

            for (const auto& s : sales) {
                if (s.id == targetId) {
                    found = true;
                    break;
                }
            }

            if (found) {
                deleteSaleById(sales, targetId);
                saveSales(filename, sales);
                cout << "Record deleted successfully.\n";
                break;
            } else {
                cout << "Invalid sale ID. Please try again.\n";
            }
        } while (!found);
    }

    // Automatically sort and save to temp.csv
    createSortedTempFile(filename);

    cout << "\nAll operations completed.\n";
    return 0;
}

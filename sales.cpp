#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <random>
#include <ctime>
#include <map>

using namespace std;

struct Sale {
    string date;  // Sales date in YYYY-MM-DD format
    int id;
    string itemName;
    int quantity;
    double price;
};

// Function to generate random sale IDs
int generateRandomId() {
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<> dis(1000, 9999);
    return dis(gen);
}

bool isValidDate(const string& date) {
    if (date.size() != 10 || date[2] != '/' || date[5] != '/') return false;
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!isdigit(date[i])) return false;
    }

    // Extract and validate day, month, and year
    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));

    if (month < 1 || month > 12) return false;  // Invalid month
    if (day < 1 || day > 31) return false;      // Invalid day (we'll ignore month-specific day limits)
    if (year < 2010 || year > 2025) return false; // Year must be between 2010 and 2025

    return true;
}
// Function to append a sale to the CSV file
void appendSaleToCSV(const string& filename, const Sale& s) {
    ofstream file(filename, ios::app);
    if (!file) {
        cerr << "Error: Cannot open " << filename << " for writing.\n";
        return;
    }

    file << s.date << "," << s.id << "," << s.itemName << ","
         << s.quantity << "," << fixed << setprecision(2) << s.price << "\n";
}

// Function to load sales from CSV file
vector<Sale> loadSales(const string& filename) {
    vector<Sale> sales;
    ifstream file(filename);
    string line;

    if (!file) return sales;
    getline(file, line); // Skip header

    while (getline(file, line)) {
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

// Function to save the sales to the CSV file
void saveSales(const string& filename, const vector<Sale>& sales) {
    ofstream file(filename);
    if (!file) {
        cerr << "Error: Cannot open " << filename << " for writing.\n";
        return;
    }

    file << "date,saleID,item name,item quantity,price\n";
    for (const auto& s : sales) {
        file << s.date << "," << s.id << "," << s.itemName << ","
             << s.quantity << "," << fixed << setprecision(2) << s.price << "\n";
    }
}

// Function to get the current system date in DD-MM-YYYY format
string get_today_date() {
    time_t now = time(0);
    tm *ltm = localtime(&now);

    stringstream ss;
    ss << setfill('0') << setw(2) << ltm->tm_mday << "-";  // DD
    ss << setfill('0') << setw(2) << 1 + ltm->tm_mon << "-"; // MM
    ss << 1900 + ltm->tm_year; // YYYY
    return ss.str();
}

// Function to convert date from DD/MM/YYYY to YYYY-MM-DD
string convert_date_format(const string& date) {
    return date.substr(6, 4) + "-" + date.substr(3, 2) + "-" + date.substr(0, 2); // YYYY-MM-DD
}

// Function to generate the report
void generate_report(const vector<Sale>& sales) {
    ofstream report("report.txt");
    if (!report) {
        cerr << "Error opening report.txt for writing.\n";
        return;
    }

    // Write the report date in DD-MM-YYYY format
    report << "Date: " << get_today_date() << "\n";
    report << "Sales Report : Stationary Items Sold\n\n";

    // Group sales by date (convert dates to YYYY-MM-DD for consistent grouping)
    map<string, vector<Sale>> sales_by_date;
    for (const auto& s : sales) {
        string formatted_date = convert_date_format(s.date);
        sales_by_date[formatted_date].push_back(s);
    }

    double grand_total = 0.0;
    const string line_sep = string(100, '-');

    // Iterate through each date and generate the report
    for (const auto& [date, sales_vec] : sales_by_date) {
        report << line_sep << "\n\n";
        report << "Date                SaleID             ItemName        Quantity     Price          SalesAmount \n\n";
        report << line_sep << "\n\n";

        double subtotal = 0.0;
        for (const auto& sale : sales_vec) {
            double sales_amount = sale.quantity * sale.price;
            subtotal += sales_amount;

            report << left
                   << setw(18) << date
                   << setw(18) << sale.id
                   << setw(16) << sale.itemName
                   << setw(12) << sale.quantity
                   << setw(14) << fixed << setprecision(2) << sale.price
                   << setw(18) << fixed << setprecision(2) << sales_amount
                   << "\n";
        }

        // Write subtotal for the day
        report << "\n" << line_sep << "\n\n";
        report << setw(70) << " " << "Subtotal for " << date << " is :" << fixed << setprecision(2) << subtotal << "\n\n";
        report << line_sep << "\n\n";
        grand_total += subtotal;
    }

    // Write grand total
    report << setw(70) << " " << "GRAND TOTAL is : " << fixed << setprecision(2) << grand_total << "\n\n";
    report << "Submission\n";
    report << "End of Report\n";

    report.close();
    cout << "Report generated successfully in report.txt\n";
}

// Function to sort and copy sales to temp.csv
void createSortedTempFile(const string& filename) {
    vector<Sale> sales = loadSales(filename);
    sort(sales.begin(), sales.end(), [](const Sale& a, const Sale& b) {
        return a.date < b.date;
    });
    saveSales("temp.csv", sales);
    cout << "Sales records have been sorted and saved to temp.csv.\n";
}


// Function to validate if item name contains only alphabetic characters
bool isValidItemName(const string& itemName) {
    return all_of(itemName.begin(), itemName.end(), ::isalpha);
}

// Inside the main function, where item name is entered:


// Function to validate integer input (quantity)
bool isValidInt(int& value) {
    string input;
    cin >> input;
    stringstream ss(input);
    return ss >> value && ss.eof();
}

// Function to validate double input (price)
bool isValidDouble(double& value) {
    string input;
    cin >> input;
    stringstream ss(input);
    return ss >> value && ss.eof();
}

// Function to get 'y' or 'n' input only
// Function to get 'y' or 'n' input only
char getYorN(const string& prompt) {
    string choice;
    while (true) {
        cout << prompt;
        cin >> choice;
        transform(choice.begin(), choice.end(), choice.begin(), ::tolower);  // Convert input to lowercase for uniformity

        if (choice == "y" || choice == "yes") {
            return 'y';
        } else if (choice == "n" || choice == "no") {
            return 'n';
        } else {
            cout << "Invalid answer. Please enter 'y' for yes or 'n' for no.\n";
            // The loop will continue and ask the same question
        }
    }
}





// Function to display all sales records
void displaySales(const vector<Sale>& sales) {
    cout << "\nSales Records:\n";
    cout << left
         << setw(18) << "Date"
         << setw(18) << "SaleID"
         << setw(20) << "Item Name"
         << setw(12) << "Quantity"
         << setw(14) << "Price"
         << setw(18) << "Sales Amount"
         << "\n";
    cout << string(100, '-') << "\n";

    for (const auto& sale : sales) {
        double sales_amount = sale.quantity * sale.price;
        cout << left
             << setw(18) << sale.date
             << setw(18) << sale.id
             << setw(20) << sale.itemName
             << setw(12) << sale.quantity
             << setw(14) << fixed << setprecision(2) << sale.price
             << setw(18) << fixed << setprecision(2) << sales_amount
             << "\n";
    }
    cout << string(100, '-') << "\n";
}



int main() {
    const string filename = "sales.csv";
    char choice;

    // Add sale records
    do {
        Sale s;
        s.id = generateRandomId();

        // Validate date
        do {
            cout << "Enter date (DD/MM/YYYY): ";
            cin >> s.date;
            if (!isValidDate(s.date)) cout << "Invalid date, try again.\n";
        } while (!isValidDate(s.date));

        cin.ignore();

        // Validate item name (only alphabetic characters)
        do {
            cout << "Enter item name: ";
            getline(cin, s.itemName);
            if (!isValidItemName(s.itemName)) {
                cout << "Invalid item name. Only alphabetic characters are allowed. Try again.\n";
            }
        } while (!isValidItemName(s.itemName));

        // Validate quantity (only integers)
        do {
            cout << "Enter item quantity: ";
        } while (!isValidInt(s.quantity));  // Loops until a valid integer is entered

        // Validate price (only doubles)
        do {
            cout << "Enter unit price: ";
        } while (!isValidDouble(s.price));  // Loops until a valid double is entered

        appendSaleToCSV(filename, s);
        cout << "Record saved.\n";

        // Ask if the user wants to add another record
        choice = getYorN("Add another record? (y/n): ");

    } while (choice == 'y');  // Loops if 'y' or 'yes' is entered

    // Update sale records
    choice = getYorN("Do you want to update any record? (y/n): ");
    if (choice == 'y') {
        vector<Sale> sales = loadSales(filename);
        displaySales(sales); // Display all sales

        int targetId;
        cout << "Enter sale ID to update: ";
        cin >> targetId;

        auto it = find_if(sales.begin(), sales.end(), [targetId](const Sale& s) { return s.id == targetId; });
        if (it != sales.end()) {
            Sale& s = *it;
            cout << "Enter new date (DD/MM/YYYY): ";
            cin >> s.date;
            cin.ignore();
            cout << "Enter new item name: ";
            getline(cin, s.itemName);
            cout << "Enter new quantity: ";
            cin >> s.quantity;
            cout << "Enter new unit price: ";
            cin >> s.price;
            saveSales(filename, sales);
            cout << "Record updated.\n";
        } else {
            cout << "Sale ID not found.\n";
        }
    }

    // Delete sale records
    choice = getYorN("Do you want to delete any record? (y/n): ");
    if (choice == 'y') {
        vector<Sale> sales = loadSales(filename);
        displaySales(sales); // Display all sales

        int targetId;
        cout << "Enter sale ID to delete: ";
        cin >> targetId;

        auto it = remove_if(sales.begin(), sales.end(), [targetId](const Sale& s) { return s.id == targetId; });
        if (it != sales.end()) {
            sales.erase(it, sales.end());
            saveSales(filename, sales);
            cout << "Record deleted.\n";
        } else {
            cout << "Sale ID not found.\n";
        }
    }

    // Sort and copy to temp.csv
    createSortedTempFile(filename);

    // Generate report
    vector<Sale> sales = loadSales(filename);  // Load all sales for reporting
    generate_report(sales);

    return 0;
}

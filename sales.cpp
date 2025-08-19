#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <filesystem>   
#include <limits>        

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


int getNextId(const string& filename) {
    ifstream file(filename);
    if (!file) return 1; 
    string line;
    getline(file, line); 

    int maxId = 0;
    while (getline(file, line)) {
        if (line.empty()) continue;

        size_t firstComma = line.find(',');
        size_t secondComma = line.find(',', firstComma + 1);
        if (firstComma == string::npos || secondComma == string::npos) continue;

        string idStr = line.substr(firstComma + 1, secondComma - firstComma - 1);

        try {
            int id = stoi(idStr);
            if (id > maxId) maxId = id;
        } catch (...) {
            
        }
    }
    return maxId + 1;
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
        cerr << " Error: Cannot open " << filename << " for writing.\n";
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

int main() {
    const string filename = "sales.csv";
    int nextId = getNextId(filename);

    char choice;
    do {
        Sale s;
        s.id = nextId++;

        // Date validation loop
        do {
            cout << "Enter date (DD/MM/YYYY): ";
            cin >> s.date;
            if (!isValidDate(s.date))
                cout << "Invalid date, try again.\n";
        } while (!isValidDate(s.date));

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer

        cout << "Enter item name: ";
        getline(cin, s.itemName);

        cout << "Enter item quantity: ";
        cin >> s.quantity;

        cout << "Enter unit price: ";
        cin >> s.price;

        appendSaleToCSV(filename, s);

        cout << " Record saved to sales.csv.\n";
        cout << " Add another record? (y/n): ";
        cin >> choice;

    } while (choice == 'y' || choice == 'Y');

    cout << "\n All records saved successfully.\n";
    return 0;
}

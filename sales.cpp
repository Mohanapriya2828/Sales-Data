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

        cout << "Enter date (YYYY-MM-DD): ";
        cin >> s.date;

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

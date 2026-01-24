#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <ctime>
#include <limits>
#include <vector>
#include <cctype>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[94m"
#define YELLOW "\033[93m"
using namespace std;

enum Status
{
    AVAILABLE,
    OUT_OF_STOCK
};
enum SortType
{
    NAME,
    PRICE
};

struct Product
{
    string id;
    string name;
    string category;
    float price;
    int quantity;
    Status status;
};

struct ProductSales
{
    string id;
    string name;
    int unitsSold = 0;
    float sales = 0;
    float tax = 0;
};

struct ShopData
{
    int receiptCount = 0;
    int totalItemsSold = 0;
    float totalSales = 0;
    float totalTax = 0;
    vector<ProductSales> productSummary;
};

const float TAX_RATE = 0.1f;

string normalizer(const string txt);
string inputString(const string msg);
template <typename T>
T inputNumber(const string msg, T min);

void loadProducts(vector<Product> &products, int &count, const char *file);
void saveProducts(vector<Product> &products, int count, const char *file);
void loadCategories(vector<string> &categories);

template <typename T>
int searchById(vector<T> &items, int count, const string &id);
void viewProducts(vector<Product> &products, int count, bool hideOut, const vector<string> &categories, const string &filterCategory = "all");
void addProduct(vector<Product> &products, int &count, const char *file, vector<string> &categories);
void editProduct(vector<Product> &products, int count, const char *file);
void deleteProduct(vector<Product> &products, int &count, const char *file);
void sortProducts(vector<Product> &products, int count, SortType type);
void searchProducts(vector<Product> &products, int count, int role);
void generateReceipt(const Product &products, int quantity, ShopData &data, const int count);
void buyProduct(vector<Product> &products, int count, const char *file, ShopData &data, const vector<string> &categories);
void viewSellerSummary(const ShopData &data, int productCount);
void userMenu(vector<Product> &products, int &count, const char *file, int role, ShopData &data, vector<string> &categories);
void logActivity(const string &action);

int main()
{
    // system("color 90");
    int productCount = 0;
    const char *fileName = "products.txt";

    vector<Product> products;
    ShopData shopData;
    vector<string> categories;

    loadProducts(products, productCount, fileName);
    loadCategories(categories);

    int choice;
    do
    {
        cout << BLUE << "\n=== SHOP BILLING SYSTEM ===\n"
             << RESET;
        cout << "1. Seller\n2. Customer\n0. Exit\n";
        choice = inputNumber<int>("Choose role: ", 0);
        switch (choice)
        {

        case 1:
            userMenu(products, productCount, fileName, 1, shopData, categories);
            break;
        case 2:
            userMenu(products, productCount, fileName, 2, shopData, categories);
            break;
        case 0:
            cout << YELLOW << "Goodbye!\n"
                 << RESET;
            return 0;
        default:
            cout << RED << "Invalid Input\n"
                 << RESET;
        }

    } while (choice != 0);
    return 0;
}

string normalizer(const string text)
{
    string normalizedText;
    for (int i = 0; i < text.length(); i++)
    {
        if (i == 0)
        {
            normalizedText += toupper(text[i]);
        }
        else
        {
            normalizedText += tolower(text[i]);
        }
    }
    return normalizedText;
}

string inputString(const string msg)
{
    string s;
    cout << msg;
    getline(cin >> ws, s);
    return s;
}

template <typename T>
T inputNumber(const string msg, T min)
{
    T x;
    while (true)
    {
        cout << msg;
        cin >> x;
        if (cin.fail() || x < min)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << RED << "Invalid input. Try again.\n"
                 << RESET;
        }
        else
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return x;
        }
    }
}

void loadProducts(vector<Product> &products, int &count, const char *file)
{
    ifstream fin(file);
    static bool opened = false;
    if (!fin && opened)
    {
        cerr << RED << "Failed to Open product file.\n"
             << RESET;
        return;
    }
    else if (!fin)
    {
        opened = true;
        return;
    }

    fin >> count;
    fin.ignore();

    products.clear();
    for (int i = 0; i < count; i++)
    {
        Product p;

        getline(fin, p.id);
        getline(fin, p.name);
        getline(fin, p.category);
        fin >> p.price;
        fin >> p.quantity;
        int st;
        fin >> st;
        p.status = static_cast<Status>(st);
        fin.ignore();

        products.push_back(p);
    }

    fin.close();
}

void saveProducts(vector<Product> &products, int count, const char *file)
{
    ofstream fout(file);
    if (!fout)
    {
        cerr << RED << "Failed to open product file.\n"
             << RESET;
        return;
    }
    fout << count << "\n";
    for (int i = 0; i < count; i++)
    {
        fout << products[i].id << "\n"
             << products[i].name << "\n"
             << products[i].category << "\n"
             << products[i].price << "\n"
             << products[i].quantity << "\n"
             << products[i].status << "\n";
    }

    fout.close();
}

void loadCategories(vector<string> &categories)
{
    ifstream fin("categories.txt");
    static bool opened = false;
    if (!fin && opened)
    {
        cerr << RED << "Failed to Open categories file.\n"
             << RESET;
        return;
    }
    else if (!fin)
    {
        opened = true;
        return;
    }
    string cat;
    while (getline(fin, cat))
    {
        categories.push_back(cat);
    }
    fin.close();
}

void logActivity(const string &action)
{
    ofstream fout("activity_log.txt", ios::app);
    if (!fout)
    {
        cerr << RED << "Failed to open activity_log  file.\n"
             << RESET;
    }
    time_t now = time(0);
    char *dt = ctime(&now);
    fout << "[" << dt << "] " << action << endl;
    fout.close();
}

template <typename T>
int searchById(vector<T> &items, int count, const string &id)
{
    for (int i = 0; i < count; i++)
        if (normalizer(items[i].id) == normalizer(id))
            return i;
    return -1;
}

void viewProducts(vector<Product> &products, int count, bool hideOut, const vector<string> &categories, const string &filterCategory)
{
    bool hasProduct = false;
    for (int i = 0; i < count; i++)
    {
        if ((!hideOut || products[i].status == AVAILABLE) &&
            (filterCategory == "all" || normalizer(products[i].category) == normalizer(filterCategory)))
        {
            hasProduct = true;
            break;
        }
    }
    if (!hasProduct)
    {
        cout << YELLOW << "\nNo products available.\n"
             << RESET;
        return;
    }

    cout << "\nID     Name            Category        Price(ETB)  Qty\n";
    cout << "-----------------------------------------------------\n";

    for (int i = 0; i < count; i++)
    {
        if ((hideOut && products[i].status == OUT_OF_STOCK))
            continue;
        if (filterCategory != "all" && normalizer(products[i].category) != normalizer(filterCategory))
            continue;

        cout << products[i].id << " "
             << setw(7) << products[i].name << " "
             << setw(14) << products[i].category << " "
             << setw(14) << fixed << setprecision(2) << products[i].price << " "
             << setw(4) << products[i].quantity << endl;
    }
}

void addProduct(vector<Product> &products, int &count, const char *file, vector<string> &categories)
{
    Product p;
    string tempId;
    do
    {
        tempId = inputString("Enter product ID: ");
        if (searchById<Product>(products, count, tempId) == -1)
        {
            p.id = tempId;
            break;
        }
        cout << RED << "NOTICE!! Product with the same ID exists.\n"
             << RESET;
    } while (true);

    p.name = inputString("Enter product name: ");
    p.name = normalizer(p.name);

    cout << BLUE << "Available categories:\n"
         << RESET;
    int idx = 1;
    vector<string> catList(categories.begin(), categories.end());
    for (const auto &c : catList)
        cout << idx++ << ". " << c << endl;
    cout << idx << ". Add new category\n";

    int catChoice = inputNumber<int>("Choose category: ", 1);
    if (catChoice == idx)
    {
        p.category = inputString("Enter new category: ");
        p.category = normalizer(p.category);
        categories.push_back(p.category);
        ofstream fout("categories.txt", ios::app);
        if (!fout)
        {
            cerr << RED << "Failed to open categories file.\n"
                 << RESET;
        }
        else
            fout << p.category << endl;
        fout.close();
    }
    else
    {
        p.category = catList[catChoice - 1];
    }

    p.price = inputNumber<float>("Enter price (ETB): ", 0.01f);
    p.quantity = inputNumber<int>("Enter quantity: ", 0);
    p.status = (p.quantity > 0) ? AVAILABLE : OUT_OF_STOCK;

    products.push_back(p);
    count++;
    saveProducts(products, count, file);
    logActivity("Added product ID: " + p.id);
    cout << GREEN << "Product added successfully!\n"
         << RESET;
}

void editProduct(vector<Product> &products, int count, const char *file)
{
    string id = inputString("Enter product ID to edit: ");
    int i = searchById<Product>(products, count, id);
    if (i == -1)
    {
        cout << RED << "Product not found.\n"
             << RESET;
        return;
    }


    cout << "Product: " << products[i].name << "\nCategory: " << products[i].category
         << "\nPrice: " << products[i].price << " ETB\nAvailable Quantity: " << products[i].quantity << endl;
    cout <<BLUE<<"Property to edit.\n"<<RESET;
     cout<<"1.Name   2.Id   3.Price   4.Quantity  5.all\n";
    int choice=inputNumber<int>("Choice: ",1);
    switch(choice){
    	case 1:
    		products[i].name=normalizer(inputString("New name: "));
    		break;
    	case 2:
    		products[i].id=inputString("New Id: ");
    		break;
    	case 3:
    		products[i].price = inputNumber<float>("New price (ETB): ", 0.01f);
    		break;
    	case 4:
    		products[i].quantity = inputNumber<int>("New quantity: ", 0);
    		products[i].status = (products[i].quantity > 0) ? AVAILABLE : OUT_OF_STOCK;
    		break;
    	case 5:
    		products[i].id=inputString("New Id: ");
    		products[i].name=normalizer(inputString("New name: "));
    		products[i].price = inputNumber<float>("New price (ETB): ", 0.01f);
    		products[i].quantity = inputNumber<int>("New quantity: ", 0);
    		products[i].status = (products[i].quantity > 0) ? AVAILABLE : OUT_OF_STOCK;
    		break;
    	default:
    		cout<<RED<<"Invalid input\n "<<RESET;
	}
   
    
    
    
    logActivity("Edited product ID: " + products[i].id);
    saveProducts(products, count, file);
    
    cout << GREEN << "Product edited successfully!\n"
         << RESET;
}

void deleteProduct(vector<Product> &products, int &count, const char *file)
{
    string id = inputString("Enter product ID to delete: ");
    int i = searchById<Product>(products, count, id);
    if (i == -1)
    {
        cout << YELLOW << "Product not found.\n"
             << RESET;
        return;
    }

    char confirm;
    cout << YELLOW << "Are you sure you want to delete this product? (Y/N): " << RESET;
    cin >> confirm;
    cin.ignore();
    if (confirm != 'Y' && confirm != 'y')
    {
        cout << YELLOW << "Action canceled.\n"
             << RESET;
        return;
    }

    products.erase(products.begin() + i);
    count--;
    saveProducts(products, count, file);
    logActivity("Deleted product ID: " + id);
    cout << GREEN << "Product deleted successfully!\n"
         << RESET;
}

void sortProducts(vector<Product> &products, int count, SortType type)
{
    for (int i = 0; i < count - 1; i++)
        for (int j = i + 1; j < count; j++)
        {
            if ((type == NAME && normalizer(products[i].name) > normalizer(products[j].name)) ||
                (type == PRICE && products[i].price > products[j].price))
            {
                swap(products[i], products[j]);
            }
        }
    cout << GREEN << "Product sorted successfully!\n"
         << RESET;
}

void searchProducts(vector<Product> &products, int count, int role)
{
    bool found = false;
    string key = inputString("Enter product name or ID to search: ");
    cout << "\nID   Name            Category        Price(ETB)  Qty\n";
    cout << "-----------------------------------------------------\n";

    for (int i = 0; i < count; i++)
    {
        if (normalizer(products[i].id).find(normalizer(key)) != string::npos ||
            normalizer(products[i].name).find(normalizer(key)) != string::npos)
        {
            cout << products[i].id << " "
                 << setw(10) << products[i].name << " "
                 << setw(14) << products[i].category << " "
                 << setw(10) << fixed << setprecision(2) << products[i].price << " "
                 << setw(4) << products[i].quantity << endl;
            found = true;
        }
    }
    if (!found)
        cout << YELLOW << "No products found.\n"
             << YELLOW;
}

void generateReceipt(const Product &products, int quantity, ShopData &data, const int count)
{
    data.receiptCount++;

    string receiptID = string("R") +
                       string(6 - to_string(data.receiptCount).length(), '0') +
                       to_string(data.receiptCount);

    time_t now = time(0);
    char *dt = ctime(&now);

    float subtotal = quantity * products.price;
    float tax = subtotal * TAX_RATE;
    float total = subtotal + tax;

    data.totalItemsSold += quantity;
    data.totalSales += subtotal;
    data.totalTax += tax;

    int idx = searchById<ProductSales>(data.productSummary,
                                       static_cast<int>(data.productSummary.size()),
                                       products.id);

    if (idx == -1)
    {
        ProductSales ps;
        ps.id = products.id;
        ps.name = products.name;
        ps.unitsSold = quantity;
        ps.sales = subtotal;
        ps.tax = tax;
        data.productSummary.push_back(ps);
    }
    else
    {
        data.productSummary[idx].unitsSold += quantity;
        data.productSummary[idx].sales += subtotal;
        data.productSummary[idx].tax += tax;
    }

    {
        cout << BLUE << "\n=== RECEIPT ===\n"
             << RESET;
        cout << "Receipt ID: " << receiptID << endl;
        cout << "Time: " << dt;
        cout << "Product: " << products.name << endl;
        cout << "Quantity: " << quantity << endl;
        cout << "Price per unit: " << fixed << setprecision(2) << products.price << " ETB" << endl;
        cout << "Subtotal: " << fixed << setprecision(2) << subtotal << " ETB" << endl;
        cout << "Tax: " << fixed << setprecision(2) << tax << " ETB" << endl;
        cout << "Total: " << fixed << setprecision(2) << total << " ETB" << endl;
        cout << BLUE << "================\n"
             << RESET;
    }
    ofstream fout("receipts.txt", ios::app);
    if (!fout)
    {
        cerr << RED << "Failed to open the receipt file.\n"
             << RESET;
        return;
    }

    {
        fout << "Receipt ID: " << receiptID << "\n";
        fout << "Time: " << dt;
        fout << "Product: " << products.name << "\n";
        fout << "Quantity: " << quantity << "\n";
        fout << "Price per unit: " << fixed << setprecision(2) << products.price << " ETB\n";
        fout << "Subtotal: " << fixed << setprecision(2) << subtotal << " ETB\n";
        fout << "Tax: " << fixed << setprecision(2) << tax << " ETB\n";
        fout << "Total: " << fixed << setprecision(2) << total << " ETB\n";
        fout << "----------------------\n";
    }
    fout.close();
}

void buyProduct(vector<Product> &products, int count, const char *file, ShopData &data, const vector<string> &categories)
{
    cout << BLUE << "Available categories:\n"
         << RESET;
    int idx = 1;
    vector<string> catList(categories.begin(), categories.end());
    for (const auto &c : catList)
        cout << idx++ << ". " << c << endl;
    cout << idx << ". All\n";
    while (true)
    {
        int catChoice = inputNumber<int>("Choose category to view products: ", 1);
        if (catChoice > idx)
        {
            cout << RED << "Invalid Input!\n"
                 << RESET;
            continue;
        }
        else
        {
            string selectedCat = (catChoice == idx) ? "all" : catList[catChoice - 1];
            viewProducts(products, count, true, categories, selectedCat);
            break;
        }
    }

    string id = inputString("Enter product ID to buy: ");
    int i = searchById(products, count, id);
    if (i == -1 || products[i].status == OUT_OF_STOCK)
    {
        cout << YELLOW << "Product unavailable.\n"
             << RESET;
        return;
    }

    cout << "Product: " << products[i].name << "\nCategory: " << products[i].category
         << "\nPrice: " << products[i].price << " ETB\nAvailable Quantity: " << products[i].quantity << endl;

    int qty = inputNumber<int>("Enter quantity: ", 1);
    if (qty > products[i].quantity)
    {
        cout << YELLOW << "Not enough stock.\n"
             << RESET;
        return;
    }

    char confirm;
    cout << YELLOW << "Confirm purchase? (Y/N): " << RESET;
    cin >> confirm;
    cin.ignore();
    if (confirm != 'Y' && confirm != 'y')
    {
        cout << RED << "Order canceled.\n"
             << RESET;
        return;
    }

    products[i].quantity -= qty;
    if (products[i].quantity == 0)
        products[i].status = OUT_OF_STOCK;
    saveProducts(products, count, file);

    generateReceipt(products[i], qty, data, count);
    logActivity("Customer bought product ID: " + products[i].id);
}

void viewSellerSummary(const ShopData &data, int productCount)
{
    time_t now = time(0);
    char *dt = ctime(&now);

    cout << "\n Time: " << dt;
    cout << "\n=== SELLER SUMMARY PER PRODUCT ===\n";
    cout << "ID   Name            Units Sold   Sales(ETB)   Tax(ETB)\n";
    cout << "------------------------------------------------------\n";

    for (const auto &ps : data.productSummary)
    {
        cout << setw(4) << ps.id << " "
             << setw(14) << ps.name << " "
             << setw(10) << ps.unitsSold << " "
             << setw(12) << fixed << setprecision(2) << ps.sales << " "
             << setw(10) << fixed << setprecision(2) << ps.tax << endl;
    }
    cout << "Total Items Sold: " << data.totalItemsSold << endl;
    cout << "Total Sales: " << fixed << setprecision(2) << data.totalSales << " ETB\n";
    cout << "Total Tax Collected: " << fixed << setprecision(2) << data.totalTax << " ETB\n";
    cout << "Total: " << fixed << setprecision(2) << data.totalTax + data.totalSales << " ETB\n";

    ofstream fout("Summary.txt", ios::app);

    if (!fout)
    {
        cerr << RED << "Failed to open the summary file.\n"
             << RESET;
        return;
    }
    else
    {

        fout << "\n Time: " << dt;
        fout << "\n=== SELLER SUMMARY PER PRODUCT ===\n";
        fout << "ID   Name            Units Sold   Sales(ETB)   Tax(ETB)\n";
        fout << "------------------------------------------------------\n";
        for (const auto &ps : data.productSummary)
        {
            fout << setw(4) << ps.id << " "
                 << setw(14) << ps.name << " "
                 << setw(10) << ps.unitsSold << " "
                 << setw(12) << fixed << setprecision(2) << ps.sales << " "
                 << setw(10) << fixed << setprecision(2) << ps.tax << endl;
        }
        fout << "Total Items Sold: " << data.totalItemsSold << endl;
        fout << "Total Sales: " << fixed << setprecision(2) << data.totalSales << " ETB\n";
        fout << "Total Tax Collected: " << fixed << setprecision(2) << data.totalTax << " ETB\n";
        fout << "Total Tax Collected: " << fixed << setprecision(2) << data.totalTax + data.totalSales << " ETB\n";
        fout << "------------------------------------------------------\n";
    }
    fout.close();
}

void userMenu(vector<Product> &products, int &count, const char *file, int role, ShopData &data, vector<string> &categories)
{
    static int pass = 1234;
    static bool flag = true;
    int choice;
    do
    {
        if (role == 1)
        {
            while (flag)
            {
                int password = inputNumber<int>("Enter Password to continue: ", 0);
                if (password == pass)
                {
                    flag = false;
                    break;
                }
                else
                    cout << RED << "Wrong password!\n"
                         << RESET;
            }

            cout << BLUE << "\n--- SELLER MENU ---\n"
                 << RESET;
            cout << "1. Add Product\n2. Edit Product\n3. Delete Product\n4. View Products\n5. Sort Products\n6. Search Products\n7. View Summary\n8. Change Password\n0. Back\n";
            choice = inputNumber<int>("Choice: ", 0);

            switch (choice)
            {
            case 1:
                addProduct(products, count, file, categories);
                break;
            case 2:
                editProduct(products, count, file);
                break;
            case 3:
                deleteProduct(products, count, file);
                break;
            case 4:
            {
                cout << BLUE << "Filter by category:\n"
                     << RESET << "1. All\n";
                int idx = 2;
                vector<string> catList(categories.begin(), categories.end());
                for (const auto &c : catList)
                    cout << idx++ << ". " << c << endl;
                while (true)
                {
                    int cChoice = inputNumber<int>("Choose: ", 1);
                    if (cChoice >= idx)
                    {
                        cout << RED << "Invalid Input!\n"
                             << RESET;
                        continue;
                    }
                    else
                    {
                        string selCat = (cChoice == 1) ? "all" : catList[cChoice - 2];
                        viewProducts(products, count, false, categories, selCat);
                        break;
                    }
                }

                break;
            }
            case 5:
            {
                cout << BLUE << "Sort by: \n"
                     << RESET << "1. Name 2. Price 0. Back\n";
                while (true)
                {
                    int s = inputNumber<int>("Choice: ", 0);
                    if (s == 0)
                        break;
                    else if (s > 2)
                        cout << RED << "Invalid Input!\n"
                             << RESET;
                    else
                    {
                        sortProducts(products, count, s == 1 ? NAME : PRICE);
                        break;
                    }
                }
                break;
            }
            case 6:
                searchProducts(products, count, role);
                break;
            case 7:
                viewSellerSummary(data, count);
                break;
            case 8:
                while (true)
                {
                    int password = inputNumber<int>("Enter Old Password: ", 1000);
                    if (password == pass)
                    {
                        pass = inputNumber<int>("Enter New Password(at least 4 digit number): ", 1000);
                        cout << GREEN << "Password changed successfully.\n"
                             << RESET;
                        break;
                    }
                    else
                        cout << RED << "Wrong password!\n"
                             << RESET;
                }
                break;
            case 0:
                flag = true;
                break;
            default:
                cout << RED << "Invalid Input!\n"
                     << RESET;
            }
        }
        else
        {
            cout << BLUE << "\n--- CUSTOMER MENU ---\n"
                 << RESET;
            cout << "1. View Products\n2. Sort Products\n3. Buy Product\n4. Search Products\n0. Back\n";
            choice = inputNumber<int>("Choice: ", 0);
            switch (choice)
            {
            case 1:
            {
                cout << BLUE << "Filter by category:\n"
                     << RESET << "1. All\n";
                int idx = 2;
                vector<string> catList(categories.begin(), categories.end());
                for (const auto &c : catList)
                    cout << idx++ << ". " << c << endl;
                while (true)
                {
                    int cChoice = inputNumber<int>("Choose: ", 1);
                    if (cChoice >= idx)
                    {
                        cout << RED << "Invalid Input!\n"
                             << RESET;
                        continue;
                    }
                    else
                    {
                        string selCat = (cChoice == 1) ? "all" : catList[cChoice - 2];
                        viewProducts(products, count, true, categories, selCat);
                        break;
                    }
                }
                break;
            }
            case 2:
            {
                cout << BLUE << "Sort by: \n"
                     << RESET << "1. Name 2. Price 0. Back\n";
                while (true)
                {
                    int s = inputNumber<int>("Choice: ", 0);
                    if (s == 0)
                        break;
                    else if (s > 2)
                        cout << RED << "Invalid Input!\n"
                             << RESET;
                    else
                    {
                        sortProducts(products, count, s == 1 ? NAME : PRICE);
                        break;
                    }
                }
                break;
            }
            case 3:
                buyProduct(products, count, file, data, categories);
                break;
            case 4:
                searchProducts(products, count, role);
                break;
            case 0:
                break;
            default:
                cout << RED << "Invalid Input!\n"
                     << RESET;
            }
        }
    } while (choice != 0);
}
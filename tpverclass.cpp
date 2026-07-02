#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const int MAX_ENTRIES = 5000;

struct StringIntEntry { string key; int value; };
struct StringDoubleEntry { string key; double value; };
struct StringStringEntry { string key; string value; };
struct ProductSummary { string key; int sold; int transactions; };
struct MemberSummary { string key; double revenue; int count; };

int findStringIntEntry(const StringIntEntry* entries, int size, const string& key) {
    for (int i = 0; i < size; ++i) {
        if (entries[i].key == key) return i;
    }
    return -1;
}

int findStringDoubleEntry(const StringDoubleEntry* entries, int size, const string& key) {
    for (int i = 0; i < size; ++i) {
        if (entries[i].key == key) return i;
    }
    return -1;
}

int findStringStringEntry(const StringStringEntry* entries, int size, const string& key) {
    for (int i = 0; i < size; ++i) {
        if (entries[i].key == key) return i;
    }
    return -1;
}

void addStringIntEntry(StringIntEntry* entries, int& size, const string& key, int delta) {
    int idx = findStringIntEntry(entries, size, key);
    if (idx >= 0) {
        entries[idx].value += delta;
        return;
    }
    entries[size].key = key;
    entries[size].value = delta;
    ++size;
}

void addStringDoubleEntry(StringDoubleEntry* entries, int& size, const string& key, double delta) {
    int idx = findStringDoubleEntry(entries, size, key);
    if (idx >= 0) {
        entries[idx].value += delta;
        return;
    }
    entries[size].key = key;
    entries[size].value = delta;
    ++size;
}

void addStringStringEntry(StringStringEntry* entries, int& size, const string& key, const string& value) {
    int idx = findStringStringEntry(entries, size, key);
    if (idx >= 0) {
        entries[idx].value = value;
        return;
    }
    entries[size].key = key;
    entries[size].value = value;
    ++size;
}

int getStringIntValue(const StringIntEntry* entries, int size, const string& key) {
    int idx = findStringIntEntry(entries, size, key);
    return idx >= 0 ? entries[idx].value : 0;
}

double getStringDoubleValue(const StringDoubleEntry* entries, int size, const string& key) {
    int idx = findStringDoubleEntry(entries, size, key);
    return idx >= 0 ? entries[idx].value : 0.0;
}

void swapProductSummary(ProductSummary& a, ProductSummary& b) {
    ProductSummary tmp = a;
    a = b;
    b = tmp;
}

void swapMemberSummary(MemberSummary& a, MemberSummary& b) {
    MemberSummary tmp = a;
    a = b;
    b = tmp;
}

const string USER_FILE = "users.txt";
const string PRODUCTS_FILE = "products.txt";
const string CATEGORIES_FILE = "categories.txt";
const string CART_FILE = "cart.txt";
const string TRANSACTIONS_FILE = "transactions.txt";
const string MEMBERS_FILE = "members.txt";
const string MEMBER_HISTORY_FILE = "member_history.txt";
const string PROMOS_FILE = "promos.txt";
const string SHIFTS_FILE = "shifts.txt";
const string ATTENDANCE_FILE = "attendance.txt";



// forward declarations
string inputLine(const string& prompt);
bool confirmAction(const string& prompt);
void productMenu();
void categoryMenu();
void ensureDefaultCategories();

// Cart / transaction forward decls
void cartMenu();
bool productCodeExists(const string& code);
bool checkoutCart(const string& curDate);
void saveTransactionToFile(const string& date, double total, const string& items, const string& member, double paid, double change, double discount, const string& promoCode);
void addProduct();
void editProduct();
void deleteProduct();
void addCategory();
void listCategories();
// promos
bool promoCodeExists(const string& code);
void listPromos();
void addPromo();
void editPromo();
void deletePromo();
double applyPromo(const string& promoCode, const string& date, bool isMember, bool &valid, string &outPromoCode);
void promoMenu();
// shift & attendance forward declarations
void shiftMenu();
void addShift();
void listShifts();
void editShift();
void deleteShift();
void attendanceMenuForUser(const string& username);
void recordAttendance(const string& username);
void listAttendance();
void listAttendanceForEmployee(const string& username);
// reports & analytics / financial analysis
void monthlySalesReport();
void bestSellingAnalysis();
void dailySalesReport();
void categorySalesAnalysis();
void revenueByDateRange();
void profitMarginAnalysis();
void memberSalesAnalysis();
void financialDashboard();
void ensureDefaultData();
// transaction history / receipt
void listTransactions();
void viewTransactionDetails(int id);
void searchTransactions(const string& q);
// membership/search prototypes
bool memberExists(const string& uname);
void searchProducts(const string& q);


bool usernameExists(const string& username) {
    ifstream fin(USER_FILE.c_str());
    if (!fin) return false;
    string line;
    while (getline(fin, line)) {
        size_t p = line.find('|');
        if (p != string::npos) {
            string u = line.substr(0, p);
            if (u == username) return true;
        }
    }
    return false;
}

bool validateCredentials(const string& username, const string& password, string& outRole) {
    ifstream fin(USER_FILE.c_str());
    if (!fin) return false;
    string line;
    while (getline(fin, line)) {
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        if (p1 == string::npos || p2 == string::npos) continue;
        string u = line.substr(0, p1);
        string pw = line.substr(p1 + 1, p2 - p1 - 1);
        string role = line.substr(p2 + 1);
        if (u == username && pw == password) {
            outRole = role;
            return true;
        }
    }
    return false;
}

bool registerUser(const string& username, const string& password, const string& role) {
    ofstream fout(USER_FILE.c_str(), ios::app);
    if (!fout) return false;
    fout << username << '|' << password << '|' << role << '\n';
    return true;
}

bool changePassword(const string& username, const string& newPassword) {
    ifstream fin(USER_FILE.c_str());
    string all;
    bool found = false;
    if (fin) {
        string line;
        while (getline(fin, line)) {
            size_t p1 = line.find('|');
            size_t p2 = line.find('|', p1 + 1);
            if (p1 == string::npos || p2 == string::npos) continue;
            string u = line.substr(0, p1);
            string role = line.substr(p2 + 1);
            if (u == username) {
                all += u + '|' + newPassword + '|' + role + '\n';
                found = true;
            } else {
                all += line + '\n';
            }
        }
        fin.close();
    }
    if (!found) return false;
    ofstream fout(USER_FILE.c_str(), ios::trunc);
    if (!fout) return false;
    fout << all;
    return true;
}

void listUsers() {
    ifstream fin(USER_FILE.c_str());
    if (!fin) { cout << "Belum ada pengguna atau file tidak tersedia.\n"; return; }
    string line;
    int idx = 1;
    cout << "\nDaftar pengguna:\n";
    while (getline(fin, line)) {
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        if (p1 == string::npos || p2 == string::npos) continue;
        string u = line.substr(0, p1);
        string role = line.substr(p2 + 1);
        cout << idx++ << ". " << u << " (" << role << ")\n";
    }
    if (idx == 1) cout << "Belum ada pengguna.\n";
}

bool deleteUser(const string& username) {
    ifstream fin(USER_FILE.c_str());
    if (!fin) return false;
    string all;
    string line;
    bool found = false;
    while (getline(fin, line)) {
        size_t p1 = line.find('|');
        if (p1 == string::npos) continue;
        string u = line.substr(0, p1);
        if (u == username) { found = true; continue; }
        all += line + '\n';
    }
    fin.close();
    if (!found) return false;
    // konfirmasi sebelum hapus
    if (!confirmAction(string("Konfirmasi hapus pengguna '") + username + "'?")) return false;
    ofstream fout(USER_FILE.c_str(), ios::trunc);
    if (!fout) return false;
    fout << all;
    return true;
}

string inputLine(const string& prompt) {
    string s;
    cout << prompt;
    getline(cin, s);
    return s;
}

bool confirmAction(const string& prompt) {
    string r = inputLine(prompt + " (y/n): ");
    return (r == "y" || r == "Y");
}

void adminDashboard(const string& username) {
    while (true) {
        cout << "\n--- Dashboard Admin ---\n";
        cout << "Selamat datang, " << username << " (admin)\n";
        cout << "1. Logout\n";
        cout << "2. Ganti password\n";
        cout << "3. Daftar pengguna\n";
        cout << "4. Hapus pengguna\n";
        cout << "5. Buat pengguna baru\n";
        cout << "6. Kelola produk (CRUD)\n";
        cout << "7. Kelola kategori\n";
        cout << "8. Kelola promo\n";
        cout << "9. Riwayat transaksi\n";
        cout << "10. Jadwal Shift\n";
        cout << "11. Laporan & Analisis\n";
        cout << "Pilih: ";
        string choice;
        getline(cin, choice);
        if (choice == "1") break;
        else if (choice == "2") {
            string np = inputLine("Password baru: ");
            string nc = inputLine("Konfirmasi password baru: ");
            if (np == nc) {
                if (changePassword(username, np)) cout << "Password berhasil diubah.\n";
                else cout << "Gagal mengubah password.\n";
            } else cout << "Konfirmasi tidak sama.\n";
        } else if (choice == "3") {
            listUsers();
        } else if (choice == "4") {
            string target = inputLine("Masukkan username yang akan dihapus: ");
            if (target == username) { cout << "Tidak dapat menghapus akun sendiri dari dashboard.\n"; continue; }
            if (!usernameExists(target)) { cout << "Username tidak ditemukan.\n"; continue; }
            if (deleteUser(target)) cout << "Pengguna dihapus.\n";
            else cout << "Gagal menghapus pengguna.\n";
        } else if (choice == "5") {
            string u = inputLine("Pilih username: ");
            if (u.empty()) { cout << "Username tidak boleh kosong.\n"; continue; }
            if (usernameExists(u)) { cout << "Username sudah ada.\n"; continue; }
            string pw = inputLine("Password: ");
            string conf = inputLine("Konfirmasi password: ");
            if (pw != conf) { cout << "Konfirmasi tidak sama.\n"; continue; }
            string role = inputLine("Role (admin/kasir): ");
            if (role != "admin" && role != "kasir") { cout << "Role harus 'admin' atau 'kasir'.\n"; continue; }
            cout << "\nRingkasan pengguna baru:\n";
            cout << " - Username: " << u << "\n";
            cout << " - Role: " << role << "\n";
            if (!confirmAction("Konfirmasi buat pengguna?")) { cout << "Pembuatan pengguna dibatalkan.\n"; continue; }
            if (registerUser(u, pw, role)) cout << "Pengguna baru dibuat.\n";
            else cout << "Gagal membuat pengguna.\n";
        } else if (choice == "6") {
            productMenu();
        } else if (choice == "7") {
            categoryMenu();
        } else if (choice == "8") {
            promoMenu();
        } else if (choice == "9") {
            listTransactions();
        } else if (choice == "10") {
            shiftMenu();
        } else if (choice == "11") {
            financialDashboard();
        } else {
            cout << "Pilihan tidak dikenali.\n";
        }
    }
}

// --- Cart & Transactions implementation ---

const int MAX_CART = 200;

string cartCode[MAX_CART];
string cartName[MAX_CART];
double cartPrice[MAX_CART];
int cartQty[MAX_CART];
double cartSubtotal[MAX_CART];

int cartCount = 0;

class Member;
class Produk;

class Admin{
    private:
        string username;
        string password;
        string namaAdmin;
        int idAdmin;
    public:
        bool login() {
            string u = inputLine("Username: ");
            string p = inputLine("Password: ");
            string role;
            if (validateCredentials(u, p, role) && role == "admin") {
                username = u;
                password = p;
                return true;
            }
            return false;
        }
        void dashboardAdmin(const string& username) {
            while (true) {
                cout << "\n--- Dashboard Admin ---\n";
                cout << "Selamat datang, " << username << " (admin)\n";
                cout << "1. Logout\n";
                cout << "2. Ganti password\n";
                cout << "3. Daftar pengguna\n";
                cout << "4. Hapus pengguna\n";
                cout << "5. Buat pengguna baru\n";
                cout << "6. Kelola produk (CRUD)\n";
                cout << "7. Kelola kategori\n";
                cout << "8. Kelola promo\n";
                cout << "9. Riwayat transaksi\n";
                cout << "10. Jadwal Shift\n";
                cout << "11. Laporan & Analisis\n";
                cout << "Pilih: ";
                string choice;
                getline(cin, choice);
                if (choice == "1") break;
                else if (choice == "2") {
                    string np = inputLine("Password baru: ");
                    string nc = inputLine("Konfirmasi password baru: ");
                    if (np == nc) {
                        if (changePassword(username, np)) cout << "Password berhasil diubah.\n";
                        else cout << "Gagal mengubah password.\n";
                    } else cout << "Konfirmasi tidak sama.\n";
                } else if (choice == "3") {
                    listUsers();
                } else if (choice == "4") {
                    string target = inputLine("Masukkan username yang akan dihapus: ");
                    if (target == username) { cout << "Tidak dapat menghapus akun sendiri dari dashboard.\n"; continue; }
                    if (!usernameExists(target)) { cout << "Username tidak ditemukan.\n"; continue; }
                    if (deleteUser(target)) cout << "Pengguna dihapus.\n";
                    else cout << "Gagal menghapus pengguna.\n";
                } else if (choice == "5") {
                    string u = inputLine("Pilih username: ");
                    if (u.empty()) { cout << "Username tidak boleh kosong.\n"; continue; }
                    if (usernameExists(u)) { cout << "Username sudah ada.\n"; continue; }
                    string pw = inputLine("Password: ");
                    string conf = inputLine("Konfirmasi password: ");
                    if (pw != conf) { cout << "Konfirmasi tidak sama.\n"; continue; }
                    string role = inputLine("Role (admin/kasir): ");
                    if (role != "admin" && role != "kasir") { cout << "Role harus 'admin' atau 'kasir'.\n"; continue; }
                    cout << "\nRingkasan pengguna baru:\n";
                    cout << " - Username: " << u << "\n";
                    cout << " - Role: " << role << "\n";
                    if (!confirmAction("Konfirmasi buat pengguna?")) { cout << "Pembuatan pengguna dibatalkan.\n"; continue; }
                    if (registerUser(u, pw, role)) cout << "Pengguna baru dibuat.\n";
                    else cout << "Gagal membuat pengguna.\n";
                } else if (choice == "6") {
                    productMenu();
                } else if (choice == "7") {
                    categoryMenu();
                } else if (choice == "8") {
                    promoMenu();
                } else if (choice == "9") {
                    listTransactions();
                } else if (choice == "10") {
                    shiftMenu();
                } else if (choice == "11") {
                    financialDashboard();
                } else {
                    cout << "Pilihan tidak dikenali.\n";
                }
            }
        }
        void tambahProduk() {
            addProduct();
        }
        void editProduk() {
            editProduct();
        }
        void hapusProduk() {
            deleteProduct();
        }
        void kelolaKategori() {
            while (true) {
                cout << "\n--- Menu Kategori ---\n";
                cout << "1. Tambah kategori\n";
                cout << "2. Lihat kategori\n";
                cout << "3. Kembali\n";
                string c = inputLine("Pilih: ");
                if (c == "1") addCategory();
                else if (c == "2") listCategories();
                else if (c == "3") break;
                else cout << "Pilihan tidak dikenali.\n";
            }
        }
        void kelolaPromo() {
            while (true) {
                cout << "\n--- Menu Promo ---\n";
                cout << "1. Tambah promo\n";
                cout << "2. Edit promo\n";
                cout << "3. Hapus promo\n";
                cout << "4. Lihat promo\n";
                cout << "5. Kembali\n";
                string c = inputLine("Pilih: ");
                if (c == "1") addPromo();
                else if (c == "2") editPromo();
                else if (c == "3") deletePromo();
                else if (c == "4") listPromos();
                else if (c == "5") break;
                else cout << "Pilihan tidak dikenali.\n";
            }
        }
        void kelolaShift() {
            while (true) {
                cout << "\n--- Menu Jadwal Shift ---\n";
                cout << "1. Tambah jadwal shift\n";
                cout << "2. Lihat jadwal shift\n";
                cout << "3. Ubah jadwal shift\n";
                cout << "4. Hapus jadwal shift\n";
                cout << "5. Kembali\n";
                string c = inputLine("Pilih: ");
                if (c == "1") addShift();
                else if (c == "2") listShifts();
                else if (c == "3") editShift();
                else if (c == "4") deleteShift();
                else if (c == "5") break;
                else cout << "Pilihan tidak dikenali.\n";
            }
        }
        void lihatLaporan() {
            financialDashboard();
        }
        void buatPengguna() {
            string u = inputLine("Pilih username: ");
            if (u.empty()) { cout << "Username tidak boleh kosong.\n"; return; }
            if (usernameExists(u)) { cout << "Username sudah ada.\n"; return; }
            string pw = inputLine("Password: ");
            string conf = inputLine("Konfirmasi password: ");
            if (pw != conf) { cout << "Konfirmasi tidak sama.\n"; return; }
            string role = inputLine("Role (admin/kasir): ");
            if (role != "admin" && role != "kasir") { cout << "Role harus 'admin' atau 'kasir'.\n"; return; }
            cout << "\nRingkasan pengguna baru:\n";
            cout << " - Username: " << u << "\n";
            cout << " - Role: " << role << "\n";
            if (!confirmAction("Konfirmasi buat pengguna?")) { cout << "Pembuatan pengguna dibatalkan.\n"; return; }
            if (registerUser(u, pw, role)) cout << "Pengguna baru dibuat.\n";
            else cout << "Gagal membuat pengguna.\n";
        }
        void hapusPengguna() {
            string target = inputLine("Masukkan username yang akan dihapus: ");
            if (target.empty()) { cout << "Username kosong.\n"; return; }
            if (!usernameExists(target)) { cout << "Username tidak ditemukan.\n"; return; }
            if (deleteUser(target)) cout << "Pengguna dihapus.\n";
            else cout << "Gagal menghapus pengguna.\n";
        }
};

class Produk{
    private:
        string kodeProduk;
        string namaProduk;
        string kategori;
        double harga;
        int stok;
        string tanggalExpired;

    public:
        void tampilProduk() {
            ifstream fin(PRODUCTS_FILE.c_str());
            if (!fin) {
                cout << "Belum ada produk atau file tidak tersedia.\n";
                return;
            }

            string line;
            int idx = 1;

            cout << "\nDaftar produk:\n";

            while (getline(fin, line)) {
                if (line.empty()) continue;

                size_t p1 = line.find('|');
                size_t p2 = line.find('|', p1 + 1);
                size_t p3 = line.find('|', p2 + 1);
                size_t p4 = line.find('|', p3 + 1);
                size_t p5 = line.find('|', p4 + 1);

                if (p1 == string::npos ||
                    p2 == string::npos ||
                    p3 == string::npos ||
                    p4 == string::npos ||
                    p5 == string::npos)
                    continue;

                string kodeProduk = line.substr(0, p1);
                string namaProduk = line.substr(p1 + 1, p2 - p1 - 1);
                string kategori = line.substr(p2 + 1, p3 - p2 - 1);
                string harga = line.substr(p3 + 1, p4 - p3 - 1);
                string stok = line.substr(p4 + 1, p5 - p4 - 1);
                string tanggalExpired = line.substr(p5 + 1);

                cout << idx++ << ". "
                     << kodeProduk << " | "
                     << namaProduk << " | "
                     << kategori << " | Rp"
                     << harga << " | stok: "
                     << stok << " | exp: "
                     << tanggalExpired << endl;
            }

            if (idx == 1) {
                cout << "Belum ada produk.\n";
            }
        }

        void tambahProduk() {
            string kode = inputLine("Kode produk: ");
            if (kode.empty()) { cout << "Kode tidak boleh kosong.\n"; return; }
            if (productCodeExists(kode)) { cout << "Kode sudah ada.\n"; return; }
            string nama = inputLine("Nama produk: ");
            string kategori = inputLine("Kategori: ");
            string harga = inputLine("Harga: ");
            string stok = inputLine("Stok: ");
            string expired = inputLine("Expired (YYYY-MM-DD): ");
            ofstream fout(PRODUCTS_FILE.c_str(), ios::app);
            if (!fout) { cout << "Gagal menulis file produk.\n"; return; }
            fout << kode << '|' << nama << '|' << kategori << '|' << harga << '|' << stok << '|' << expired << '\n';
            cout << "Produk ditambahkan.\n";
        }

        void editProduk() {
            string kode = inputLine("Masukkan kode produk yang akan diedit: ");
            if (!productCodeExists(kode)) { cout << "Kode tidak ditemukan.\n"; return; }
            ifstream fin(PRODUCTS_FILE.c_str());
            string all;
            string line;
            while (getline(fin, line)) {
                if (line.empty()) continue;
                size_t p = line.find('|');
                if (p == string::npos) continue;
                string c = line.substr(0, p);
                if (c == kode) {
                    cout << "Mengedit produk: " << kode << "\n";
                    string nama = inputLine("Nama baru: ");
                    string kategori = inputLine("Kategori baru: ");
                    string harga = inputLine("Harga baru: ");
                    string stok = inputLine("Stok baru: ");
                    string expired = inputLine("Expired baru (YYYY-MM-DD): ");
                    all += kode + '|' + nama + '|' + kategori + '|' + harga + '|' + stok + '|' + expired + '\n';
                } else {
                    all += line + '\n';
                }
            }
            fin.close();
            ofstream fout(PRODUCTS_FILE.c_str(), ios::trunc);
            if (!fout) { cout << "Gagal menulis file produk.\n"; return; }
            fout << all;
            cout << "Produk diperbarui.\n";
        }

        void hapusProduk() {
            string kode = inputLine("Masukkan kode produk yang akan dihapus: ");
            ifstream fin(PRODUCTS_FILE.c_str());
            if (!fin) { cout << "File produk tidak ditemukan.\n"; return; }
            string all;
            string line;
            bool found = false;
            while (getline(fin, line)) {
                if (line.empty()) continue;
                size_t p = line.find('|');
                if (p == string::npos) continue;
                string c = line.substr(0, p);
                if (c == kode) { found = true; continue; }
                all += line + '\n';
            }
            fin.close();
            if (!found) { cout << "Kode tidak ditemukan.\n"; return; }
            ofstream fout(PRODUCTS_FILE.c_str(), ios::trunc);
            if (!fout) { cout << "Gagal menulis file produk.\n"; return; }
            fout << all;
            cout << "Produk dihapus.\n";
        }

        void cariProduk(const string& q) {
            ifstream fin(PRODUCTS_FILE.c_str());
            if (!fin) {
                cout << "File produk tidak ditemukan.\n";
                return;
            }

            string line;
            int ditemukan = 0;

            cout << "\nHasil pencarian untuk '" << q << "':\n";

            while (getline(fin, line)) {
                if (line.empty()) continue;

                size_t p1 = line.find('|');
                size_t p2 = line.find('|', p1 + 1);
                size_t p3 = line.find('|', p2 + 1);
                size_t p4 = line.find('|', p3 + 1);
                size_t p5 = line.find('|', p4 + 1);

                if (p1 == string::npos ||
                    p2 == string::npos ||
                    p3 == string::npos ||
                    p4 == string::npos ||
                    p5 == string::npos)
                    continue;

                string kodeProduk = line.substr(0, p1);
                string namaProduk = line.substr(p1 + 1, p2 - p1 - 1);
                string kategori = line.substr(p2 + 1, p3 - p2 - 1);
                string harga = line.substr(p3 + 1, p4 - p3 - 1);
                string stok = line.substr(p4 + 1, p5 - p4 - 1);
                string tanggalExpired = line.substr(p5 + 1);

                if (kodeProduk.find(q) != string::npos ||
                    namaProduk.find(q) != string::npos ||
                    kategori.find(q) != string::npos) {

                    cout << "- "
                         << kodeProduk << " | "
                         << namaProduk << " | "
                         << kategori << " | Rp"
                         << harga << " | stok:"
                         << stok << " | exp:"
                         << tanggalExpired << endl;

                    ditemukan++;
                }
            }

            if (!ditemukan) {
                cout << "Tidak ditemukan produk yang cocok.\n";
            }
        }

        bool cekExpired() {
            string d = inputLine("Tampilkan produk yang expired sebelum/tanggal (YYYY-MM-DD): ");
            if (d.empty()) { cout << "Tanggal tidak boleh kosong.\n"; return false; }
            ifstream fin(PRODUCTS_FILE.c_str());
            if (!fin) { cout << "File produk tidak ditemukan.\n"; return false; }
            bool found = false;
            string line;
            while (getline(fin, line)) {
                if (line.empty()) continue;
                size_t p1 = line.find('|');
                size_t p2 = line.find('|', p1 + 1);
                size_t p3 = line.find('|', p2 + 1);
                size_t p4 = line.find('|', p3 + 1);
                if (p4 == string::npos) continue;
                size_t p5 = line.find('|', p4 + 1);
                if (p5 == string::npos) continue;
                string kode = line.substr(0, p1);
                string nama = line.substr(p1 + 1, p2 - p1 - 1);
                string expired = line.substr(p5 + 1);
                if (expired <= d) {
                    cout << kode << " | " << nama << " | exp:" << expired << "\n";
                    found = true;
                }
            }
            fin.close();
            return found;
        }

        void tampilProdukExpired() {
            cekExpired();
        }
};

class Member{
    private:
        string idMember;
        string username;
        string namaMember;
        string nomorHP;
        int poinMember;
    public:
            void daftarMember(){
                string username = inputLine("Username member: ");
                if (username.empty()) {
                cout << "Username kosong.\n";
                return;
    }

                if (memberExists(username)) {
                 cout << "Member sudah terdaftar.\n";
                return;
        }

                string namaMember = inputLine("Nama lengkap: ");
                string nomorHP = inputLine("No. telp: ");

                ofstream fout(MEMBERS_FILE.c_str(), ios::app);
                if (!fout) {
                    cout << "Gagal menulis file member.\n";
                return;
            }

                fout << username << '|'
                    << namaMember << '|'
                    << nomorHP << '\n';

                cout << "Member terdaftar.\n";
}
             void tampilMember(){
                ifstream fin(MEMBERS_FILE.c_str());
                if (!fin) {
                cout << "Belum ada member.\n";
                return;
    }

                string line;
                int idx = 1;

                cout << "\nDaftar member:\n";

                while (getline(fin, line)) {
                    if (line.empty()) continue;
                    size_t p1 = line.find('|');
                    size_t p2 = line.find('|', p1 + 1);

                    if (p1 == string::npos || p2 == string::npos) continue;

                    string username = line.substr(0, p1);
                    string namaMember = line.substr(p1 + 1, p2 - p1 - 1);
                    string nomorHP = line.substr(p2 + 1);

                    cout << idx++ << ". "
                    << username << " | "
                    << namaMember << " | "
                    << nomorHP << endl;
    }
}

            void simpanRiwayatBelanja(const string& username){
                ifstream fin(MEMBER_HISTORY_FILE.c_str());
                if (!fin){
                cout << "Belum ada riwayat.\n";
                return;
    }

                string line;

                cout << "\nRiwayat untuk " << username << ":\n";

                while(getline(fin, line)){
                    if(line.empty()) continue;

                    size_t p1 = line.find('|');
                    if(p1 == string::npos) continue;

                    string usernameFile = line.substr(0, p1);

                    if(usernameFile != username) continue;

                    size_t p2 = line.find('|', p1 + 1);
                    size_t p3 = line.find('|', p2 + 1);
                    size_t p4 = line.find('|', p3 + 1);

                    string idTransaksi = line.substr(p1 + 1, p2 - p1 - 1);
                    string tanggal = line.substr(p2 + 1, p3 - p2 - 1);
                    string totalBelanja = line.substr(p3 + 1);

                    cout << "Transaksi "
                    << idTransaksi
                    << " | "
                    << tanggal
                    << " | Rp"
                    << totalBelanja
                    << endl;
    }
}

};

class Kasir{
    private:
        int idKasir;
        string namaKasir;
        string username;
        string password;
    public:
        bool login() {
            string u = inputLine("Username: ");
            string p = inputLine("Password: ");
            string role;
            if (validateCredentials(u, p, role) && role == "kasir") {
                username = u;
                password = p;
                return true;
            }
            return false;
        }
        void logout() {
            cout << "Logout berhasil.\n";
        }
        void dashboardKasir(const string& user) {
            Member member;
            Produk produk;
            while (true) {
                cout << "\n--- Dashboard Kasir ---\n";
                cout << "Selamat datang, " << user << " (kasir)\n";
                cout << "1. Logout\n";
                cout << "2. Ganti password\n";
                cout << "3. Keranjang Belanja\n";
                cout << "4. Cari produk\n";
                cout << "5. Membership\n";
                cout << "6. Riwayat Transaksi\n";
                cout << "Pilih: ";
                string choice;
                getline(cin, choice);
                if (choice == "1") break;
                else if (choice == "2") {
                    string np = inputLine("Password baru: ");
                    string nc = inputLine("Konfirmasi password baru: ");
                    if (np == nc) {
                        if (changePassword(user, np)) cout << "Password berhasil diubah.\n";
                        else cout << "Gagal mengubah password.\n";
                    } else cout << "Konfirmasi tidak sama.\n";
                } else if (choice == "3") {
                    cartMenu();
                } else if (choice == "4") {
                    string q = inputLine("Masukkan kata kunci (nama/kode/kategori): ");
                    if (q.empty()) cout << "Kata kunci kosong.\n";
                    else produk.cariProduk(q);
                } else if (choice == "5") {
                    while (true) {
                        cout << "\n-- Menu Membership --\n";
                        cout << "1. Registrasi member\n";
                        cout << "2. Lihat member\n";
                        cout << "3. Lihat riwayat member\n";
                        cout << "4. Kembali\n";
                        string m = inputLine("Pilih: ");
                        if (m == "1") member.daftarMember();
                        else if (m == "2") member.tampilMember();
                        else if (m == "3") {
                            string mem = inputLine("Masukkan username member: ");
                            if (mem.empty()) cout << "Username kosong.\n";
                            else member.simpanRiwayatBelanja(mem);
                        } else if (m == "4") break;
                        else cout << "Pilihan tidak dikenali.\n";
                    }
                } else if (choice == "6") {
                    listTransactions();
                } else {
                    cout << "Pilihan tidak dikenali.\n";
                }
            }
        }
};

class Promo{
    private:
        string kodePromo;
        double persenDiskon;
        string tanggalMulai;
        string tanggalSelesai;
    public:
        void tambahPromo() {
            addPromo();
        }
        bool cekPromo() {
            string code = inputLine("Kode promo: ");
            string date = inputLine("Tanggal (YYYY-MM-DD): ");
            string m = inputLine("Member? (y/n): ");
            bool isMember = (m == "y" || m == "Y");
            bool valid = false;
            string outCode;
            double pct = applyPromo(code, date, isMember, valid, outCode);
            if (valid) cout << "Promo " << outCode << " berlaku: " << pct << "%\n";
            else cout << "Promo tidak valid atau tidak dapat digunakan.\n";
            return valid;
        }
        double hitungDiskon(double totalBelanja) {
            string code = inputLine("Kode promo: ");
            string date = inputLine("Tanggal (YYYY-MM-DD): ");
            string m = inputLine("Member? (y/n): ");
            bool isMember = (m == "y" || m == "Y");
            bool valid = false; string outCode;
            double pct = applyPromo(code, date, isMember, valid, outCode);
            if (!valid) return 0.0;
            return totalBelanja * (pct / 100.0);
        }

};


class Keranjang{
public:
   
    bool tambahBarang(const string& code, int qty) {
        if (qty <= 0) return false;
        if (!productCodeExists(code)) return false;

        ifstream fin(PRODUCTS_FILE.c_str());
        if (!fin) return false;

        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;

            size_t p1 = line.find('|');
            if (p1 == string::npos) continue;

            string kode = line.substr(0, p1);
            if (kode != code) continue;

            size_t p2 = line.find('|', p1 + 1);
            size_t p3 = line.find('|', p2 + 1);
            size_t p4 = line.find('|', p3 + 1);
            size_t p5 = line.find('|', p4 + 1);

            if (p2 == string::npos || p3 == string::npos ||
                p4 == string::npos || p5 == string::npos) continue;

            string nama = line.substr(p1 + 1, p2 - p1 - 1);
            string kategori = line.substr(p2 + 1, p3 - p2 - 1);
            string harga = line.substr(p3 + 1, p4 - p3 - 1);
            string stok = line.substr(p4 + 1, p5 - p4 - 1);
            string expired = line.substr(p5 + 1);

            double price = 0.0;
            try {
                price = stod(harga);
            } catch (...) {
                price = 0.0;
            }

            // cek apakah sudah ada di cart
            for (int i = 0; i < cartCount; ++i) {
                if (cartCode[i] == code) {
                    cartQty[i] += qty;
                    cartSubtotal[i] = cartQty[i] * cartPrice[i];
                    return true;
                }
            }

            if (cartCount >= MAX_CART) return false;

            cartCode[cartCount] = code;
            cartName[cartCount] = nama;
            cartPrice[cartCount] = price;   // FIX DI SINI
            cartQty[cartCount] = qty;
            cartSubtotal[cartCount] = qty * price;

            cartCount++;
            return true;
        }

        return false;
    }

  bool hapusBarang(int idx) {
    if (idx < 1 || idx > cartCount) return false;

    int i = idx - 1;

    for (int j = i; j < cartCount - 1; ++j) {
        cartCode[j] = cartCode[j + 1];
        cartName[j] = cartName[j + 1];
        cartPrice[j] = cartPrice[j + 1];
        cartQty[j] = cartQty[j + 1];
        cartSubtotal[j] = cartSubtotal[j + 1];
    }

    cartCount--;
    return true;
}
   bool ubahJumlah(int idx, int qty) {
    if (idx < 1 || idx > cartCount)
        return false;

    if (qty <= 0)
        return false;

    int i = idx - 1;

    double price = cartPrice[i];

    cartQty[i] = qty;
    cartSubtotal[i] = qty * price;

    return true;
}
double hitungSubtotal() {
    double total = 0.0;

    for (int i = 0; i < cartCount; i++) {
        total += cartSubtotal[i];
    }

    return total;
}

void tampilKeranjang() {

    if (cartCount == 0) {
        cout << "Keranjang kosong.\n";
        return;
    }

    cout << "\n--- Isi Keranjang ---\n";

    for (int i = 0; i < cartCount; i++) {

        cout << i + 1 << ". "
             << cartCode[i] << " | "
             << cartName[i] << " | Rp"
             << cartPrice[i]
             << " | qty: "
             << cartQty[i]
             << " | subtotal: "
             << cartSubtotal[i]
             << endl;
    }

    cout << "Total sementara: Rp"
         << hitungSubtotal()
         << endl;
}

void kosongkanKeranjang() {
    for (int i = 0; i < MAX_CART; ++i) {
        cartCode[i].clear();
        cartName[i].clear();
        cartPrice[i] = 0.0;
        cartQty[i] = 0;
        cartSubtotal[i] = 0.0;
    }

    cartCount = 0;
}
};

class Transaksi{
    private:
        string idTransaksi;
        string tanggalTransaksi;
        string kodeProduk;
        int jumlahBeli;
        double totalBelanja;
        string idMember;
    public:
        void simpanTransaksi(
    const string& date,
    double total,
    const string& items,
    const string& member,
    double paid,
    double change,
    double discount,
    const string& promoCode)
{
    int id = 1;

    ifstream fin(TRANSACTIONS_FILE.c_str());
    string line;

    while (fin && getline(fin, line)) {
        if (!line.empty())
            id++;
    }
    fin.close();

    ofstream fout(TRANSACTIONS_FILE.c_str(), ios::app);

    if (!fout)
        return;

    double finalAmount = total - discount;

    fout << id << '|'
         << date << '|'
         << (int)total << '|'
         << (int)discount << '|'
         << (int)finalAmount << '|'
         << (int)paid << '|'
         << (int)change << '|'
         << promoCode << '|'
         << member << '|'
         << items
         << endl;

    if (!member.empty()) {
        ofstream fh(MEMBER_HISTORY_FILE.c_str(), ios::app);

        if (fh) {
            fh << member << '|'
               << id << '|'
               << date << '|'
               << (int)finalAmount
               << endl;
        }
    }
}
        void prosesTransaksi() {
            string curDate = inputLine("Tanggal transaksi (YYYY-MM-DD): ");
            if (checkoutCart(curDate)) cout << "Transaksi tersimpan.\n";
            else cout << "Checkout gagal.\n";
        }
        double hitungTotal() {
            double total = 0.0;
            for (int i = 0; i < cartCount; ++i) total += cartSubtotal[i];
            return total;
        }
        
};

class Pembayaran{
private:
    double totalBelanja;
    double uangBayar;
    double kembalian;
    bool statusPembayaran;

public:

    void setTotal(double total){
        totalBelanja = total;
    }

    bool validasiPembayaran(){

        while (true){
            string bayar = inputLine("Masukkan jumlah tunai yang dibayar (angka): ");

            try{
                uangBayar = stod(bayar);
            }
            catch(...){
                uangBayar = 0.0;
            }

            if(uangBayar < totalBelanja){
                cout << "Jumlah tidak cukup. Masukkan lagi.\n";
            }
            else{
                break;
            }
        }

        statusPembayaran = true;
        return statusPembayaran;
    }

    double hitungKembalian(){

        kembalian = uangBayar - totalBelanja;
        return kembalian;
    }

    double getUangBayar(){
        return uangBayar;
    }

};

class Struk{
    private:
    string idTransaksi;
    string nomorStruk;
    string namaToko;
    string alamatToko;
    string nomorTelepon;
    string namaKasir;
    public:
        void cetakKeFile(int id){
    ifstream fin(TRANSACTIONS_FILE.c_str()); if (!fin) { cout<<"File transaksi tidak ditemuk    an.\n"; return; }
    string line; bool found=false; string out;
    while (getline(fin,line)) {
        if (line.empty()) continue;
        size_t p1=line.find('|'); if (p1==string::npos) continue;
        int iid=0; for (size_t i=0;i<p1;++i) if (line[i]>='0'&&line[i]<='9') iid = iid*10 + (line[i]-'0');
        if (iid!=id) continue;
        found=true;
        // reuse viewTransactionDetails parsing quickly
        size_t p2=line.find('|',p1+1); size_t p3=line.find('|',p2+1); size_t p4=line.find('|',p3+1); size_t p5=line.find('|',p4+1); size_t p6=line.find('|',p5+1); size_t p7=line.find('|',p6+1); size_t p8=line.find('|',p7+1); size_t p9=line.find('|',p8+1);
        string date = (p2==string::npos)?string(""):line.substr(p1+1,p2-p1-1);
        string total = (p3==string::npos)?string(""):line.substr(p2+1,p3-p2-1);
        string discount = (p4==string::npos)?string(""):line.substr(p3+1,p4-p3-1);
        string finalAmt = (p5==string::npos)?string(""):line.substr(p4+1,p5-p4-1);
        string paid = (p6==string::npos)?string(""):line.substr(p5+1,p6-p5-1);
        string change = (p7==string::npos)?string(""):line.substr(p6+1,p7-p6-1);
        string promo = (p8==string::npos)?string(""):line.substr(p7+1,p8-p7-1);
        string member = (p9==string::npos)?string(""):line.substr(p8+1,p9-p8-1);
        string items = (p9==string::npos)?string(""):line.substr(p9+1);
        // build out
        out += "Toko Kosmetik - TPalPro\n";
        out += "Alamat: Jl. Contoh No.1\n";
        out += string("Tanggal: ") + date + "\n";
        out += string("Transaksi ID: ") + to_string(id) + "\n";
        out += string("Member: ") + member + "\n";
        out += "Items:\n";
        size_t pos=0; while (pos < items.size()) {
            size_t comma = items.find(',', pos);
            string chunk = (comma==string::npos)?items.substr(pos):items.substr(pos, comma-pos);
            size_t a = chunk.find(':'); size_t b = (a==string::npos)?string::npos:chunk.find(':', a+1);
            string code = (a==string::npos)?chunk:chunk.substr(0,a);
            string qty = (a==string::npos||b==string::npos)?string(""):chunk.substr(a+1,b-a-1);
            string price = (b==string::npos)?string(""):chunk.substr(b+1);
            out += string(" - ") + code + " x" + qty + " @Rp" + price + "\n";
            if (comma==string::npos) break; pos = comma+1;
        }
        out += string("Subtotal: Rp") + total + "\n";
        out += string("Diskon: Rp") + discount + "\n";
        out += string("Total akhir: Rp") + finalAmt + "\n";
        out += string("Dibayar: Rp") + paid + " | Kembali: Rp" + change + "\n";
        out += string("Promo: ") + promo + "\n";
        out += "--- Terima Kasih ---\n";
        break;
    }
    fin.close();
    if (!found) { cout<<"Transaksi tidak ditemukan.\n"; return; }
    string fname = string("receipt_") + to_string(id) + string(".txt");
    ofstream fout(fname.c_str()); if (!fout) { cout<<"Gagal membuat file struk.\n"; return; }
    fout << out; fout.close();
    cout<<"Struk disimpan sebagai "<<fname<<"\n";
}
        void cetakStruk() {
            string sid = inputLine("Masukkan ID transaksi untuk cetak struk: ");
            int id = 0; for (size_t i = 0; i < sid.size(); ++i) if (sid[i] >= '0' && sid[i] <= '9') id = id * 10 + (sid[i] - '0');
            if (id > 0) cetakKeFile(id);
            else cout << "ID tidak valid.\n";
        }

};

class Shift{
    private:
        int idPegawai;
        string namaPegawai;
        string tanggalShift;
        string jenisShift;
    public:
        void tambahShift() { ::addShift(); }
        void hapusShift() { ::deleteShift(); }
        void editShift() { ::editShift(); }
        void tampilShift() { ::listShifts(); }

};

class Presensi{
    private:
        int idPegawai;
        string namaPegawai;
        string tanggalPresensi;
        bool statusHadir;
    public:
        void inputPresensi() {
            string user = inputLine("Username pegawai: ");
            if (user.empty()) { cout << "Username kosong.\n"; return; }
            recordAttendance(user);
        }
        void tampilPresensi() {
            string user = inputLine("Username (kosong untuk semua): ");
            if (user.empty()) listAttendance();
            else listAttendanceForEmployee(user);
        }

};

class LaporanPenjualan{
    private:
        string tanggalLaporan;
        string periodeLaporan;
        int totalTransaksi;
        double totalPendapatan;
    public:
        void hitungPenjualanHarian() { dailySalesReport(); }
        void hitungPenjualanMingguan() { monthlySalesReport(); }
        void hitungPenjualanBulanan() { monthlySalesReport(); }
        void hitungPenjualanTahunan() { monthlySalesReport(); }
        void tampilLaporan() { monthlySalesReport(); }
        void simpanLaporan() { monthlySalesReport(); }

};

class AnalisisProduk{
    private:
        string kodeProduk;
        string namaProduk;
        int jumlahTerjual;
        int stokProduk;
    public:
        void hitungBarangTerlaris() { bestSellingAnalysis(); }
        void rankingProduk() { bestSellingAnalysis(); }
        bool rekomendasiRestock() { profitMarginAnalysis(); return false; }

};

Keranjang krj;

void kasirDashboard(const string& username) {
    Member member;
    Produk produk;
    while (true) {
        cout << "\n--- Dashboard Kasir ---\n";
        cout << "Selamat datang, " << username << " (kasir)\n";
        cout << "1. Logout\n";
        cout << "2. Ganti password\n";
        cout << "3. Keranjang Belanja\n";
        cout << "4. Cari produk\n";
        cout << "5. Membership\n";
        cout << "6. Riwayat Transaksi\n";
        cout << "Pilih: ";
        string choice;
        getline(cin, choice);
        if (choice == "1") break;
        else if (choice == "2") {
            string np = inputLine("Password baru: ");
            string nc = inputLine("Konfirmasi password baru: ");
            if (np == nc) {
                if (changePassword(username, np)) cout << "Password berhasil diubah.\n";
                else cout << "Gagal mengubah password.\n";
            } else cout << "Konfirmasi tidak sama.\n";
        } else if (choice == "3") {
            cartMenu();
        } else if (choice == "4") {
            string q = inputLine("Masukkan kata kunci (nama/kode/kategori): ");
            if (q.empty()) cout << "Kata kunci kosong.\n";
            else produk.cariProduk(q);
        } else if (choice == "5") {
            while (true) {
                cout << "\n-- Menu Membership --\n";
                cout << "1. Registrasi member\n";
                cout << "2. Lihat member\n";
                cout << "3. Lihat riwayat member\n";
                cout << "4. Kembali\n";
                string m = inputLine("Pilih: ");
                if (m == "1") member.daftarMember();
                else if (m == "2") member.tampilMember();
                else if (m == "3") {
                    string mem = inputLine("Masukkan username member: ");
                    if (mem.empty()) cout << "Username kosong.\n";
                    else member.simpanRiwayatBelanja(mem);
                } else if (m == "4") break;
                else cout << "Pilihan tidak dikenali.\n";
            }
        } else if (choice == "6") {
            listTransactions();
        } else if (choice == "7") {
            financialDashboard();
        } else {
            cout << "Pilihan tidak dikenali.\n";
        }
    }
}

// --- Helper functions for products, cart, transactions, membership ---

bool findProductByCode(const string& code, string& nama, string& kategori, string& harga, string& stok, string& expired) {
    ifstream fin(PRODUCTS_FILE.c_str());
    if (!fin) return false;
    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1+1);
        size_t p3 = line.find('|', p2+1);
        size_t p4 = line.find('|', p3+1);
        size_t p5 = line.find('|', p4+1);
        if (p1==string::npos||p2==string::npos||p3==string::npos||p4==string::npos||p5==string::npos) continue;
        string c = line.substr(0,p1);
        if (c == code) {
            nama = line.substr(p1+1, p2-p1-1);
            kategori = line.substr(p2+1, p3-p2-1);
            harga = line.substr(p3+1, p4-p3-1);
            stok = line.substr(p4+1, p5-p4-1);
            expired = line.substr(p5+1);
            fin.close();
            return true;
        }
    }
    return false;
}





bool memberExists(const string& uname) {
    ifstream fin(MEMBERS_FILE.c_str()); if (!fin) return false;
    string line; while (getline(fin,line)) { if (line.empty()) continue; size_t p=line.find('|'); if (p==string::npos) continue; string u=line.substr(0,p); if (u==uname) return true; } return false;
}









int main() {
    ensureDefaultCategories();
    ensureDefaultData();
    while (true) {
        cout << "\n=== Toko Kosmetik - Auth Menu ===\n";
        cout << "1. Login\n";
        cout << "2. Registrasi\n";
        cout << "3. Lupa Password\n";
        cout << "4. Keluar\n";
        cout << "Pilih: ";
        string choice;
        getline(cin, choice);

        if (choice == "1") {
            string username = inputLine("Username: ");
            string password = inputLine("Password: ");
            string role;
            if (validateCredentials(username, password, role)) {
                cout << "Login berhasil. Role: " << role << "\n";
                if (role == "admin") adminDashboard(username);
                else kasirDashboard(username);
            } else {
                cout << "Username atau password salah.\n";
            }
        } else if (choice == "2") {
            string username = inputLine("Pilih username: ");
            if (username.empty()) { cout << "Username tidak boleh kosong.\n"; continue; }
            if (usernameExists(username)) { cout << "Username sudah ada.\n"; continue; }
            string password = inputLine("Password: ");
            string confirm = inputLine("Konfirmasi password: ");
            if (password != confirm) { cout << "Konfirmasi tidak sama.\n"; continue; }
            string role = inputLine("Role (admin/kasir): ");
            if (role != "admin" && role != "kasir") { cout << "Role harus 'admin' atau 'kasir'.\n"; continue; }
            cout << "\nRingkasan akun:\n";
            cout << " - Username: " << username << "\n";
            cout << " - Role: " << role << "\n";
            if (!confirmAction("Konfirmasi registrasi?")) { cout << "Registrasi dibatalkan.\n"; continue; }
            if (registerUser(username, password, role)) cout << "Registrasi berhasil.\n";
            else cout << "Gagal registrasi (tidak dapat menulis file).\n";
        } else if (choice == "3") {
            string username = inputLine("Masukkan username untuk reset password: ");
            if (!usernameExists(username)) { cout << "Username tidak ditemukan.\n"; continue; }
            string np = inputLine("Password baru: ");
            string nc = inputLine("Konfirmasi password baru: ");
            if (np != nc) { cout << "Konfirmasi tidak sama.\n"; continue; }
            cout << "Anda akan mengganti password akun '" << username << "'.\n";
            if (!confirmAction("Lanjut ganti password?")) { cout << "Perubahan password dibatalkan.\n"; continue; }
            if (changePassword(username, np)) cout << "Password berhasil diubah.\n";
            else cout << "Gagal mengubah password.\n";
        } else if (choice == "4") {
            cout << "Keluar...\n";
            break;
        } else {
            cout << "Pilihan tidak dikenali.\n";
        }
    }
    return 0;
}

// --- Product & Category management ---

bool productCodeExists(const string& code) {
    ifstream fin(PRODUCTS_FILE.c_str());
    if (!fin) return false;
    string line;
    while (getline(fin, line)) {
        size_t p = line.find('|');
        if (p == string::npos) continue;
        string c = line.substr(0, p);
        if (c == code) return true;
    }
    return false;
}



void addProduct() {
    string kode = inputLine("Kode produk: ");
    if (kode.empty()) { cout << "Kode tidak boleh kosong.\n"; return; }
    if (productCodeExists(kode)) { cout << "Kode sudah ada.\n"; return; }
    string nama = inputLine("Nama produk: ");
    string kategori = inputLine("Kategori: ");
    string harga = inputLine("Harga: ");
    string stok = inputLine("Stok: ");
    string expired = inputLine("Expired (YYYY-MM-DD): ");
    ofstream fout(PRODUCTS_FILE.c_str(), ios::app);
    if (!fout) { cout << "Gagal menulis file produk.\n"; return; }
    fout << kode << '|' << nama << '|' << kategori << '|' << harga << '|' << stok << '|' << expired << '\n';
    cout << "Produk ditambahkan.\n";
}

void editProduct() {
    string kode = inputLine("Masukkan kode produk yang akan diedit: ");
    if (!productCodeExists(kode)) { cout << "Kode tidak ditemukan.\n"; return; }
    ifstream fin(PRODUCTS_FILE.c_str());
    string all;
    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        size_t p = line.find('|');
        if (p==string::npos) continue;
        string c = line.substr(0,p);
        if (c == kode) {
            cout << "Mengedit produk: " << kode << "\n";
            string nama = inputLine("Nama baru: ");
            string kategori = inputLine("Kategori baru: ");
            string harga = inputLine("Harga baru: ");
            string stok = inputLine("Stok baru: ");
            string expired = inputLine("Expired baru (YYYY-MM-DD): ");
            all += kode + '|' + nama + '|' + kategori + '|' + harga + '|' + stok + '|' + expired + '\n';
        } else {
            all += line + '\n';
        }
    }
    fin.close();
    ofstream fout(PRODUCTS_FILE.c_str(), ios::trunc);
    if (!fout) { cout << "Gagal menulis file produk.\n"; return; }
    fout << all;
    cout << "Produk diperbarui.\n";
}

void deleteProduct() {
    string kode = inputLine("Masukkan kode produk yang akan dihapus: ");
    ifstream fin(PRODUCTS_FILE.c_str());
    if (!fin) { cout << "File produk tidak ditemukan.\n"; return; }
    string all;
    string line;
    bool found = false;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        size_t p = line.find('|');
        if (p==string::npos) continue;
        string c = line.substr(0,p);
        if (c == kode) { found = true; continue; }
        all += line + '\n';
    }
    fin.close();
    if (!found) { cout << "Kode tidak ditemukan.\n"; return; }
    ofstream fout(PRODUCTS_FILE.c_str(), ios::trunc);
    if (!fout) { cout << "Gagal menulis file produk.\n"; return; }
    fout << all;
    cout << "Produk dihapus.\n";
}

bool categoryExists(const string& cat) {
    ifstream fin(CATEGORIES_FILE.c_str());
    if (!fin) return false;
    string line;
    while (getline(fin, line)) {
        if (line == cat) return true;
    }
    return false;
}

void listCategories() {
    ifstream fin(CATEGORIES_FILE.c_str());
    if (!fin) { cout << "Belum ada kategori.\n"; return; }
    string line;
    int idx = 1;
    cout << "\nDaftar kategori:\n";
    while (getline(fin, line)) {
        if (line.empty()) continue;
        cout << idx++ << ". " << line << "\n";
    }
    if (idx==1) cout << "Belum ada kategori.\n";
}

void addCategory() {
    string cat = inputLine("Nama kategori: ");
    if (cat.empty()) { cout << "Kategori tidak boleh kosong.\n"; return; }
    if (categoryExists(cat)) { cout << "Kategori sudah ada.\n"; return; }
    ofstream fout(CATEGORIES_FILE.c_str(), ios::app);
    if (!fout) { cout << "Gagal menulis file kategori.\n"; return; }
    fout << cat << '\n';
    cout << "Kategori ditambahkan.\n";
}

void ensureDefaultCategories() {
    ifstream fin(CATEGORIES_FILE.c_str());
    bool need = true;
    if (fin) {
        string any;
        if (getline(fin, any)) need = false;
        fin.close();
    }
    if (need) {
        ofstream fout(CATEGORIES_FILE.c_str(), ios::app);
        if (fout) {
            fout << "Skincare\n";
            fout << "Bibir\n";
            fout << "Mata\n";
            fout << "Wajah\n";
            fout << "Bodycare\n";
        }
    }
}

void ensureSampleTransactions() {
    bool needTransactions = true;
    ifstream tfin(TRANSACTIONS_FILE.c_str());
    if (tfin) {
        string any;
        if (getline(tfin, any)) needTransactions = false;
        tfin.close();
    }

    if (!needTransactions) return;

    ofstream tfout(TRANSACTIONS_FILE.c_str(), ios::app);
    ofstream mhout(MEMBER_HISTORY_FILE.c_str(), ios::app);
    if (!tfout || !mhout) return;

    tfout << "1|2026-06-01|150000|10000|140000|150000|10000|PROMO10|alice|P0001(2),P0003(1)\n";
    tfout << "2|2026-06-02|95000|5000|90000|100000|10000||bob|P0005(1),P0007(1)\n";
    tfout << "3|2026-06-03|180000|0|180000|200000|20000|MEMBER15|alice|P0008(2),P0020(1)\n";
    tfout << "4|2026-06-10|250000|20000|230000|250000|20000||carol|P0002(1),P0006(1),P0011(2)\n";

    mhout << "alice|1|2026-06-01|140000\n";
    mhout << "bob|2|2026-06-02|90000\n";
    mhout << "alice|3|2026-06-03|180000\n";
    tfout.close();
    mhout.close();
}

// Ensure sample static data exists (products, promos, members)
void ensureDefaultData() {
    // Seed products if empty
    bool needProducts = true;
    ifstream pfin(PRODUCTS_FILE.c_str());
    if (pfin) { string any; if (getline(pfin, any)) needProducts = false; pfin.close(); }
    if (needProducts) {
        ofstream pf(PRODUCTS_FILE.c_str(), ios::app);
        if (pf) {
            // several sample products across categories
            pf << "P0001|Lipstick Red|Bibir|75000|25|2028-12-31\n";
            pf << "P0002|Lipstick Pink|Bibir|70000|30|2028-12-31\n";
            pf << "P0003|Mascara Black|Mata|90000|15|2028-12-31\n";
            pf << "P0004|Eyeliner Liquid|Mata|65000|18|2028-12-31\n";
            pf << "P0005|Face Cleanser|Wajah|45000|40|2028-12-31\n";
            pf << "P0006|Moisturizer|Wajah|120000|22|2028-12-31\n";
            pf << "P0007|Body Lotion|Bodycare|60000|35|2028-12-31\n";
            pf << "P0008|Hand Cream|Bodycare|40000|50|2028-12-31\n";
            pf << "P0009|Serum Vitamin C|Skincare|150000|12|2028-12-31\n";
            pf << "P0010|Sunscreen SPF50|Skincare|95000|28|2028-12-31\n";
            pf << "P0011|Blush Peach|Wajah|55000|20|2028-12-31\n";
            pf << "P0012|BB Cream|Wajah|85000|14|2028-12-31\n";
            pf << "P0013|Face Mask Clay|Skincare|30000|60|2028-12-31\n";
            pf << "P0014|Lip Balm|Bibir|20000|80|2028-12-31\n";
            pf << "P0015|Eye Shadow Palette|Mata|180000|10|2028-12-31\n";
            pf << "P0016|Facial Toner|Skincare|50000|45|2028-12-31\n";
            pf << "P0017|Makeup Remover|Wajah|38000|33|2028-12-31\n";
            pf << "P0018|Body Scrub|Bodycare|48000|26|2028-12-31\n";
            pf << "P0019|Concealer|Wajah|67000|19|2028-12-31\n";
            pf << "P0020|Facial Oil|Skincare|140000|9|2028-12-31\n";
            // more entries to bulk up the file
            for (int i = 21; i <= 40; ++i) {
                char code[8]; sprintf(code, "P%04d", i);
                string kode(code);
                string name = string("SampleProd_") + kode;
                string cat = (i%5==0)?"Bodycare":(i%5==1)?"Skincare":(i%5==2)?"Bibir":(i%5==3)?"Mata":"Wajah";
                int price = 20000 + (i*1500);
                int stock = 10 + (i%7)*5;
                string exp = "2028-12-31";
                pf << kode << '|' << name << '|' << cat << '|' << price << '|' << stock << '|' << exp << '\n';
            }
            pf.close();
        }
    }

    // Seed promos if empty
    bool needPromos = true;
    ifstream rfin(PROMOS_FILE.c_str()); if (rfin) { string any; if (getline(rfin, any)) needPromos = false; rfin.close(); }
    if (needPromos) {
        ofstream rf(PROMOS_FILE.c_str(), ios::app);
        if (rf) {
            rf << "PROMO10|Diskon 10% semua produk|10|2026-05-01|2026-12-31|0\n";
            rf << "MEMBER15|Diskon 15% untuk member|15|2026-06-01|2027-06-01|1\n";
            rf << "FLASH20|Diskon flash sale 20%|20|2026-11-25|2026-11-30|0\n";
            rf.close();
        }
    }

    // Seed members if empty
    bool needMembers = true;
    ifstream mfin(MEMBERS_FILE.c_str()); if (mfin) { string any; if (getline(mfin, any)) needMembers = false; mfin.close(); }
    if (needMembers) {
        ofstream mf(MEMBERS_FILE.c_str(), ios::app);
        if (mf) {
            mf << "alice|Alice Wonderland|081234567890\n";
            mf << "bob|Bob Builder|082345678901\n";
            mf << "carol|Carol Singer|083456789012\n";
            mf.close();
        }
    }

    ensureSampleTransactions();
}

void productMenu() {
    Produk produk;
    while (true) {
        cout << "\n--- Menu Produk ---\n";
        cout << "1. Tambah produk\n";
        cout << "2. Lihat produk\n";
        cout << "3. Edit produk\n";
        cout << "4. Hapus produk\n";
        cout << "5. Transaksi (jual produk)\n";
        cout << "6. Tampilkan produk expired/sebelum tanggal\n";
        cout << "7. Kembali\n";
        cout << "Pilih: ";
        string c; getline(cin, c);
        if (c == "1") produk.tambahProduk();
        else if (c == "2") produk.tampilProduk();
        else if (c == "3") produk.editProduk();
        else if (c == "4") produk.hapusProduk();
        else if (c == "5") {
            // transaksi
            // will implement below as processSale
            string curDate = inputLine("Tanggal hari ini (YYYY-MM-DD): ");
            string code = inputLine("Kode produk yang dibeli: ");
            string qtys = inputLine("Jumlah: ");
            int qty = 0; for (size_t i=0;i<qtys.size();++i) if (qtys[i]>='0'&&qtys[i]<='9') qty = qty*10 + (qtys[i]-'0');
            if (qty<=0) { cout << "Jumlah tidak valid.\n"; }
            else {
                // reduce stock
                bool success = false;
                ifstream fin(PRODUCTS_FILE.c_str());
                if (!fin) { cout << "File produk tidak ditemukan.\n"; }
                else {
                    string all;
                    string line;
                    while (getline(fin, line)) {
                        if (line.empty()) continue;
                        size_t p1 = line.find('|');
                        size_t p2 = line.find('|', p1 + 1);
                        size_t p3 = line.find('|', p2 + 1);
                        size_t p4 = line.find('|', p3 + 1);
                        size_t p5 = line.find('|', p4 + 1);
                        if (p1==string::npos||p2==string::npos||p3==string::npos||p4==string::npos||p5==string::npos) { all += line + '\n'; continue; }
                        string kode = line.substr(0,p1);
                        string nama = line.substr(p1+1, p2-p1-1);
                        string kategori = line.substr(p2+1, p3-p2-1);
                        string harga = line.substr(p3+1, p4-p3-1);
                        string stok = line.substr(p4+1, p5-p4-1);
                        string expired = line.substr(p5+1);
                        if (kode == code) {
                            // check expired
                            if (!curDate.empty() && expired <= curDate) {
                                cout << "Produk sudah expired/tidak layak jual.\n";
                                all += line + '\n';
                                continue;
                            }
                            int s = 0; for (size_t i=0;i<stok.size();++i) if (stok[i]>='0'&&stok[i]<='9') s = s*10 + (stok[i]-'0');
                            if (s < qty) {
                                cout << "Stok tidak cukup.\n";
                                all += line + '\n';
                            } else {
                                s -= qty;
                                // notify low stock
                                if (s == 0) cout << "Transaksi berhasil. Stok habis untuk produk " << kode << "\n";
                                else if (s <= 5) cout << "Transaksi berhasil. Peringatan: stok menipis untuk " << kode << ": " << s << "\n";
                                else cout << "Transaksi berhasil. Sisa stok: " << s << "\n";
                                // rebuild line
                                all += kode + '|' + nama + '|' + kategori + '|' + harga + '|' + to_string(s) + '|' + expired + '\n';
                                success = true;
                            }
                        } else {
                            all += line + '\n';
                        }
                    }
                    fin.close();
                    ofstream fout(PRODUCTS_FILE.c_str(), ios::trunc);
                    if (!fout) cout << "Gagal menyimpan perubahan stok.\n";
                    else fout << all;
                }
            }
        }
        else if (c == "6") {
            string d = inputLine("Tampilkan produk yang expired sebelum/tanggal (YYYY-MM-DD): ");
            if (d.empty()) { cout << "Tanggal tidak boleh kosong.\n"; }
            else {
                ifstream fin(PRODUCTS_FILE.c_str());
                if (!fin) { cout << "File produk tidak ditemukan.\n"; }
                else {
                    cout << "\nProduk expired/sebelum " << d << ":\n";
                    string line;
                    while (getline(fin, line)) {
                        if (line.empty()) continue;
                        size_t p5 = string::npos;
                        // find fifth separator
                        size_t p1 = line.find('|');
                        size_t p2 = (p1==string::npos)?string::npos:line.find('|', p1+1);
                        size_t p3 = (p2==string::npos)?string::npos:line.find('|', p2+1);
                        size_t p4 = (p3==string::npos)?string::npos:line.find('|', p3+1);
                        if (p4!=string::npos) p5 = line.find('|', p4+1);
                        if (p5==string::npos) continue;
                        string kode = line.substr(0,p1);
                        string nama = line.substr(p1+1, p2-p1-1);
                        string expired = line.substr(p5+1);
                        if (expired <= d) cout << kode << " | " << nama << " | exp:" << expired << "\n";
                    }
                    fin.close();
                }
            }
        }
        else if (c == "7") {
            break;
        }
        else cout << "Pilihan tidak dikenali.\n";
    }
}

void categoryMenu() {
    while (true) {
        cout << "\n--- Menu Kategori ---\n";
        cout << "1. Tambah kategori\n";
        cout << "2. Lihat kategori\n";
        cout << "3. Kembali\n";
        cout << "Pilih: ";
        string c; getline(cin, c);
        if (c == "1") addCategory();
        else if (c == "2") listCategories();
        else if (c == "3") break;
        else cout << "Pilihan tidak dikenali.\n";
    }
}





// add product by code and qty; returns true if added/updated








// --- Promo management and transaction persistence ---

bool promoCodeExists(const string& code) {
    ifstream fin(PROMOS_FILE.c_str()); if (!fin) return false;
    string line; while (getline(fin,line)) { if (line.empty()) continue; size_t p=line.find('|'); if (p==string::npos) continue; string c=line.substr(0,p); if (c==code) return true; } return false;
}

void listPromos() {
    ifstream fin(PROMOS_FILE.c_str()); if (!fin) { cout<<"Belum ada promo.\n"; return; }
    string line; int idx=1; cout<<"\nDaftar promo:\n";
    while (getline(fin,line)) {
        if (line.empty()) continue;
        // format: code|desc|percent|start|end|memberOnly
        size_t p1=line.find('|'); size_t p2=line.find('|',p1+1); size_t p3=line.find('|',p2+1); size_t p4=line.find('|',p3+1); size_t p5=line.find('|',p4+1);
        if (p1==string::npos||p2==string::npos||p3==string::npos||p4==string::npos||p5==string::npos) continue;
        string code=line.substr(0,p1); string desc=line.substr(p1+1,p2-p1-1); string pct=line.substr(p2+1,p3-p2-1); string start=line.substr(p3+1,p4-p3-1); string end=line.substr(p4+1,p5-p4-1); string memberOnly=line.substr(p5+1);
        cout<<idx++<<". "<<code<<" | "<<desc<<" | "<<pct<<"% | "<<start<<" - "<<end<<" | memberOnly:"<<memberOnly<<"\n";
    }
}

void addPromo() {
    string code=inputLine("Kode promo: "); if (code.empty()) { cout<<"Kode kosong.\n"; return; }
    if (promoCodeExists(code)) { cout<<"Kode promo sudah ada.\n"; return; }
    string desc=inputLine("Deskripsi promo: ");
    string pcts=inputLine("Persentase diskon (angka, contoh 10 untuk 10%): ");
    string start=inputLine("Mulai (YYYY-MM-DD): ");
    string end=inputLine("Selesai (YYYY-MM-DD): ");
    string m=inputLine("Hanya untuk member? (y/n): "); string memberOnly=(m=="y"||m=="Y")?"1":"0";
    ofstream fout(PROMOS_FILE.c_str(), ios::app); if (!fout) { cout<<"Gagal menulis file promo.\n"; return; }
    fout<<code<<"|"<<desc<<"|"<<pcts<<"|"<<start<<"|"<<end<<"|"<<memberOnly<<"\n";
    cout<<"Promo ditambahkan.\n";
}

void editPromo() {
    string code=inputLine("Kode promo yang diedit: "); if (!promoCodeExists(code)) { cout<<"Kode tidak ditemukan.\n"; return; }
    ifstream fin(PROMOS_FILE.c_str()); if (!fin) { cout<<"File promo tidak ditemukan.\n"; return; }
    string all; string line; while (getline(fin,line)) { if (line.empty()) continue; size_t p=line.find('|'); if (p==string::npos) { all+=line+'\n'; continue; } string c=line.substr(0,p); if (c==code) {
            cout<<"Mengedit promo: "<<code<<"\n";
            string desc=inputLine("Deskripsi baru: "); string pcts=inputLine("Persentase baru: "); string start=inputLine("Mulai baru (YYYY-MM-DD): "); string end=inputLine("Selesai baru (YYYY-MM-DD): "); string m=inputLine("Hanya untuk member? (y/n): "); string memberOnly=(m=="y"||m=="Y")?"1":"0";
            all += code + '|' + desc + '|' + pcts + '|' + start + '|' + end + '|' + memberOnly + '\n';
        } else all += line + '\n'; }
    fin.close(); ofstream fout(PROMOS_FILE.c_str(), ios::trunc); if (!fout) { cout<<"Gagal menulis file promo.\n"; return; } fout<<all; cout<<"Promo diperbarui.\n";
}

void deletePromo() {
    string code=inputLine("Kode promo yang dihapus: "); if (!promoCodeExists(code)) { cout<<"Kode tidak ditemukan.\n"; return; }
    if (!confirmAction(string("Hapus promo '") + code + "' ?")) return;
    ifstream fin(PROMOS_FILE.c_str()); string all; string line; while (getline(fin,line)) { if (line.empty()) continue; size_t p=line.find('|'); if (p==string::npos) { all+=line+'\n'; continue; } string c=line.substr(0,p); if (c==code) continue; all+=line+'\n'; } fin.close(); ofstream fout(PROMOS_FILE.c_str(), ios::trunc); if (!fout) { cout<<"Gagal menulis file promo.\n"; return; } fout<<all; cout<<"Promo dihapus.\n";
}

// applyPromo: returns percent (0.0..100.0). valid set to true when applicable.
double applyPromo(const string& promoCode, const string& date, bool isMember, bool &valid, string &outPromoCode) {
    valid = false; outPromoCode.clear(); if (promoCode.empty()) return 0.0;
    ifstream fin(PROMOS_FILE.c_str()); if (!fin) return 0.0;
    string line; while (getline(fin,line)) {
        if (line.empty()) continue;
        size_t p1=line.find('|'); size_t p2=line.find('|',p1+1); size_t p3=line.find('|',p2+1); size_t p4=line.find('|',p3+1); size_t p5=line.find('|',p4+1);
        if (p1==string::npos||p2==string::npos||p3==string::npos||p4==string::npos||p5==string::npos) continue;
        string code=line.substr(0,p1); string desc=line.substr(p1+1,p2-p1-1); string pcts=line.substr(p2+1,p3-p2-1); string start=line.substr(p3+1,p4-p3-1); string end=line.substr(p4+1,p5-p4-1); string memberOnly=line.substr(p5+1);
        if (code != promoCode) continue;
        // check date range
        if (!start.empty() && date < start) { valid = false; return 0.0; }
        if (!end.empty() && date > end) { valid = false; return 0.0; }
        if (memberOnly == "1" && !isMember) { valid = false; return 0.0; }
        double pct = 0.0; try { pct = stod(pcts); } catch(...) { pct = 0.0; }
        valid = true; outPromoCode = code; return pct;
    }
    return 0.0;
}

void promoMenu() {
    while (true) {
        cout << "\n--- Menu Promo ---\n";
        cout << "1. Tambah promo\n";
        cout << "2. Edit promo\n";
        cout << "3. Hapus promo\n";
        cout << "4. Lihat promo\n";
        cout << "5. Kembali\n";
        string c = inputLine("Pilih: ");
        Promo promo;
        if (c=="1") promo.tambahPromo();
        else if (c=="2") editPromo();
        else if (c=="3") deletePromo();
        else if (c=="4") listPromos();
        else if (c=="5") break;
        else cout<<"Pilihan tidak dikenali.\n";
    }
}

// Unified transaction persistence (detailed)


// Checkout: validate expired and stock, then decrement stock and save transaction
bool isValidDateFormat(const string& date) {
    if (date.size() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;
    for (int i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) continue;
        if (date[i] < '0' || date[i] > '9') return false;
    }
    return true;
}

bool isExpired(const string& expDate, const string& curDate) {
    if (!isValidDateFormat(expDate) || !isValidDateFormat(curDate)) return false;
    return expDate < curDate;
}

bool checkoutCart(const string& curDate) {
    if (cartCount == 0) { cout << "Keranjang kosong.\n"; return false; }
    // read products and validate
    ifstream fin(PRODUCTS_FILE.c_str());
    if (!fin) { cout << "File produk tidak ditemukan.\n"; return false; }
    // read whole file into a single string
    string allProducts;
    string line;
    while (getline(fin, line)) {
        allProducts += line;
        allProducts += '\n';
    }
    fin.close();

    // validate each cart item against allProducts and update stock in-memory
    for (int ci = 0; ci < cartCount; ++ci) {
        string code = cartCode[ci];
        int needQty = cartQty[ci];
        // find the product line that starts with code|
        size_t searchPos = 0;
        size_t foundPos = string::npos;
        while (true) {
            foundPos = allProducts.find(code + '|', searchPos);
            if (foundPos == string::npos) break;
            if (foundPos == 0 || allProducts[foundPos - 1] == '\n') break;
            searchPos = foundPos + 1;
        }
        if (foundPos == string::npos) { cout << "Produk " << code << " tidak ditemukan pada file.\n"; return false; }
        size_t p1 = foundPos;
        size_t p2 = allProducts.find('|', p1 + 1);
        size_t p3 = allProducts.find('|', p2 + 1);
        size_t p4 = allProducts.find('|', p3 + 1);
        size_t p5 = allProducts.find('|', p4 + 1);
        if (p2==string::npos||p3==string::npos||p4==string::npos||p5==string::npos) { cout << "Format file produk rusak.\n"; return false; }
        string stokStr = allProducts.substr(p4 + 1, p5 - (p4 + 1));
        size_t nlpos = allProducts.find('\n', p5 + 1);
        string expired = (nlpos==string::npos) ? allProducts.substr(p5 + 1) : allProducts.substr(p5 + 1, nlpos - (p5 + 1));
        int stokVal = 0; for (size_t i=0;i<stokStr.size();++i) if (stokStr[i]>='0'&&stokStr[i]<='9') stokVal = stokVal*10 + (stokStr[i]-'0');
        if (!curDate.empty() && isExpired(expired, curDate)) { cout << "Produk " << code << " sudah expired/tidak layak jual.\n"; return false; }
        if (stokVal < needQty) { cout << "Stok tidak cukup untuk produk " << code << ". (butuh " << needQty << ", tersedia " << stokVal << ")\n"; return false; }
        // update stock in allProducts string
        int newStock = stokVal - needQty;
        string newStockStr = to_string(newStock);
        allProducts.replace(p4 + 1, p5 - (p4 + 1), newStockStr);
        // continue to next item
    }

    // write updated products back to file
    ofstream fout(PRODUCTS_FILE.c_str(), ios::trunc);
    if (!fout) { cout << "Gagal menyimpan perubahan stok.\n"; return false; }
    fout << allProducts;
    fout.close();

    // compute total and prepare items string
    double total = 0.0;
    string itemsDesc;
    for (int i = 0; i < cartCount; ++i) {
        total += cartSubtotal[i];
        if (!itemsDesc.empty()) itemsDesc += ",";
       itemsDesc += cartCode[i] + ":" + to_string(cartQty[i]) + ":" + to_string(cartPrice[i]);
    }

    // ask for member (optional)
    string member = inputLine("Username member (kosong jika non-member): ");
    bool isMember = false;
    if (!member.empty()) {
        if (memberExists(member)) isMember = true;
        else { cout << "Member tidak ditemukan, akan diproses sebagai non-member.\n"; member.clear(); }
    }

    // ask for promo code (optional)
    string promoIn = inputLine("Kode promo (kosong jika tidak ada): ");
    bool promoValid = false; string appliedPromoCode;
    double promoPct = 0.0;
    if (!promoIn.empty()) {
        promoPct = applyPromo(promoIn, curDate, isMember, promoValid, appliedPromoCode);
        if (!promoValid) { cout << "Promo tidak valid atau tidak dapat digunakan.\n"; promoIn.clear(); appliedPromoCode.clear(); promoPct = 0.0; }
        else cout << "Promo "<<appliedPromoCode<<" diterapkan: "<<promoPct<<"%\n";
    }

    double discountAmt = total * (promoPct/100.0);
    // optional member flat discount (small benefit)
    double memberDiscount = 0.0;
    if (isMember) {
        // give small additional 2% loyalty discount
        memberDiscount = total * 0.02;
        cout << "Diskon member tambahan: 2% -> Rp" << (int)memberDiscount << "\n";
    }
    double totalDiscount = discountAmt + memberDiscount;
    double finalTotal = total - totalDiscount;
    if (finalTotal < 0) finalTotal = 0;

    cout << "Total: Rp" << (int)total << " | Diskon: Rp" << (int)totalDiscount << " | Bayar: Rp" << (int)finalTotal << "\n";

    // payment (tunai)
    Pembayaran pembayaran;

    pembayaran.setTotal(finalTotal);
    pembayaran.validasiPembayaran();

    double paid = pembayaran.getUangBayar();
    double change = pembayaran.hitungKembalian();

    // persist transaction with details to the shared analytics file
    Transaksi transaksi;
transaksi.simpanTransaksi(
    curDate,
    total,
    itemsDesc,
    member,
    paid,
    change,
    totalDiscount,
    appliedPromoCode
);

    // print simple receipt
    cout << "\n--- Struk Pembayaran ---\n";
    cout << "Tanggal: " << curDate << "\n";
    for (int i = 0; i < cartCount; ++i) cout << cartCode[i] << " x" << cartQty[i] << " = Rp" << (int)cartSubtotal[i] << "\n";
    cout << "Subtotal: Rp" << (int)total << "\n";
    cout << "Diskon total: Rp" << (int)totalDiscount << "\n";
    cout << "Total bayar: Rp" << (int)finalTotal << "\n";
    cout << "Tunai: Rp" << (int)paid << " | Kembali: Rp" << (int)change << "\n";
    if (!appliedPromoCode.empty()) cout << "Promo: " << appliedPromoCode << "\n";
    if (!member.empty()) cout << "Member: " << member << "\n";
    cout << "--- Terima kasih ---\n\n";

    krj.kosongkanKeranjang();
    return true;
}

// Transaction history and receipt printing
void listTransactions() {
    ifstream fin(TRANSACTIONS_FILE.c_str()); if (!fin) { cout<<"Belum ada transaksi.\n"; return; }
    string line; cout<<"\nDaftar transaksi:\n";
    while (getline(fin,line)) {
        if (line.empty()) continue;
        // id|date|total|discount|final|paid|change|promo|member|items
        size_t p1=line.find('|'); if (p1==string::npos) continue;
        size_t p2=line.find('|',p1+1); if (p2==string::npos) continue;
        size_t p3=line.find('|',p2+1); if (p3==string::npos) continue;
        size_t p4=line.find('|',p3+1); if (p4==string::npos) continue;
        size_t p5=line.find('|',p4+1); if (p5==string::npos) continue;
        size_t p6=line.find('|',p5+1); if (p6==string::npos) continue;
        size_t p7=line.find('|',p6+1); if (p7==string::npos) continue;
        size_t p8=line.find('|',p7+1); if (p8==string::npos) continue;
        size_t p9=line.find('|',p8+1);
        string id = line.substr(0,p1);
        string date = line.substr(p1+1,p2-p1-1);
        string finalAmt = line.substr(p4+1,p5-p4-1);
        string member = (p8==string::npos)?string(""):line.substr(p7+1,p8-p7-1);
        cout<<"- ID:"<<id<<" | "<<date<<" | Rp"<<finalAmt<<" | Member:"<<member<<"\n";
    }
    fin.close();
    // allow view detail
    string r = inputLine("Lihat detail transaksi ID (masukkan nomor) atau kosong untuk kembali: ");
    if (r.empty()) return;
    int id = 0; for (size_t i=0;i<r.size();++i) if (r[i]>='0'&&r[i]<='9') id = id*10 + (r[i]-'0');
    if (id>0) viewTransactionDetails(id);
}

void viewTransactionDetails(int id) {
    ifstream fin(TRANSACTIONS_FILE.c_str()); if (!fin) { cout<<"Belum ada transaksi.\n"; return; }
    string line; bool found=false;
    while (getline(fin,line)) {
        if (line.empty()) continue;
        size_t p1=line.find('|'); if (p1==string::npos) continue;
        string sid = line.substr(0,p1);
        int iid=0; for (size_t i=0;i<sid.size();++i) if (sid[i]>='0'&&sid[i]<='9') iid = iid*10 + (sid[i]-'0');
        if (iid != id) continue;
        found=true;
        // parse full
        size_t p2=line.find('|',p1+1); size_t p3=line.find('|',p2+1); size_t p4=line.find('|',p3+1); size_t p5=line.find('|',p4+1); size_t p6=line.find('|',p5+1); size_t p7=line.find('|',p6+1); size_t p8=line.find('|',p7+1); size_t p9=line.find('|',p8+1);
        string date = (p2==string::npos)?string(""):line.substr(p1+1,p2-p1-1);
        string total = (p3==string::npos)?string(""):line.substr(p2+1,p3-p2-1);
        string discount = (p4==string::npos)?string(""):line.substr(p3+1,p4-p3-1);
        string finalAmt = (p5==string::npos)?string(""):line.substr(p4+1,p5-p4-1);
        string paid = (p6==string::npos)?string(""):line.substr(p5+1,p6-p5-1);
        string change = (p7==string::npos)?string(""):line.substr(p6+1,p7-p6-1);
        string promo = (p8==string::npos)?string(""):line.substr(p7+1,p8-p7-1);
        string member = (p9==string::npos)?string(""):line.substr(p8+1,p9-p8-1);
        string items = (p9==string::npos)?string(""):line.substr(p9+1);
        // print nicely with store header
        cout<<"\n--- Struk Transaksi (ID:"<<id<<") ---\n";
        cout<<"Toko Kosmetik - TPalPro\n";
        cout<<"Alamat: Jl. Contoh No.1\n";
        cout<<"Tanggal: "<<date<<"\n";
        cout<<"Member: "<<member<<"\n";
        cout<<"Items:\n";
        // items format code:qty:price,code2:...
        size_t pos=0; while (pos < items.size()) {
            size_t comma = items.find(',', pos);
            string chunk = (comma==string::npos)?items.substr(pos):items.substr(pos, comma-pos);
            size_t a = chunk.find(':'); size_t b = (a==string::npos)?string::npos:chunk.find(':', a+1);
            string code = (a==string::npos)?chunk:chunk.substr(0,a);
            string qty = (a==string::npos||b==string::npos)?string(""):chunk.substr(a+1,b-a-1);
            string price = (b==string::npos)?string(""):chunk.substr(b+1);
            cout<<" - "<<code<<" x"<<qty<<" @Rp"<<price<<"\n";
            if (comma==string::npos) break; pos = comma+1;
        }
        cout<<"Subtotal: Rp"<<total<<"\n";
        cout<<"Diskon: Rp"<<discount<<"\n";
        cout<<"Total akhir: Rp"<<finalAmt<<"\n";
        cout<<"Dibayar: Rp"<<paid<<" | Kembali: Rp"<<change<<"\n";
        cout<<"Promo: "<<promo<<"\n";
        cout<<"---\n";
        // offer to print to file
        string pr = inputLine("Cetak struk ke file? (y/n): ");
        Struk struk;

        if (pr == "y" || pr == "Y") {
        struk.cetakKeFile(id);
}
        break;
    }
    fin.close();
    if (!found) cout<<"Transaksi tidak ditemukan.\n";
}



void searchTransactions(const string& q) {
    ifstream fin(TRANSACTIONS_FILE.c_str()); if (!fin) { cout<<"Belum ada transaksi.\n"; return; }
    string line; int found=0; cout<<"\nHasil pencarian riwayat untuk '"<<q<<"':\n";
    while (getline(fin,line)) {
        if (line.empty()) continue;
        if (line.find(q) != string::npos) {
            // print brief
            size_t p1=line.find('|'); size_t p2=line.find('|',p1+1); size_t p4=line.find('|',p2+1);
            string id=line.substr(0,p1); string date=line.substr(p1+1,p2-p1-1); string finalAmt=line.substr(p4+1,p2-p4-1);
            cout<<"- ID:"<<id<<" | "<<date<<" | Rp"<<finalAmt<<"\n"; found++; }
    }
    if (!found) cout<<"Tidak ditemukan riwayat yang cocok.\n";
}

void cartMenu() {
Keranjang krj;
Produk produk;
    // keep existing cart in memory across menu usage (do not re-init here)
    while (true) {
        cout << "\n--- Keranjang Belanja ---\n";
        cout << "1. Tambah barang ke keranjang\n";
        cout << "2. Hapus barang dari keranjang\n";
        cout << "3. Ubah jumlah barang di keranjang\n";
        cout << "4. Lihat isi keranjang\n";
        cout << "5. Checkout\n";
        cout << "6. Kosongkan keranjang\n";
        cout << "7. Kembali\n";
        cout << "Pilih: ";
        string ch; getline(cin, ch);
        if (ch == "1") {
            cout << "\nDaftar produk tersedia:\n";
            produk.tampilProduk();
            string code = inputLine("Kode produk: ");
            string q = inputLine("Jumlah: ");
            int qty = 0; for (size_t i=0;i<q.size();++i) if (q[i]>='0'&&q[i]<='9') qty = qty*10 + (q[i]-'0');
            if (qty <= 0) { cout << "Jumlah tidak valid.\n"; continue; }
            if (krj.tambahBarang(code, qty)) cout << "Barang ditambahkan ke keranjang.\n";
            else cout << "Gagal menambahkan ke keranjang. (cek kode atau stok)\n";
        } else if (ch == "2") {
            krj.tampilKeranjang();
            string idxs = inputLine("Masukkan nomor item yang akan dihapus: ");
            int idx = 0; for (size_t i=0;i<idxs.size();++i) if (idxs[i]>='0'&&idxs[i]<='9') idx = idx*10 + (idxs[i]-'0');
            if (krj.hapusBarang(idx)) cout << "Item dihapus dari keranjang.\n";
            else cout << "Gagal menghapus item.\n";
        } else if (ch == "3") {
            krj.tampilKeranjang();
            string idxs = inputLine("Masukkan nomor item yang akan diubah: ");
            int idx = 0; for (size_t i=0;i<idxs.size();++i) if (idxs[i]>='0'&&idxs[i]<='9') idx = idx*10 + (idxs[i]-'0');
            string q = inputLine("Jumlah baru: ");
            int qty = 0; for (size_t i=0;i<q.size();++i) if (q[i]>='0'&&q[i]<='9') qty = qty*10 + (q[i]-'0');
            if (krj.ubahJumlah(idx, qty)) cout << "Jumlah diperbarui.\n";
            else cout << "Gagal memperbarui jumlah.\n";
        } else if (ch == "4") {
            krj.tampilKeranjang();
        } else if (ch == "5") {
            string date = inputLine("Tanggal transaksi (YYYY-MM-DD): ");
            if (checkoutCart(date)) cout << "Transaksi tersimpan.\n";
            else cout << "Checkout gagal.\n";
        } else if (ch == "6") {
            krj.kosongkanKeranjang();
            cout << "Keranjang dikosongkan.\n";
        } else if (ch == "7") {
            break;
        } else cout << "Pilihan tidak dikenali.\n";
    }
}

// --- Shift schedule & attendance implementation ---

void shiftMenu() {
    while (true) {
        cout << "\n--- Menu Jadwal Shift ---\n";
        cout << "1. Tambah jadwal shift\n";
        cout << "2. Lihat jadwal shift\n";
        cout << "3. Ubah jadwal shift\n";
        cout << "4. Hapus jadwal shift\n";
        cout << "5. Kembali\n";
        string c = inputLine("Pilih: ");
        Shift shift;
        if (c=="1") shift.tambahShift();
        else if (c=="2") shift.tampilShift();
        else if (c=="3") shift.editShift();
        else if (c=="4") shift.hapusShift();
        else if (c=="5") break;
        else cout<<"Pilihan tidak dikenali.\n";
    }
}

// shifts format: id|date|shift|employee
void addShift() {
    string date = inputLine("Tanggal (YYYY-MM-DD): "); if (date.empty()) { cout<<"Tanggal kosong.\n"; return; }
    cout<<"Tipe shift: 1=Opening,2=Middle,3=Closing\n";
    string s = inputLine("Pilih tipe shift (1/2/3): "); string shiftName;
    if (s=="1") shiftName="Opening"; else if (s=="2") shiftName="Middle"; else if (s=="3") shiftName="Closing"; else { cout<<"Tipe tidak valid.\n"; return; }
    string emp = inputLine("Username pegawai: "); if (emp.empty()) { cout<<"Username kosong.\n"; return; }
    // generate id
    int id=1; ifstream fin(SHIFTS_FILE.c_str()); string line; while (fin && getline(fin,line)) { if (!line.empty()) id++; } fin.close();
    ofstream fout(SHIFTS_FILE.c_str(), ios::app); if (!fout) { cout<<"Gagal menulis file shift.\n"; return; }
    fout << id << '|' << date << '|' << shiftName << '|' << emp << '\n'; fout.close();
    cout<<"Jadwal shift ditambahkan (ID:"<<id<<").\n";
}

void listShifts() {
    ifstream fin(SHIFTS_FILE.c_str()); if (!fin) { cout<<"Belum ada jadwal shift.\n"; return; }
    string line; cout<<"\nDaftar jadwal shift:\n";
    while (getline(fin,line)) {
        if (line.empty()) continue;
        // id|date|shift|employee
        size_t p1=line.find('|'); size_t p2=line.find('|',p1+1); size_t p3=line.find('|',p2+1); size_t p4=line.find('|',p3+1);
        if (p1==string::npos||p2==string::npos||p3==string::npos||p4==string::npos) continue;
        string id=line.substr(0,p1); string date=line.substr(p1+1,p2-p1-1); string shift=line.substr(p2+1,p3-p2-1); string emp=line.substr(p3+1);
        cout<<"- ID:"<<id<<" | "<<date<<" | "<<shift<<" | "<<emp<<"\n";
    }
    fin.close();
}

void editShift() {
    string rid = inputLine("Masukkan ID jadwal yang akan diedit: "); if (rid.empty()) { cout<<"ID kosong.\n"; return; }
    int id=0; for (size_t i=0;i<rid.size();++i) if (rid[i]>='0'&&rid[i]<='9') id=id*10+(rid[i]-'0'); if (id==0) { cout<<"ID tidak valid.\n"; return; }
    ifstream fin(SHIFTS_FILE.c_str()); if (!fin) { cout<<"File shift tidak ditemukan.\n"; return; }
    string all; string line; bool found=false;
    while (getline(fin,line)) {
        if (line.empty()) continue;
        size_t p1=line.find('|'); if (p1==string::npos) { all += line+'\n'; continue; }
        int cid=0; for (size_t i=0;i<p1;++i) if (line[i]>='0'&&line[i]<='9') cid=cid*10+(line[i]-'0');
        if (cid != id) { all += line+'\n'; continue; }
        found=true;
        string date = inputLine("Tanggal baru (YYYY-MM-DD): "); string s = inputLine("Tipe shift (1=Opening,2=Middle,3=Closing): "); string emp = inputLine("Username pegawai baru: "); string shiftName;
        if (s=="1") shiftName="Opening"; else if (s=="2") shiftName="Middle"; else if (s=="3") shiftName="Closing"; else shiftName = "Opening";
        all += rid + '|' + date + '|' + shiftName + '|' + emp + '\n';
    }
    fin.close(); if (!found) { cout<<"Jadwal ID tidak ditemukan.\n"; return; }
    ofstream fout(SHIFTS_FILE.c_str(), ios::trunc); if (!fout) { cout<<"Gagal menyimpan jadwal.\n"; return; } fout<<all; cout<<"Jadwal diperbarui.\n";
}

void deleteShift() {
    string rid = inputLine("Masukkan ID jadwal yang akan dihapus: "); if (rid.empty()) { cout<<"ID kosong.\n"; return; }
    int id=0; for (size_t i=0;i<rid.size();++i) if (rid[i]>='0'&&rid[i]<='9') id=id*10+(rid[i]-'0'); if (id==0) { cout<<"ID tidak valid.\n"; return; }
    if (!confirmAction(string("Konfirmasi hapus jadwal ID ")+rid+"?")) return;
    ifstream fin(SHIFTS_FILE.c_str()); if (!fin) { cout<<"File shift tidak ditemukan.\n"; return; }
    string all; string line; bool found=false;
    while (getline(fin,line)) {
        if (line.empty()) continue;
        size_t p1=line.find('|'); if (p1==string::npos) { all+=line+'\n'; continue; }
        int cid=0; for (size_t i=0;i<p1;++i) if (line[i]>='0'&&line[i]<='9') cid=cid*10+(line[i]-'0');
        if (cid==id) { found=true; continue; }
        all += line + '\n';
    }
    fin.close(); if (!found) { cout<<"Jadwal tidak ditemukan.\n"; return; }
    ofstream fout(SHIFTS_FILE.c_str(), ios::trunc); if (!fout) { cout<<"Gagal menyimpan file shift.\n"; return; } fout<<all; cout<<"Jadwal dihapus.\n";
}

// attendance: attendance format id|date|username|status
void attendanceMenuForUser(const string& username) {
    while (true) {
        cout << "\n--- Menu Presensi ("<<username<<") ---\n";
        cout << "1. Input presensi hari ini\n";
        cout << "2. Lihat riwayat presensi saya\n";
        cout << "3. Kembali\n";
        string c = inputLine("Pilih: ");
        if (c=="1") { recordAttendance(username); }
        else if (c=="2") { listAttendanceForEmployee(username); }
        else if (c=="3") break;
        else cout<<"Pilihan tidak dikenali.\n";
    }
}

void recordAttendance(const string& username) {
    string date = inputLine("Tanggal presensi (YYYY-MM-DD): "); if (date.empty()) { cout<<"Tanggal kosong.\n"; return; }
    cout<<"Status: 1=Hadir,2=Izin,3=Sakit,4=Alfa\n";
    string s = inputLine("Pilih status (1/2/3/4): "); string status;
    if (s=="1") status = "Hadir"; else if (s=="2") status = "Izin"; else if (s=="3") status = "Sakit"; else status = "Alfa";
    int id=1; ifstream fin(ATTENDANCE_FILE.c_str()); string line; while (fin && getline(fin,line)) { if (!line.empty()) id++; } fin.close();
    ofstream fout(ATTENDANCE_FILE.c_str(), ios::app); if (!fout) { cout<<"Gagal menulis file presensi.\n"; return; }
    fout << id << '|' << date << '|' << username << '|' << status << '\n'; fout.close();
    cout<<"Presensi tersimpan (ID:"<<id<<").\n";
}

void listAttendance() {
    ifstream fin(ATTENDANCE_FILE.c_str()); if (!fin) { cout<<"Belum ada data presensi.\n"; return; }
    string line; cout<<"\nDaftar presensi:\n";
    while (getline(fin,line)) {
        if (line.empty()) continue;
        size_t p1=line.find('|'); size_t p2=line.find('|',p1+1); size_t p3=line.find('|',p2+1); size_t p4=line.find('|',p3+1);
        if (p1==string::npos||p2==string::npos||p3==string::npos||p4==string::npos) continue;
        string id=line.substr(0,p1); string date=line.substr(p1+1,p2-p1-1); string user=line.substr(p2+1,p3-p2-1); string status=line.substr(p3+1);
        cout<<"- ID:"<<id<<" | "<<date<<" | "<<user<<" | "<<status<<"\n";
    }
    fin.close();
}

void listAttendanceForEmployee(const string& username) {
    if (username.empty()) { listAttendance(); return; }
    ifstream fin(ATTENDANCE_FILE.c_str()); if (!fin) { cout<<"Belum ada data presensi.\n"; return; }
    string line; cout<<"\nRiwayat presensi untuk "<<username<<":\n";
    int found=0;
    while (getline(fin,line)) {
        if (line.empty()) continue;
        size_t p1=line.find('|'); size_t p2=line.find('|',p1+1); size_t p3=line.find('|',p2+1); size_t p4=line.find('|',p3+1);
        if (p1==string::npos||p2==string::npos||p3==string::npos||p4==string::npos) continue;
        string date=line.substr(p1+1,p2-p1-1); string user=line.substr(p2+1,p3-p2-1); string status=line.substr(p3+1);
        if (user == username) { cout<<"- "<<date<<" | "<<status<<"\n"; found++; }
    }
    fin.close(); if (!found) cout<<"Tidak ditemukan riwayat untuk pengguna ini.\n";
}

// ======== FINANCIAL ANALYSIS FUNCTIONS ========

void monthlySalesReport() {
    ifstream fin(TRANSACTIONS_FILE.c_str());
    if (!fin) { cout<<"\nBelum ada data transaksi.\n"; return; }
    string line; double totalRevenue=0; int txCount=0; double totalDiscount=0;
    cout<<"\n========== LAPORAN PENJUALAN BULANAN ==========\n";
    cout<<"Format: ID|Tanggal|Total|Diskon|Final|Paid|Change|Promo|Member|Items\n";
    cout<<"\nDetail Transaksi:\n";
    cout<<"---------------------------------------------------------\n";
    cout<<"ID | Tgl      | Total     | Diskon | Final    | Member\n";
    cout<<"---------------------------------------------------------\n";
    while (getline(fin,line)) {
        if (line.empty()) continue;
        size_t p1=line.find('|'); size_t p2=line.find('|',p1+1); size_t p3=line.find('|',p2+1);
        size_t p4=line.find('|',p3+1); size_t p5=line.find('|',p4+1); size_t p6=line.find('|',p5+1);
        size_t p7=line.find('|',p6+1); size_t p8=line.find('|',p7+1); size_t p9=line.find('|',p8+1);
        if (p1==string::npos||p2==string::npos||p3==string::npos||p4==string::npos||p5==string::npos) continue;
        string id=line.substr(0,p1);
        string date=line.substr(p1+1,p2-p1-1);
        int total=0; for (size_t i=p2+1;i<p3;++i) if (line[i]>='0'&&line[i]<='9') total=total*10+(line[i]-'0');
        int diskon=0; for (size_t i=p3+1;i<p4;++i) if (line[i]>='0'&&line[i]<='9') diskon=diskon*10+(line[i]-'0');
        int final=0; for (size_t i=p4+1;i<p5;++i) if (line[i]>='0'&&line[i]<='9') final=final*10+(line[i]-'0');
        string member=(p8!=string::npos)?line.substr(p8+1,p9-p8-1):"Non-Member";
        cout<<id<<" | "<<date<<" | Rp"<<total<<" | Rp"<<diskon<<" | Rp"<<final<<" | "<<member<<"\n";
        totalRevenue+=final; totalDiscount+=diskon; txCount++;
    }
    fin.close();
    cout<<"---------------------------------------------------------\n";
    cout<<"Total Transaksi: "<<txCount<<"\n";
    cout<<"Total Diskon: Rp"<<(int)totalDiscount<<"\n";
    cout<<"Total Pendapatan Bersih: Rp"<<(int)totalRevenue<<"\n";
    cout<<"Rata-rata Transaksi: Rp"<<(txCount>0?(int)(totalRevenue/txCount):0)<<"\n";
    cout<<"=================================================\n";
}

void bestSellingAnalysis() {
    ifstream fin(TRANSACTIONS_FILE.c_str());
    if (!fin) { cout<<"\nBelum ada data transaksi.\n"; return; }
    string line; 
    cout<<"\n========== ANALISIS BARANG TERLARIS ==========\n";
    StringIntEntry productSales[MAX_ENTRIES];
    int productSalesSize = 0;
    StringIntEntry productCount[MAX_ENTRIES];
    int productCountSize = 0;
    
    while (getline(fin,line)) {
        if (line.empty()) continue;
        size_t p1=line.find('|'); size_t p2=line.find('|',p1+1); size_t p3=line.find('|',p2+1);
        size_t p4=line.find('|',p3+1); size_t p5=line.find('|',p4+1); size_t p6=line.find('|',p5+1);
        size_t p7=line.find('|',p6+1); size_t p8=line.find('|',p7+1); size_t p9=line.find('|',p8+1);
        if (p9==string::npos) continue;
        string items=line.substr(p9+1);
        
        // parse items: "P0001(2),P0002(1)" etc
        size_t pos=0;
        while (pos<items.length()) {
            size_t popen=items.find('(',pos);
            size_t pclose=items.find(')',popen);
            if (popen==string::npos||pclose==string::npos) break;
            string productId=items.substr(pos,popen-pos);
            int qty=0;
            for (size_t i=popen+1;i<pclose;++i)
                if (items[i]>='0'&&items[i]<='9') qty=qty*10+(items[i]-'0');
            addStringIntEntry(productSales, productSalesSize, productId, qty);
            addStringIntEntry(productCount, productCountSize, productId, 1);
            pos=pclose+1;
            if (pos<items.length() && items[pos]==',') pos++;
        }
    }
    fin.close();
    
    // Display results
    cout<<"\nProduk Terlaris (berdasarkan jumlah terjual):\n";
    cout<<"-------------------------------------------\n";
    cout<<"Rank | ID Produk | Jumlah Terjual | Frekuensi\n";
    cout<<"-------------------------------------------\n";
    
    ProductSummary sorted[MAX_ENTRIES];
    int sortedSize = 0;
    for (int i = 0; i < productSalesSize; ++i) {
        sorted[sortedSize].key = productSales[i].key;
        sorted[sortedSize].sold = productSales[i].value;
        sorted[sortedSize].transactions = getStringIntValue(productCount, productCountSize, productSales[i].key);
        ++sortedSize;
    }
    
    for (int i = 0; i < sortedSize; ++i) {
        for (int j = i + 1; j < sortedSize; ++j) {
            if (sorted[j].sold > sorted[i].sold) {
                swapProductSummary(sorted[i], sorted[j]);
            }
        }
    }
    
    int rank=1;
    for (int i = 0; i < sortedSize; ++i) {
        if (rank>10) break;
        cout<<rank<<" | "<<sorted[i].key<<" | "<<sorted[i].sold<<" unit | "<<sorted[i].transactions<<" transaksi\n";
        rank++;
    }
    cout<<"-------------------------------------------\n";
    cout<<"======================================\n";
}

void dailySalesReport() {
    ifstream fin(TRANSACTIONS_FILE.c_str());
    if (!fin) { cout<<"\nBelum ada data transaksi.\n"; return; }
    string line;
    cout<<"\n========== LAPORAN PENJUALAN HARIAN ==========\n";
    StringDoubleEntry dailyRevenue[MAX_ENTRIES];
    int dailyRevenueSize = 0;
    StringIntEntry dailyCount[MAX_ENTRIES];
    int dailyCountSize = 0;
    
    while (getline(fin,line)) {
        if (line.empty()) continue;
        size_t p1=line.find('|'); size_t p2=line.find('|',p1+1); size_t p3=line.find('|',p2+1);
        size_t p4=line.find('|',p3+1); size_t p5=line.find('|',p4+1);
        if (p5==string::npos) continue;
        string date=line.substr(p1+1,p2-p1-1);
        int final=0; for (size_t i=p4+1;i<p5;++i) 
            if (line[i]>='0'&&line[i]<='9') final=final*10+(line[i]-'0');
        addStringDoubleEntry(dailyRevenue, dailyRevenueSize, date, final);
        addStringIntEntry(dailyCount, dailyCountSize, date, 1);
    }
    fin.close();
    
    cout<<"\nRingkasan Penjualan Per Hari:\n";
    cout<<"-----------------------------------------------\n";
    cout<<"Tanggal    | Jumlah Transaksi | Total Pendapatan\n";
    cout<<"-----------------------------------------------\n";
    
    double totalAll=0;
    for (int i = 0; i < dailyRevenueSize; ++i) {
        cout<<dailyRevenue[i].key<<" | "<<getStringIntValue(dailyCount, dailyCountSize, dailyRevenue[i].key)<<" transaksi | Rp"<<(int)dailyRevenue[i].value<<"\n";
        totalAll+=dailyRevenue[i].value;
    }
    cout<<"-----------------------------------------------\n";
    cout<<"Total Keseluruhan: Rp"<<(int)totalAll<<"\n";
    cout<<"========================================\n";
}

void categorySalesAnalysis() {
    ifstream fprod(PRODUCTS_FILE.c_str());
    if (!fprod) { cout<<"\nFile produk tidak ditemukan.\n"; return; }
    ifstream fin(TRANSACTIONS_FILE.c_str());
    if (!fin) { cout<<"\nBelum ada data transaksi.\n"; return; }
    
    // Load product categories
    StringStringEntry productCategory[MAX_ENTRIES];
    int productCategorySize = 0;
    string line;
    while (getline(fprod,line)) {
        if (line.empty()) continue;
        size_t p1=line.find('|'); size_t p2=line.find('|',p1+1);
        if (p1==string::npos || p2==string::npos) continue;
        string pid=line.substr(0,p1);
        string cat=line.substr(p1+1,p2-p1-1);
        addStringStringEntry(productCategory, productCategorySize, pid, cat);
    }
    fprod.close();
    
    // Calculate sales by category
    StringDoubleEntry catRevenue[MAX_ENTRIES];
    int catRevenueSize = 0;
    StringIntEntry catQuantity[MAX_ENTRIES];
    int catQuantitySize = 0;
    
    while (getline(fin,line)) {
        if (line.empty()) continue;
        size_t p1=line.find('|'); size_t p2=line.find('|',p1+1); size_t p3=line.find('|',p2+1);
        size_t p4=line.find('|',p3+1); size_t p5=line.find('|',p4+1); size_t p6=line.find('|',p5+1);
        size_t p7=line.find('|',p6+1); size_t p8=line.find('|',p7+1); size_t p9=line.find('|',p8+1);
        if (p9==string::npos) continue;
        
        int final=0; for (size_t i=p4+1;i<p5;++i) 
            if (line[i]>='0'&&line[i]<='9') final=final*10+(line[i]-'0');
        string items=line.substr(p9+1);
        
        size_t pos=0;
        while (pos<items.length()) {
            size_t popen=items.find('(',pos);
            size_t pclose=items.find(')',popen);
            if (popen==string::npos||pclose==string::npos) break;
            string productId=items.substr(pos,popen-pos);
            int qty=0;
            for (size_t i=popen+1;i<pclose;++i)
                if (items[i]>='0'&&items[i]<='9') qty=qty*10+(items[i]-'0');
            
            int catIdx = findStringStringEntry(productCategory, productCategorySize, productId);
            if (catIdx >= 0) {
                string cat = productCategory[catIdx].value;
                addStringIntEntry(catQuantity, catQuantitySize, cat, qty);
                addStringDoubleEntry(catRevenue, catRevenueSize, cat, final * qty / 100.0);
            }
            pos=pclose+1;
            if (pos<items.length() && items[pos]==',') pos++;
        }
    }
    fin.close();
    
    cout<<"\n========== ANALISIS PENJUALAN PER KATEGORI ==========\n";
    cout<<"Kategori | Jumlah Item Terjual | Total Pendapatan\n";
    cout<<"----------------------------------------------------\n";
    double totalCat=0;
    for (int i = 0; i < catRevenueSize; ++i) {
        cout<<catRevenue[i].key<<" | "<<getStringIntValue(catQuantity, catQuantitySize, catRevenue[i].key)<<" | Rp"<<(int)catRevenue[i].value<<"\n";
        totalCat+=catRevenue[i].value;
    }
    cout<<"----------------------------------------------------\n";
    cout<<"Total Pendapatan dari Semua Kategori: Rp"<<(int)totalCat<<"\n";
    cout<<"==================================================\n";
}

void revenueByDateRange() {
    string startDate=inputLine("Masukkan tanggal mulai (YYYY-MM-DD): ");
    string endDate=inputLine("Masukkan tanggal akhir (YYYY-MM-DD): ");
    
    ifstream fin(TRANSACTIONS_FILE.c_str());
    if (!fin) { cout<<"\nBelum ada data transaksi.\n"; return; }
    
    string line;
    double revenue=0; int count=0; double discount=0;
    cout<<"\n========== LAPORAN PENDAPATAN PER PERIODE ==========\n";
    cout<<"Periode: "<<startDate<<" hingga "<<endDate<<"\n";
    cout<<"---------------------------------------------------\n";
    
    while (getline(fin,line)) {
        if (line.empty()) continue;
        size_t p1=line.find('|'); size_t p2=line.find('|',p1+1); size_t p3=line.find('|',p2+1);
        size_t p4=line.find('|',p3+1); size_t p5=line.find('|',p4+1);
        if (p5==string::npos) continue;
        
        string date=line.substr(p1+1,p2-p1-1);
        if (date>=startDate && date<=endDate) {
            int total=0; for (size_t i=p2+1;i<p3;++i) 
                if (line[i]>='0'&&line[i]<='9') total=total*10+(line[i]-'0');
            int diskon=0; for (size_t i=p3+1;i<p4;++i) 
                if (line[i]>='0'&&line[i]<='9') diskon=diskon*10+(line[i]-'0');
            int final=0; for (size_t i=p4+1;i<p5;++i) 
                if (line[i]>='0'&&line[i]<='9') final=final*10+(line[i]-'0');
            revenue+=final;
            discount+=diskon;
            count++;
        }
    }
    fin.close();
    
    cout<<"Total Transaksi: "<<count<<"\n";
    cout<<"Total Diskon: Rp"<<(int)discount<<"\n";
    cout<<"Total Pendapatan: Rp"<<(int)revenue<<"\n";
    cout<<"Rata-rata per Transaksi: Rp"<<(count>0?(int)(revenue/count):0)<<"\n";
    cout<<"====================================================\n";
}

void profitMarginAnalysis() {
    ifstream fprod(PRODUCTS_FILE.c_str());
    ifstream fin(TRANSACTIONS_FILE.c_str());
    if (!fprod || !fin) { cout<<"\nFile tidak ditemukan.\n"; return; }
    
    string line;
    cout<<"\n========== ANALISIS MARGIN KEUNTUNGAN ==========\n";
    cout<<"Catatan: Perhitungan berdasarkan harga jual produk\n";
    cout<<"-----------------------------------------------\n";
    cout<<"ID Produk | Nama Produk | Harga Jual | Margin\n";
    cout<<"-----------------------------------------------\n";
    
    while (getline(fprod,line)) {
        if (line.empty()) continue;
        size_t p1=line.find('|'); size_t p2=line.find('|',p1+1); size_t p3=line.find('|',p2+1);
        if (p1==string::npos || p2==string::npos || p3==string::npos) continue;
        
        string pid=line.substr(0,p1);
        string pname=line.substr(p1+1,p2-p1-1);
        int price=0; for (size_t i=p2+1;i<p3;++i) 
            if (line[i]>='0'&&line[i]<='9') price=price*10+(line[i]-'0');
        
        // Assume 40% markup/margin
        int margin=(price*40)/100;
        cout<<pid<<" | "<<pname<<" | Rp"<<price<<" | Rp"<<margin<<" (40%)\n";
    }
    fprod.close();
    fin.close();
    cout<<"-----------------------------------------------\n";
    cout<<"=============================================\n";
}

void memberSalesAnalysis() {
    ifstream fin(TRANSACTIONS_FILE.c_str());
    if (!fin) { cout<<"\nBelum ada data transaksi.\n"; return; }
    
    string line;
    MemberSummary memberRevenue[MAX_ENTRIES];
    int memberRevenueSize = 0;
    double nonMemberRevenue=0;
    int nonMemberCount=0;
    
    while (getline(fin,line)) {
        if (line.empty()) continue;
        size_t p1=line.find('|'); size_t p2=line.find('|',p1+1); size_t p3=line.find('|',p2+1);
        size_t p4=line.find('|',p3+1); size_t p5=line.find('|',p4+1); size_t p6=line.find('|',p5+1);
        size_t p7=line.find('|',p6+1); size_t p8=line.find('|',p7+1);
        if (p8==string::npos) continue;
        
        int final=0; for (size_t i=p4+1;i<p5;++i) 
            if (line[i]>='0'&&line[i]<='9') final=final*10+(line[i]-'0');
        string member=line.substr(p8+1);
        
        if (member=="Non-Member") {
            nonMemberRevenue+=final;
            nonMemberCount++;
        } else {
            int idx = -1;
            for (int i = 0; i < memberRevenueSize; ++i) {
                if (memberRevenue[i].key == member) {
                    idx = i;
                    break;
                }
            }
            if (idx >= 0) {
                memberRevenue[idx].revenue += final;
                memberRevenue[idx].count++;
            } else {
                memberRevenue[memberRevenueSize].key = member;
                memberRevenue[memberRevenueSize].revenue = final;
                memberRevenue[memberRevenueSize].count = 1;
                ++memberRevenueSize;
            }
        }
    }
    fin.close();
    
    cout<<"\n========== ANALISIS PENJUALAN PER MEMBER ==========\n";
    cout<<"Member | Jumlah Transaksi | Total Pendapatan\n";
    cout<<"---------------------------------------------------\n";
    
    for (int i = 0; i < memberRevenueSize; ++i) {
        cout<<memberRevenue[i].key<<" | "<<memberRevenue[i].count<<" | Rp"<<(int)memberRevenue[i].revenue<<"\n";
    }
    cout<<"\nPenjualan Non-Member: "<<nonMemberCount<<" transaksi, Rp"<<(int)nonMemberRevenue<<"\n";
    double totalAllMember=0;
    for (int i = 0; i < memberRevenueSize; ++i) totalAllMember+=memberRevenue[i].revenue;
    cout<<"Total: Rp"<<(int)(nonMemberRevenue+totalAllMember)<<" (semua member & non-member)\n";
    cout<<"====================================================\n";
}

void financialDashboard() {
    while (true) {
        cout<<"\n========== DASHBOARD ANALISIS KEUANGAN ==========\n";
        cout<<"1. Laporan Penjualan Bulanan\n";
        cout<<"2. Analisis Barang Terlaris\n";
        cout<<"3. Laporan Penjualan Harian\n";
        cout<<"4. Analisis Penjualan Per Kategori\n";
        cout<<"5. Pendapatan Per Periode Tanggal\n";
        cout<<"6. Analisis Margin Keuntungan\n";
        cout<<"7. Analisis Penjualan Per Member\n";
        cout<<"8. Kembali\n";
        cout<<"==================================================\n";
        string choice=inputLine("Pilih menu (1-8): ");
        
        if (choice=="1") monthlySalesReport();
        else if (choice=="2") bestSellingAnalysis();
        else if (choice=="3") dailySalesReport();
        else if (choice=="4") categorySalesAnalysis();
        else if (choice=="5") revenueByDateRange();
        else if (choice=="6") profitMarginAnalysis();
        else if (choice=="7") memberSalesAnalysis();
        else if (choice=="8") break;
        else cout<<"Pilihan tidak dikenali.\n";
    }
}
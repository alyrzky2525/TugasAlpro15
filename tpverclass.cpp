#include <iostream>
#include <fstream>
#include <string>
#include <iomanip> // Mengatur format tampilan output (setw, left, right, fixed, setprecision)
#include <sstream>  // Mengolah string menggunakan stringstream untuk parsing data dari file

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
void menuproduk();
void kelolaKategori();
void ensureDefaultCategories();

void KeranjangMenu();
bool productCodeExists(const string& code);
bool checkoutCart(const string& curDate);
void saveTransactionToFile(const string& date, double total, const string& items, const string& member, double paid, double change, double discount, const string& promoCode);
void editProduk();
void tambahkankategori();
void daftarKategori();
string toLowerCase(string text);
int jumlahHariBulan(int bulan, int tahun);
int tanggalKeHari(string tgl);
string statusExpired(string hariIni, string expired);
// promos
bool promoCodeExists(const string& code);
void tampilPromo();
void tambahPromo();
void editPromo();
void hapusPromo();
double applyPromo(const string& promoCode, const string& date, bool isMember, bool &valid, string &outPromoCode);
void kelolaPromo();
// shift 
void kelolaShift();
void tambahShift();
void listShifts();
void editShift();
void deleteShift();
void attendanceMenuForUser(const string& username);
void recordAttendance(const string& username);
void listAttendance();
void listAttendanceForEmployee(const string& username);
// laporan penjualan & analisis 
void laporanPenjualanBulanan();
void analisisProdukTerlaris();
void hitungPenjualanHarian();
void analisisPenjualanPerKategori();
void laporanPendapatanPerPeriode();
void analisisMarginKeuntungan();
void analisisPenjualanMember();
void menuDashboardKeuangan();
void ensureDefaultData();
// riwayat transaksi
void listTransaksi();
void viewTransactionDetails(int id);
void searchTransactions(const string& q);
// membership
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

bool validateCredentials(const string& username,
                         const string& password,
                         string& outRole) {
    ifstream fin(USER_FILE.c_str());

    if (!fin) {
        return false;
    }

    string line;

    while (getline(fin, line)) {
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);

        if (p1 == string::npos || p2 == string::npos) {
            continue;
        }

        string usernameInFile = line.substr(0, p1);
        string passwordInFile = line.substr(p1 + 1, p2 - p1 - 1);
        string roleInFile = line.substr(p2 + 1);

        if (usernameInFile == username &&
            passwordInFile == password) {
            outRole = roleInFile;
            return true;
        }
    }

    return false;
}

bool registerUser(const string& username,
                  const string& password,
                  const string& role) {
    ofstream fout(USER_FILE.c_str(), ios::app);

    if (!fout) {
        return false;
    }

    fout << username << '|' << password << '|' << role << '\n';

    return true;
}
bool changePassword(const string& username,
                    const string& newPassword) {
    ifstream fin(USER_FILE.c_str());

    if (!fin) {
        return false;
    }

    string all;
    bool found = false;
    string line;

    while (getline(fin, line)) {
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);

        if (p1 == string::npos || p2 == string::npos) {
            continue;
        }

        string usernameInFile = line.substr(0, p1);
        string roleInFile = line.substr(p2 + 1);

        if (usernameInFile == username) {
            all += usernameInFile + '|' + newPassword + '|' + roleInFile + '\n';
            found = true;
        } else {
            all += line + '\n';
        }
    }

    fin.close();

    if (!found) {
        return false;
    }

    ofstream fout(USER_FILE.c_str(), ios::trunc);

    if (!fout) {
        return false;
    }

    fout << all;

    return true;
}

void listUsers() {
    ifstream fin(USER_FILE.c_str());

    if (!fin) {
        cout << "Belum ada pengguna atau file tidak tersedia.\n";
        return;
    }

    string line;
    int idx = 1;

    cout << "\nDaftar pengguna:\n";

    while (getline(fin, line)) {
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);

        if (p1 == string::npos || p2 == string::npos) {
            continue;
        }

        string usernameInFile = line.substr(0, p1);
        string roleInFile = line.substr(p2 + 1);

        cout << idx++ << ". "
             << usernameInFile
             << " ("
             << roleInFile
             << ")\n";
    }

    if (idx == 1) {
        cout << "Belum ada pengguna.\n";
    }
}

bool deleteUser(const string& username) {
    ifstream fin(USER_FILE.c_str());

    if (!fin) {
        return false;
    }

    string all;
    string line;
    bool found = false;

    while (getline(fin, line)) {
        size_t p1 = line.find('|');

        if (p1 == string::npos) {
            continue;
        }

        string usernameInFile = line.substr(0, p1);

        if (usernameInFile == username) {
            found = true;
            continue;
        }

        all += line + '\n';
    }

    fin.close();

    if (!found) {
        return false;
    }

    // Konfirmasi sebelum menghapus pengguna
    if (!confirmAction("Konfirmasi hapus pengguna '" + username + "'?")) {
        return false;
    }

    ofstream fout(USER_FILE.c_str(), ios::trunc);

    if (!fout) {
        return false;
    }

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
        cout << "\n";
        cout << "==================================================\n";
        cout << "           SISTEM MANAJEMEN KOSMETIK\n";
        cout << "==================================================\n";
        cout << "                 DASHBOARD ADMIN\n";
        cout << "==================================================\n";
        cout << " Selamat Datang : " << username << " (Admin)\n";
        cout << "--------------------------------------------------\n";
        cout << " 1. Logout\n";
        cout << " 2. Ganti Password\n";
        cout << " 3. Daftar Pengguna\n";
        cout << " 4. Hapus Pengguna\n";
        cout << " 5. Buat Pengguna Baru\n";
        cout << " 6. Kelola Produk (CRUD)\n";
        cout << " 7. Kelola Kategori\n";
        cout << " 8. Kelola Promo\n";
        cout << " 9. Riwayat Transaksi\n";
        cout << "10. Jadwal Shift\n";
        cout << "11. Laporan & Analisis\n";
        cout << "--------------------------------------------------\n";
        cout << " Pilih Menu : ";

        string choice;
        getline(cin, choice);

        if (choice == "1") {
            break;

        } else if (choice == "2") {
            string newPassword = inputLine("Password baru: ");
            string confirmPassword = inputLine("Konfirmasi password baru: ");

            if (newPassword == confirmPassword) {
                if (changePassword(username, newPassword)) {
                    cout << "Password berhasil diubah.\n";
                } else {
                    cout << "Gagal mengubah password.\n";
                }
            } else {
                cout << "Konfirmasi tidak sama.\n";
            }

        } else if (choice == "3") {
            listUsers();

        } else if (choice == "4") {
            string target = inputLine("Masukkan username yang akan dihapus: ");

            if (target == username) {
                cout << "Tidak dapat menghapus akun sendiri dari dashboard.\n";
                continue;
            }

            if (!usernameExists(target)) {
                cout << "Username tidak ditemukan.\n";
                continue;
            }

            if (deleteUser(target)) {
                cout << "Pengguna dihapus.\n";
            } else {
                cout << "Gagal menghapus pengguna.\n";
            }

        } else if (choice == "5") {
            string usernameBaru = inputLine("Pilih username: ");

            if (usernameBaru.empty()) {
                cout << "Username tidak boleh kosong.\n";
                continue;
            }

            if (usernameExists(usernameBaru)) {
                cout << "Username sudah ada.\n";
                continue;
            }

            string password = inputLine("Password: ");

            if (password.length() < 8) {
                cout << "Password minimal 8 karakter.\n";
                continue;
            }

            string confirmPassword = inputLine("Konfirmasi password: ");

            if (password != confirmPassword) {
                cout << "Konfirmasi tidak sama.\n";
                continue;
            }

            string role = inputLine("Role (admin/kasir): ");

            if (role != "admin" && role != "kasir") {
                cout << "Role harus 'admin' atau 'kasir'.\n";
                continue;
            }

            cout << "\nRingkasan pengguna baru:\n";
            cout << " - Username : " << usernameBaru << '\n';
            cout << " - Role     : " << role << '\n';

            if (!confirmAction("Konfirmasi buat pengguna?")) {
                cout << "Pembuatan pengguna dibatalkan.\n";
                continue;
            }

            if (registerUser(usernameBaru, password, role)) {
                cout << "Pengguna baru dibuat.\n";
            } else {
                cout << "Gagal membuat pengguna.\n";
            }

        } else if (choice == "6") {
            menuproduk();

        } else if (choice == "7") {
            kelolaKategori();

        } else if (choice == "8") {
            kelolaPromo();

        } else if (choice == "9") {
            listTransaksi();

        } else if (choice == "10") {
            kelolaShift();

        } else if (choice == "11") {
            menuDashboardKeuangan();

        } else {
            cout << "Pilihan tidak dikenali.\n";
        }
    }
}

string formatRupiah(int angka) {
    string s = to_string(angka);
    string hasil = "";
    int hitung = 0;

    for (int i = s.length() - 1; i >= 0; i--) {
        hasil = s[i] + hasil;
        hitung++;

        if (hitung == 3 && i != 0) {
            hasil = "." + hasil;
            hitung = 0;
        }
    }

    return "Rp" + hasil;
}

// --- Cart & Transactions implementation ---
const int MAX_CART = 200;

string cartCode[MAX_CART];
string cartName[MAX_CART];
double cartPrice[MAX_CART];
int cartQty[MAX_CART];
double cartSubtotal[MAX_CART];

int cartCount = 0;

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
        cout << "\n";
        cout << "==================================================\n";
        cout << "           SISTEM MANAJEMEN KOSMETIK\n";
        cout << "==================================================\n";
        cout << "                 DASHBOARD ADMIN\n";
        cout << "==================================================\n";
        cout << " Selamat Datang : " << username << " (Admin)\n";
        cout << "--------------------------------------------------\n";
        cout << " 1. Logout\n";
        cout << " 2. Ganti Password\n";
        cout << " 3. Daftar Pengguna\n";
        cout << " 4. Hapus Pengguna\n";
        cout << " 5. Buat Pengguna Baru\n";
        cout << " 6. Kelola Produk (CRUD)\n";
        cout << " 7. Kelola Kategori\n";
        cout << " 8. Kelola Promo\n";
        cout << " 9. Riwayat Transaksi\n";
        cout << "10. Jadwal Shift\n";
        cout << "11. Laporan & Analisis\n";
        cout << "--------------------------------------------------\n";
        cout << "Pilih Menu : ";

        string choice;
        getline(cin, choice);

        if (choice == "1") { 
            break;

        } else if (choice == "2") {
            string np = inputLine("Password baru: ");
            string nc = inputLine("Konfirmasi password baru: ");

            if (np == nc) {
                if (changePassword(username, np)) {
                    cout << "Password berhasil diubah.\n";
                } else {
                    cout << "Gagal mengubah password.\n";
                }
            } else {
                cout << "Konfirmasi tidak sama.\n";
            }

        } else if (choice == "3") {
            listUsers();

        } else if (choice == "4") {
            string target = inputLine("Masukkan username yang akan dihapus: ");

            if (target == username) {
                cout << "Tidak dapat menghapus akun sendiri dari dashboard.\n";
                continue;
            }

            if (!usernameExists(target)) {
                cout << "Username tidak ditemukan.\n";
                continue;
            }

            if (deleteUser(target)) {
                cout << "Pengguna dihapus.\n";
            } else {
                cout << "Gagal menghapus pengguna.\n";
            }

        } else if (choice == "5") {
            string u = inputLine("Pilih username: ");

            if (u.empty()) {
                cout << "Username tidak boleh kosong.\n";
                continue;
            }

            if (usernameExists(u)) {
                cout << "Username sudah ada.\n";
                continue;
            }

            string pw = inputLine("Password: ");

            if (pw.length() < 8) {
                cout << "Password minimal 8 karakter.\n";
                return;
            }

            string conf = inputLine("Konfirmasi password: ");

            if (pw != conf) {
                cout << "Konfirmasi tidak sama.\n";
                continue;
            }

            string role = inputLine("Role (admin/kasir): ");

            if (role != "admin" && role != "kasir") {
                cout << "Role harus 'admin' atau 'kasir'.\n";
                continue;
            }

            cout << "\nRingkasan Pengguna Baru\n";
            cout << "--------------------------------------------------\n";
            cout << " Username : " << u << "\n";
            cout << " Role     : " << role << "\n";
            cout << "--------------------------------------------------\n";

            if (!confirmAction("Konfirmasi buat pengguna?")) {
                cout << "Pembuatan pengguna dibatalkan.\n";
                continue;
            }

            if (registerUser(u, pw, role)) {
                cout << "Pengguna baru dibuat.\n";
            } else {
                cout << "Gagal membuat pengguna.\n";
            }

        } else if (choice == "6") {
            menuproduk();

        } else if (choice == "7") {
            kelolaKategori();

        } else if (choice == "8") {
            kelolaPromo();

        } else if (choice == "9") {
            listTransaksi();

        } else if (choice == "10") {
            kelolaShift();

        } else if (choice == "11") {
            menuDashboardKeuangan();

        } else {
            cout << "Pilihan tidak dikenali.\n";
        }
    }
}
        void tambahProduk() {

    string kode = inputLine("Kode produk: ");
    if (kode.empty()) {
        cout << "Kode tidak boleh kosong.\n";
        return;
    }
    if (productCodeExists(kode)) {
        cout << "Kode sudah ada.\n";
        return;
    }

    string nama = inputLine("Nama produk: ");
    if (nama.empty()) {
        cout << "Nama produk tidak boleh kosong.\n";
        return;
    }

    string kategori = inputLine("Kategori: ");
    if (kategori.empty()) {
        cout << "Kategori tidak boleh kosong.\n";
        return;
    }

    string hargaModal = inputLine("Harga modal: ");
    if (hargaModal.empty()) {
        cout << "Harga modal tidak boleh kosong.\n";
        return;
    }

    string hargaJual = inputLine("Harga jual: ");
    if (hargaJual.empty()) {
        cout << "Harga jual tidak boleh kosong.\n";
        return;
    }

    string stok = inputLine("Stok: ");
    if (stok.empty()) {
        cout << "Stok tidak boleh kosong.\n";
        return;
    }

    string expired = inputLine("Expired (YYYY-MM-DD): ");
    if (expired.empty()) {
        cout << "Tanggal expired tidak boleh kosong.\n";
        return;
    }

    ofstream fout(PRODUCTS_FILE.c_str(), ios::app);
    if (!fout) {
        cout << "Gagal menulis file produk.\n";
        return;
    }

    fout << kode << '|'
         << nama << '|'
         << kategori << '|'
         << hargaModal << '|'
         << hargaJual << '|'
         << stok << '|'
         << expired << '\n';

    fout.close();

    cout << "Produk berhasil ditambahkan.\n";
}
void editProduk() {

    string kode = inputLine("Masukkan kode produk yang akan diedit: ");

    if (!productCodeExists(kode)) {
        cout << "Kode tidak ditemukan.\n";
        return;
    }

    ifstream fin(PRODUCTS_FILE.c_str());

    string all;
    string line;

    while (getline(fin, line)) {

        if (line.empty()) continue;

        size_t p = line.find('|');

        if (p == string::npos) continue;

        string c = line.substr(0, p);

        if (c == kode) {

            cout << "\n=== Edit Produk ===\n";

            string nama = inputLine("Nama Produk : ");

            // PILIH BRAND
            cout << "\nPilih Brand\n";
            cout << "1. Wardah\n";
            cout << "2. Emina\n";
            cout << "3. Skintific\n";
            cout << "4. Somethinc\n";
            cout << "5. Garnier\n";
            cout << "6. Azarine\n";
            cout << "7. Maybelline\n";
            cout << "8. Implora\n";
            cout << "9. Pinkflash\n";
            cout << "10. Scarlett\n";
            cout << "11. Hada Labo\n";
            cout << "12. Nivea\n";
            cout << "13. Vaseline\n";
            cout << "14. HMNS\n";
            cout << "15. Lainnya\n";

            string pilihBrand = inputLine("Pilihan : ");
            string brand;

            if (pilihBrand=="1") brand="Wardah";
            else if (pilihBrand=="2") brand="Emina";
            else if (pilihBrand=="3") brand="Skintific";
            else if (pilihBrand=="4") brand="Somethinc";
            else if (pilihBrand=="5") brand="Garnier";
            else if (pilihBrand=="6") brand="Azarine";
            else if (pilihBrand=="7") brand="Maybelline";
            else if (pilihBrand=="8") brand="Implora";
            else if (pilihBrand=="9") brand="Pinkflash";
            else if (pilihBrand=="10") brand="Scarlett";
            else if (pilihBrand=="11") brand="Hada Labo";
            else if (pilihBrand=="12") brand="Nivea";
            else if (pilihBrand=="13") brand="Vaseline";
            else if (pilihBrand=="14") brand="HMNS";
            else if (pilihBrand=="15")
                brand = inputLine("Masukkan Brand : ");
            else {
                cout<<"Brand tidak valid.\n";
                return;
            }

            // PILIH KATEGORI
            cout << "\nPilih Kategori\n";
            cout << "1. Skincare\n";
            cout << "2. Makeup\n";
            cout << "3. Body Care\n";
            cout << "4. Hair Care\n";
            cout << "5. Fragrance\n";
            cout << "6. Beauty Tools\n";
            cout << "7. Personal Care\n";

            string pilihKategori = inputLine("Pilihan : ");
            string kategori;

            if (pilihKategori=="1") kategori="Skincare";
            else if (pilihKategori=="2") kategori="Makeup";
            else if (pilihKategori=="3") kategori="Body Care";
            else if (pilihKategori=="4") kategori="Hair Care";
            else if (pilihKategori=="5") kategori="Fragrance";
            else if (pilihKategori=="6") kategori="Beauty Tools";
            else if (pilihKategori=="7") kategori="Personal Care";
            else {
                cout<<"Kategori tidak valid.\n";
                return;
            }

            string hargaBeli = inputLine("Harga Beli : ");
            string hargaJual = inputLine("Harga Jual : ");
            string stok = inputLine("Stok : ");
            string minimalStok = inputLine("Minimal Stok : ");
            string expired = inputLine("Expired (YYYY-MM-DD) : ");

            all += kode + "|" +
                   nama + "|" +
                   brand + "|" +
                   kategori + "|" +
                   hargaBeli + "|" +
                   hargaJual + "|" +
                   stok + "|" +
                   minimalStok + "|" +
                   expired + "\n";

        }
        else {

            all += line + "\n";

        }

    }

    fin.close();

    ofstream fout(PRODUCTS_FILE.c_str(), ios::trunc);

    if (!fout) {

        cout << "Gagal menulis file produk.\n";
        return;

    }

    fout << all;

    fout.close();

    cout << "\nProduk berhasil diperbarui.\n";
}

void kelolaKategori() {
    while (true) {
        cout << "\n";
        cout << "==================================================\n";
        cout << "           SISTEM MANAJEMEN KOSMETIK\n";
        cout << "==================================================\n";
        cout << "                MENU KATEGORI\n";
        cout << "==================================================\n";
        cout << " 1. Tambah Kategori\n";
        cout << " 2. Lihat Kategori\n";
        cout << " 3. Kembali\n";
        cout << "--------------------------------------------------\n";

        string c = inputLine("Pilih Menu : ");

        if (c == "1") {
            tambahkankategori();
        } else if (c == "2") {
            daftarKategori();
        } else if (c == "3") {
            break;
        } else {
            cout << "Pilihan tidak dikenali.\n";
        }
    }
}
void kelolaPromo() {
    while (true) {
        cout << "\n";
        cout << "==================================================\n";
        cout << "           SISTEM MANAJEMEN KOSMETIK\n";
        cout << "==================================================\n";
        cout << "                  MENU PROMO\n";
        cout << "==================================================\n";
        cout << " 1. Tambah Promo\n";
        cout << " 2. Edit Promo\n";
        cout << " 3. Hapus Promo\n";
        cout << " 4. Lihat Promo\n";
        cout << " 5. Kembali\n";
        cout << "--------------------------------------------------\n";

        string c = inputLine("Pilih Menu : ");

        if (c == "1") {
            tambahPromo();
        } else if (c == "2") {
            editPromo();
        } else if (c == "3") {
            hapusPromo();
        } else if (c == "4") {
            tampilPromo();
        } else if (c == "5") {
            break;
        } else {
            cout << "Pilihan tidak dikenali.\n";
        }
    }
}
      void kelolaShift() {
    while (true) {
        cout << "\n";
        cout << "==================================================\n";
        cout << "           SISTEM MANAJEMEN KOSMETIK\n";
        cout << "==================================================\n";
        cout << "              MENU JADWAL SHIFT\n";
        cout << "==================================================\n";
        cout << " 1. Tambah Jadwal Shift\n";
        cout << " 2. Lihat Jadwal Shift\n";
        cout << " 3. Ubah Jadwal Shift\n";
        cout << " 4. Hapus Jadwal Shift\n";
        cout << " 5. Kembali\n";
        cout << "--------------------------------------------------\n";

        string c = inputLine("Pilih Menu : ");

        if (c == "1") {
            tambahShift();
        } else if (c == "2") {
            listShifts();
        } else if (c == "3") {
            editShift();
        } else if (c == "4") {
            deleteShift();
        } else if (c == "5") {
            break;
        } else {
            cout << "Pilihan tidak dikenali.\n";
        }
    }
}
        void lihatLaporan() {
            menuDashboardKeuangan();
        }
        void buatPengguna() {
            cout << "\n";
            cout << "==================================================\n";
            cout << "              BUAT PENGGUNA BARU\n";
            cout << "==================================================\n";

            string u = inputLine("Username             : ");

            if (u.empty()) {
                cout << "Username tidak boleh kosong.\n";
                    return;
            }

            if (usernameExists(u)) {
                cout << "Username sudah ada.\n";
                    return;
            }

            string pw = inputLine("Password             : ");

             if (pw.length() < 8) {
                cout << "Password minimal 8 karakter.\n";
                    return;
        }

            string conf = inputLine("Konfirmasi Password  : ");

            if (pw != conf) {
                cout << "Konfirmasi tidak sama.\n";
                    return;
        }

            string role = inputLine("Role (admin/kasir)   : ");

            if (role != "admin" && role != "kasir") {
                cout << "Role harus 'admin' atau 'kasir'.\n";
                    return;
        }

                cout << "\n";
                cout << "--------------------------------------------------\n";
                cout << "             RINGKASAN PENGGUNA BARU\n";
                cout << "--------------------------------------------------\n";
                cout << " Username : " << u << "\n";
                cout << " Role     : " << role << "\n";
                cout << "--------------------------------------------------\n";

            if (!confirmAction("Konfirmasi buat pengguna?")) {
                cout << "Pembuatan pengguna dibatalkan.\n";
                 return;
        }

            if (registerUser(u, pw, role)) {
                cout << "Pengguna baru dibuat.\n";
             } else {
                 cout << "Gagal membuat pengguna.\n";
        }
}
       void hapusPengguna() {
    cout << "\n";
    cout << "==================================================\n";
    cout << "               HAPUS PENGGUNA\n";
    cout << "==================================================\n";

    string target = inputLine("Masukkan Username : ");

    if (target.empty()) {
        cout << "Username kosong.\n";
        return;
    }

    if (!usernameExists(target)) {
        cout << "Username tidak ditemukan.\n";
        return;
    }

    if (deleteUser(target)) {
        cout << "Pengguna dihapus.\n";
    } else {
        cout << "Gagal menghapus pengguna.\n";
    }
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

    cout << "\n===================================================================================================================================\n";
cout << left
     << setw(5)  << "No"
     << setw(10) << "Kode"
     << setw(30) << "Nama Produk"
     << setw(18) << "Brand"
     << setw(16) << "Kategori"
     << right
     << setw(16) << "Harga Beli"
     << setw(16) << "Harga Jual"
     << setw(16) << "Margin"
     << setw(8)  << "Stok"
     << setw(12) << "Min Stok"
     << " "                      // separator
     << left
     << setw(12) << "Status"
     << setw(15) << "Expired"
     << endl;

    cout << "===================================================================================================================================\n";

    int no = 1;

    while (getline(fin, line)) {

        if (line.empty()) continue;

        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        size_t p5 = line.find('|', p4 + 1);
        size_t p6 = line.find('|', p5 + 1);
        size_t p7 = line.find('|', p6 + 1);
        size_t p8 = line.find('|', p7 + 1);

        if (p1==string::npos||p2==string::npos||p3==string::npos||
            p4==string::npos||p5==string::npos||p6==string::npos||
            p7==string::npos||p8==string::npos)
            continue;

        string kode       = line.substr(0,p1);
        string nama       = line.substr(p1+1,p2-p1-1);
        string brand      = line.substr(p2+1,p3-p2-1);
        string kategori   = line.substr(p3+1,p4-p3-1);
        string hargaBeli  = line.substr(p4+1,p5-p4-1);
        string hargaJual  = line.substr(p5+1,p6-p5-1);
        string stok       = line.substr(p6+1,p7-p6-1);
        string minStok    = line.substr(p7+1,p8-p7-1);
        string expired    = line.substr(p8+1);

        int stokInt = atoi(stok.c_str());
        int minInt = atoi(minStok.c_str());

        int beli = atoi(hargaBeli.c_str());
        int jual = atoi(hargaJual.c_str());

        int margin = jual - beli;

        string status;

        if (stokInt < minInt)
            status = "RESTOCK";
        else if (stokInt == minInt)
            status = "MENIPIS";
        else
            status = "AMAN";

cout << left
     << setw(5)  << no++
     << setw(10) << kode
     << setw(30) << nama
     << setw(18) << brand
     << setw(16) << kategori

     << right
     << setw(16) << formatRupiah(stoi(hargaBeli))
     << setw(16) << formatRupiah(stoi(hargaJual))
     << setw(16) << formatRupiah(margin)

     << setw(8)  << stok
     << setw(12) << minStok

     << "  "          // <-- tambahkan 2 spasi

     << left
     << setw(12) << status
     << setw(15) << expired
     << endl;
    }

    cout << "===================================================================================================================================\n";

    fin.close();
}

      void tambahProduk() {

    // Generate kode otomatis
    int nomor = 1;
    ifstream fin(PRODUCTS_FILE.c_str());
    string line;
    while (getline(fin, line)) {
        if (!line.empty())
            nomor++;
    }
    fin.close();

    string kode = "P";
    if (nomor < 10) kode += "000";
    else if (nomor < 100) kode += "00";
    else if (nomor < 1000) kode += "0";
    kode += to_string(nomor);

    cout << "\nKode Produk : " << kode << " (otomatis)\n";

    string nama = inputLine("Nama Produk : ");

    // =======================
    // BRAND
    // =======================
    cout << "\nPilih Brand\n";
    cout << "1. Wardah\n";
    cout << "2. Emina\n";
    cout << "3. Skintific\n";
    cout << "4. Somethinc\n";
    cout << "5. Garnier\n";
    cout << "6. Azarine\n";
    cout << "7. Maybelline\n";
    cout << "8. Implora\n";
    cout << "9. Pinkflash\n";
    cout << "10. Scarlett\n";
    cout << "11. Hada Labo\n";
    cout << "12. Nivea\n";
    cout << "13. Vaseline\n";
    cout << "14. HMNS\n";
    cout << "15. Lainnya\n";

    string pilihBrand = inputLine("Pilihan : ");
    string brand;

    if (pilihBrand == "1") brand = "Wardah";
    else if (pilihBrand == "2") brand = "Emina";
    else if (pilihBrand == "3") brand = "Skintific";
    else if (pilihBrand == "4") brand = "Somethinc";
    else if (pilihBrand == "5") brand = "Garnier";
    else if (pilihBrand == "6") brand = "Azarine";
    else if (pilihBrand == "7") brand = "Maybelline";
    else if (pilihBrand == "8") brand = "Implora";
    else if (pilihBrand == "9") brand = "Pinkflash";
    else if (pilihBrand == "10") brand = "Scarlett";
    else if (pilihBrand == "11") brand = "Hada Labo";
    else if (pilihBrand == "12") brand = "Nivea";
    else if (pilihBrand == "13") brand = "Vaseline";
    else if (pilihBrand == "14") brand = "HMNS";
    else if (pilihBrand == "15")
        brand = inputLine("Masukkan Brand : ");
    else {
        cout << "Pilihan brand tidak valid.\n";
        return;
    }

    // KATEGORI
    cout << "\nPilih Kategori\n";
    cout << "1. Skincare\n";
    cout << "2. Makeup\n";
    cout << "3. Body Care\n";
    cout << "4. Hair Care\n";
    cout << "5. Fragrance\n";
    cout << "6. Beauty Tools\n";
    cout << "7. Personal Care\n";

    string pilihKategori = inputLine("Pilihan : ");
    string kategori;

    if (pilihKategori == "1") kategori = "Skincare";
    else if (pilihKategori == "2") kategori = "Makeup";
    else if (pilihKategori == "3") kategori = "Body Care";
    else if (pilihKategori == "4") kategori = "Hair Care";
    else if (pilihKategori == "5") kategori = "Fragrance";
    else if (pilihKategori == "6") kategori = "Beauty Tools";
    else if (pilihKategori == "7") kategori = "Personal Care";
    else {
        cout << "Kategori tidak valid.\n";
        return;
    }

    string hargaBeli = inputLine("Harga Beli : ");
    string hargaJual = inputLine("Harga Jual : ");
    string stok = inputLine("Stok : ");
    string minimalStok = inputLine("Minimal Stok : ");
    string expired = inputLine("Expired (YYYY-MM-DD) : ");

    ofstream fout(PRODUCTS_FILE.c_str(), ios::app);

    if (!fout) {
        cout << "Gagal menulis file produk.\n";
        return;
    }

    fout << kode << '|'
         << nama << '|'
         << brand << '|'
         << kategori << '|'
         << hargaBeli << '|'
         << hargaJual << '|'
         << stok << '|'
         << minimalStok << '|'
         << expired << '\n';

    fout.close();

    cout << "\nProduk berhasil ditambahkan.\n";
}

        void daftarKategori() {
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

    cout << "\n================ HASIL PENCARIAN ================\n";
    cout << "Keyword : " << q << "\n\n";

    string keyword = toLowerCase(q);

    cout << left
         << setw(8)  << "Kode"
         << setw(22) << "Nama"
         << setw(15) << "Brand"
         << setw(15) << "Kategori"
         << setw(12) << "Harga"
         << setw(8)  << "Stok"
         << "Expired\n";

    cout << "--------------------------------------------------------------------------\n";

while (getline(fin, line)) {

    if (line.empty())
        continue;

    size_t p1 = line.find('|');
    size_t p2 = line.find('|', p1 + 1);
    size_t p3 = line.find('|', p2 + 1);
    size_t p4 = line.find('|', p3 + 1);
    size_t p5 = line.find('|', p4 + 1);
    size_t p6 = line.find('|', p5 + 1);
    size_t p7 = line.find('|', p6 + 1);
    size_t p8 = line.find('|', p7 + 1);

    if (p1 == string::npos || p2 == string::npos || p3 == string::npos ||
        p4 == string::npos || p5 == string::npos || p6 == string::npos ||
        p7 == string::npos || p8 == string::npos)
        continue;

    string kode      = line.substr(0, p1);
    string nama      = line.substr(p1 + 1, p2 - p1 - 1);
    string brand     = line.substr(p2 + 1, p3 - p2 - 1);
    string kategori  = line.substr(p3 + 1, p4 - p3 - 1);

    string hargaJual = line.substr(p5 + 1, p6 - p5 - 1);
    string stok      = line.substr(p6 + 1, p7 - p6 - 1);
    string expired   = line.substr(p8 + 1);

    // Pencarian tanpa membedakan huruf besar/kecil
    string kodeCari      = toLowerCase(kode);
    string namaCari      = toLowerCase(nama);
    string brandCari     = toLowerCase(brand);
    string kategoriCari  = toLowerCase(kategori);

    if (kodeCari.find(keyword) != string::npos ||
        namaCari.find(keyword) != string::npos ||
        brandCari.find(keyword) != string::npos ||
        kategoriCari.find(keyword) != string::npos) {

        cout << left
             << setw(8)  << kode
             << setw(22) << nama
             << setw(15) << brand
             << setw(15) << kategori
             << setw(12) << ("Rp" + hargaJual)
             << setw(8)  << stok
             << expired
             << endl;

        ditemukan++;
    }
}

    fin.close();

    if (ditemukan == 0) {
        cout << "\nProduk tidak ditemukan.\n";
    }

    cout << "=============================================================\n";
}

bool cekExpired() {

    string d = inputLine("Tampilkan produk yang expired sampai tanggal (YYYY-MM-DD): ");

    if (d.empty()) {
        cout << "Tanggal tidak boleh kosong.\n";
        return false;
    }

    ifstream fin(PRODUCTS_FILE.c_str());

    if (!fin) {
        cout << "File produk tidak ditemukan.\n";
        return false;
    }

    bool found = false;
    string line;

    cout << "\n=========================================================================================\n";
    cout << left
        << setw(8)  << "Kode"
        << setw(25) << "Nama Produk"
        << setw(15) << "Brand"
        << setw(15) << "Kategori"
        << setw(15) << "Expired"
        << "Status\n";

    cout << "=========================================================================================\n";

    while (getline(fin, line)) {

        if (line.empty())
            continue;

        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        size_t p5 = line.find('|', p4 + 1);
        size_t p6 = line.find('|', p5 + 1);
        size_t p7 = line.find('|', p6 + 1);
        size_t p8 = line.find('|', p7 + 1);

        if (p1==string::npos || p2==string::npos || p3==string::npos ||
            p4==string::npos || p5==string::npos || p6==string::npos ||
            p7==string::npos || p8==string::npos)
            continue;

        string kode      = line.substr(0, p1);
        string nama      = line.substr(p1 + 1, p2 - p1 - 1);
        string brand     = line.substr(p2 + 1, p3 - p2 - 1);
        string kategori  = line.substr(p3 + 1, p4 - p3 - 1);
        string expired   = line.substr(p8 + 1);
        string status = statusExpired(d, expired);

        if (expired <= d) {

    cout << left
        << setw(8)  << kode
        << setw(25) << nama
        << setw(15) << brand
        << setw(15) << kategori
        << setw(15) << expired
        << status
        << endl;

            found = true;
        }
    }

    fin.close();

    if (!found) {
        cout << "Tidak ada produk yang expired sebelum atau pada tanggal tersebut.\n";
    }

    return found;
}

bool productCodeExists(const string& code) {
    ifstream fin(PRODUCTS_FILE.c_str());

    if (!fin)
        return false;

    string line;

    while (getline(fin, line)) {

        if (line.empty())
            continue;

        size_t p = line.find('|');

        if (p == string::npos)
            continue;

        if (line.substr(0, p) == code)
            return true;
    }

    return false;
} 

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

    void tambahkankategori() {
    string cat = inputLine("Nama kategori: ");
    if (cat.empty()) {
        cout << "Kategori tidak boleh kosong.\n";
        return;
    }

    if (categoryExists(cat)) {
        cout << "Kategori sudah ada.\n";
        return;
    }

    ofstream fout(CATEGORIES_FILE.c_str(), ios::app);
    if (!fout) {
        cout << "Gagal menulis file kategori.\n";
        return;
    }

    fout << cat << '\n';
    cout << "Kategori ditambahkan.\n";
}

bool categoryExists(const string& cat) {
    ifstream fin(CATEGORIES_FILE.c_str());
    if (!fin) return false;

    string line;
    while (getline(fin, line)) {
        if (line == cat)
            return true;
    }

    return false;
}

void editProduk() {
    string kode = inputLine("Masukkan kode produk yang akan diedit: ");
    if (!productCodeExists(kode)) { 
    cout << "Kode tidak ditemukan.\n"; 
    return; }
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

class Kasir {
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

        void transaksi() {
         KeranjangMenu();
    }

        void inputMember() {
        Member member;
        member.daftarMember();
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

            if (choice == "1") {
                break;
            } 
            else if (choice == "2") {
                string np = inputLine("Password baru: ");
                string nc = inputLine("Konfirmasi password baru: ");

                if (np == nc) {
                    if (changePassword(user, np))
                        cout << "Password berhasil diubah.\n";
                    else
                        cout << "Gagal mengubah password.\n";
                } 
                else {
                    cout << "Konfirmasi tidak sama.\n";
                }
            } 
            else if (choice == "3") {
                 transaksi();   
            } 
            else if (choice == "4") {
                string q = inputLine("Masukkan kata kunci (nama/kode/kategori): ");

                if (q.empty())
                    cout << "Kata kunci kosong.\n";
                else
                    produk.cariProduk(q);
            } 
            else if (choice == "5") {
                while (true) {
                    cout << "\n-- Menu Membership --\n";
                    cout << "1. Registrasi member\n";
                    cout << "2. Lihat member\n";
                    cout << "3. Lihat riwayat member\n";
                    cout << "4. Kembali\n";

                    string m = inputLine("Pilih: ");

                    if (m == "1") {
                         inputMember();
                    } 
                    else if (m == "2") {
                        member.tampilMember();
                    } 
                    else if (m == "3") {
                        string mem = inputLine("Masukkan username member: ");

                        if (mem.empty())
                            cout << "Username kosong.\n";
                        else
                            member.simpanRiwayatBelanja(mem);
                    } 
                    else if (m == "4") {
                        break;
                    } 
                    else {
                        cout << "Pilihan tidak dikenali.\n";
                    }
                }
            } 
            else if (choice == "6") {
                listTransaksi();
            } 
            else {
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

        bool promoCodeExists(const string& code) {
            ifstream fin(PROMOS_FILE.c_str()); if (!fin) return false;
            string line; while (getline(fin,line)) { if (line.empty()) continue; 
                size_t p=line.find('|'); if (p==string::npos) continue;
                string c=line.substr(0,p); if (c==code) return true;
             } 
             return false;
}

        void tampilPromo() {
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

void editPromo() {
    Promo promo;
    string code=inputLine("Kode promo yang diedit: "); if (!promo.promoCodeExists(code)) { cout<<"Kode tidak ditemukan.\n"; return; }
    ifstream fin(PROMOS_FILE.c_str()); if (!fin) { cout<<"File promo tidak ditemukan.\n"; return; }
    string all; string line; while (getline(fin,line)) { if (line.empty()) continue; size_t p=line.find('|'); if (p==string::npos) { all+=line+'\n'; continue; } string c=line.substr(0,p); if (c==code) {
            cout<<"Mengedit promo: "<<code<<"\n";
            string desc=inputLine("Deskripsi baru: "); string pcts=inputLine("Persentase baru: "); string start=inputLine("Mulai baru (YYYY-MM-DD): "); string end=inputLine("Selesai baru (YYYY-MM-DD): "); string m=inputLine("Hanya untuk member? (y/n): "); string memberOnly=(m=="y"||m=="Y")?"1":"0";
            all += code + '|' + desc + '|' + pcts + '|' + start + '|' + end + '|' + memberOnly + '\n';
        } else all += line + '\n'; }
    fin.close(); ofstream fout(PROMOS_FILE.c_str(), ios::trunc); if (!fout) { cout<<"Gagal menulis file promo.\n"; return; } fout<<all; cout<<"Promo diperbarui.\n";
}

void hapusPromo() {
    Promo promo;
    string code=inputLine("Kode promo yang dihapus: "); if (!promo.promoCodeExists(code)) { cout<<"Kode tidak ditemukan.\n"; return; }
    if (!confirmAction(string("Hapus promo '") + code + "' ?")) return;
    ifstream fin(PROMOS_FILE.c_str()); string all; string line; while (getline(fin,line)) { if (line.empty()) continue; size_t p=line.find('|'); if (p==string::npos) { all+=line+'\n'; continue; } string c=line.substr(0,p); if (c==code) continue; all+=line+'\n'; } fin.close(); ofstream fout(PROMOS_FILE.c_str(), ios::trunc); if (!fout) { cout<<"Gagal menulis file promo.\n"; return; } fout<<all; cout<<"Promo dihapus.\n";
}

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



};


class Keranjang{
public:
   
bool tambahBarang(const string& code, int qty) {
    Produk produk;

    if (qty <= 0)
        return false;

    if (!produk.productCodeExists(code))
        return false;

    ifstream fin(PRODUCTS_FILE.c_str());
    if (!fin)
        return false;

    string line;

    while (getline(fin, line)) {
        if (line.empty())
            continue;

        size_t p1 = line.find('|');
        if (p1 == string::npos)
            continue;

        string kode = line.substr(0, p1);

        if (kode != code)
            continue;

        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        size_t p5 = line.find('|', p4 + 1);
        size_t p6 = line.find('|', p5 + 1);
        size_t p7 = line.find('|', p6 + 1);
        size_t p8 = line.find('|', p7 + 1);

        if (p2 == string::npos || p3 == string::npos ||
            p4 == string::npos || p5 == string::npos ||
            p6 == string::npos || p7 == string::npos ||
            p8 == string::npos)
            continue;

        string nama = line.substr(p1 + 1, p2 - p1 - 1);
        string brand = line.substr(p2 + 1, p3 - p2 - 1);
        string kategori = line.substr(p3 + 1, p4 - p3 - 1);
        string hargaBeli = line.substr(p4 + 1, p5 - p4 - 1);
        string hargaJual = line.substr(p5 + 1, p6 - p5 - 1);
        string stok = line.substr(p6 + 1, p7 - p6 - 1);
        string minStok = line.substr(p7 + 1, p8 - p7 - 1);
        string expired = line.substr(p8 + 1);

        double price = 0.0;
        try {
            price = stod(hargaJual);
        } catch (...) {
            price = 0.0;
        }

        int stokProduk = atoi(stok.c_str());

        // Cek stok jika produk baru ditambahkan
        if (qty > stokProduk) {
            cout << "Stok tidak mencukupi.\n";
            return false;
        }

        // Jika produk sudah ada di keranjang
        for (int i = 0; i < cartCount; ++i) {
            if (cartCode[i] == code) {

                if (cartQty[i] + qty > stokProduk) {
                    cout << "Stok tidak mencukupi.\n";
                    return false;
                }

                cartQty[i] += qty;
                cartSubtotal[i] = cartQty[i] * cartPrice[i];

                return true;
            }
        }

        if (cartCount >= MAX_CART) {
            cout << "Keranjang sudah penuh.\n";
            return false;
        }

        cartCode[cartCount] = code;
        cartName[cartCount] = nama;
        cartPrice[cartCount] = price;
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

    // Jika promo kosong
    string promo = promoCode;
    if (promo.empty())
        promo = "-";

    // Jika bukan member
    string namaMember = member;
    if (namaMember.empty())
        namaMember = "Non-Member";

    // Simpan transaksi
    fout << id << '|'
         << date << '|'
         << (int)total << '|'
         << (int)discount << '|'
         << (int)finalAmount << '|'
         << (int)paid << '|'
         << (int)change << '|'
         << promo << '|'
         << namaMember << '|'
         << items
         << endl;

    fout.close();

    // Simpan riwayat belanja hanya untuk member
    if (namaMember != "Non-Member") {
        ofstream fh(MEMBER_HISTORY_FILE.c_str(), ios::app);

        if (fh) {
            fh << namaMember << '|'
               << id << '|'
               << date << '|'
               << (int)finalAmount
               << endl;

            fh.close();
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
    
bool checkoutCart(const string& curDate);

        
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
        void tambahShift() {
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
        void hitungPenjualanHarian() {

        ifstream fin(TRANSACTIONS_FILE.c_str());

        if (!fin) {
            cout << "\nBelum ada data transaksi.\n";
            return;
        }

    string tanggal = inputLine("Masukkan Tanggal (YYYY-MM-DD): ");

    string line;

    double totalRevenue = 0;
    double totalDiscount = 0;
    int txCount = 0;

    cout << "\n===================================================================================================\n";
    cout << "                               LAPORAN PENJUALAN HARIAN\n";
    cout << "===================================================================================================\n";

    cout << left
         << setw(8)  << "ID"
         << setw(15) << "Tanggal"
         << right
         << setw(18) << "Total"
         << setw(18) << "Diskon"
         << setw(18) << "Final"
         << "   Member"
         << endl;

    cout << "---------------------------------------------------------------------------------------------------\n";

    while (getline(fin, line)) {

        if (line.empty())
            continue;

        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        size_t p5 = line.find('|', p4 + 1);
        size_t p6 = line.find('|', p5 + 1);
        size_t p7 = line.find('|', p6 + 1);
        size_t p8 = line.find('|', p7 + 1);
        size_t p9 = line.find('|', p8 + 1);

        if (p1 == string::npos ||
            p2 == string::npos ||
            p3 == string::npos ||
            p4 == string::npos ||
            p5 == string::npos)
            continue;

        string id = line.substr(0, p1);

        string date = line.substr(
            p1 + 1,
            p2 - p1 - 1
        );

        if (date != tanggal)
            continue;

        int total = 0;

        for (size_t i = p2 + 1; i < p3; ++i) {
            if (line[i] >= '0' && line[i] <= '9')
                total = total * 10 + (line[i] - '0');
        }

        int diskon = 0;

        for (size_t i = p3 + 1; i < p4; ++i) {
            if (line[i] >= '0' && line[i] <= '9')
                diskon = diskon * 10 + (line[i] - '0');
        }

        int final = 0;

        for (size_t i = p4 + 1; i < p5; ++i) {
            if (line[i] >= '0' && line[i] <= '9')
                final = final * 10 + (line[i] - '0');
        }

        string member;

        if (p8 != string::npos)
            member = line.substr(p8 + 1, p9 - p8 - 1);
        else
            member = "Non-Member";

        cout << left
             << setw(8) << id
             << setw(15) << date
             << right
             << setw(18) << formatRupiah(total)
             << setw(18) << formatRupiah(diskon)
             << setw(18) << formatRupiah(final)
             << "   "
             << member
             << endl;

        totalRevenue += final;
        totalDiscount += diskon;
        txCount++;
    }

    fin.close();

    cout << "===================================================================================================\n";

    cout << "\nRINGKASAN LAPORAN\n";
    cout << "===================================================================================================\n";

    cout << "Tanggal                 : " << tanggal << endl;

    cout << "Total Transaksi         : " << txCount << endl;

    cout << "Total Diskon            : "
         << formatRupiah((int)totalDiscount)
         << endl;

    cout << "Total Pendapatan Bersih : "
         << formatRupiah((int)totalRevenue)
         << endl;

    cout << "Rata-rata Transaksi     : "
         << formatRupiah(txCount > 0 ? (int)(totalRevenue / txCount) : 0)
         << endl;

    cout << "===================================================================================================\n";
}
    void hitungPenjualanMingguan() {

    ifstream fin(TRANSACTIONS_FILE.c_str());

    if (!fin) {
        cout << "\nBelum ada data transaksi.\n";
        return;
    }

    cout << "\nCatatan: Tanggal awal minggu hanya dapat diisi dari tanggal 01-25.\n";

    string tanggalAwal = inputLine("Masukkan Tanggal Awal (YYYY-MM-DD): ");

    int hariAwal = stoi(tanggalAwal.substr(8,2));

    if (hariAwal < 1 || hariAwal > 25) {
    cout << "\nTanggal awal harus berada pada rentang 01-25.\n";
    fin.close();
    return;
}

string tanggalAkhir = tanggalAwal;

int hariAkhir = hariAwal + 6;

if (hariAkhir < 10)
    tanggalAkhir.replace(8,2,"0"+to_string(hariAkhir));
else
    tanggalAkhir.replace(8,2,to_string(hariAkhir));

    string line;

    double totalRevenue = 0;
    double totalDiscount = 0;
    int txCount = 0;

    cout << "\n===================================================================================================\n";
    cout << "                              LAPORAN PENJUALAN MINGGUAN\n";
    cout << "===================================================================================================\n";

    cout << left
         << setw(8)  << "ID"
         << setw(15) << "Tanggal"
         << right
         << setw(18) << "Total"
         << setw(18) << "Diskon"
         << setw(18) << "Final"
         << "   Member"
         << endl;

    cout << "---------------------------------------------------------------------------------------------------\n";

    while (getline(fin, line)) {

        if (line.empty())
            continue;

        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        size_t p5 = line.find('|', p4 + 1);
        size_t p6 = line.find('|', p5 + 1);
        size_t p7 = line.find('|', p6 + 1);
        size_t p8 = line.find('|', p7 + 1);
        size_t p9 = line.find('|', p8 + 1);

        if (p1 == string::npos ||
            p2 == string::npos ||
            p3 == string::npos ||
            p4 == string::npos ||
            p5 == string::npos)
            continue;

        string id = line.substr(0, p1);

        string date = line.substr(
            p1 + 1,
            p2 - p1 - 1
        );

        // Filter Mingguan
        if (date < tanggalAwal || date > tanggalAkhir)
            continue;

        int total = 0;

        for (size_t i = p2 + 1; i < p3; ++i) {
            if (line[i] >= '0' && line[i] <= '9')
                total = total * 10 + (line[i] - '0');
        }

        int diskon = 0;

        for (size_t i = p3 + 1; i < p4; ++i) {
            if (line[i] >= '0' && line[i] <= '9')
                diskon = diskon * 10 + (line[i] - '0');
        }

        int final = 0;

        for (size_t i = p4 + 1; i < p5; ++i) {
            if (line[i] >= '0' && line[i] <= '9')
                final = final * 10 + (line[i] - '0');
        }

        string member;

        if (p8 != string::npos)
            member = line.substr(p8 + 1, p9 - p8 - 1);
        else
            member = "Non-Member";

        cout << left
             << setw(8) << id
             << setw(15) << date
             << right
             << setw(18) << formatRupiah(total)
             << setw(18) << formatRupiah(diskon)
             << setw(18) << formatRupiah(final)
             << "   "
             << member
             << endl;

        totalRevenue += final;
        totalDiscount += diskon;
        txCount++;
    }

    fin.close();

        cout << "===================================================================================================\n";

        cout << "\nRINGKASAN LAPORAN\n";
        cout << "===================================================================================================\n";

        cout << "Periode                 : "
         << tanggalAwal
         << " s/d "
         << tanggalAkhir
         << endl;

        cout << "Total Transaksi         : "
         << txCount
         << endl;

        cout << "Total Diskon            : "
         << formatRupiah((int)totalDiscount)
         << endl;

        cout << "Total Pendapatan Bersih : "
         << formatRupiah((int)totalRevenue)
         << endl;

        cout << "Rata-rata Transaksi     : "
         << formatRupiah(txCount > 0 ? (int)(totalRevenue / txCount) : 0)
         << endl;

        cout << "===================================================================================================\n";
}
        void hitungPenjualanBulanan() { laporanPenjualanBulanan(); }
        void hitungPenjualanTahunan() {

    ifstream fin(TRANSACTIONS_FILE.c_str());

    if (!fin) {
        cout << "\nBelum ada data transaksi.\n";
        return;
    }

    string tahun = inputLine("Masukkan Tahun (YYYY): ");

    string line;

    double totalRevenue = 0;
    double totalDiscount = 0;
    int txCount = 0;

    cout << "\n===================================================================================================\n";
    cout << "                              LAPORAN PENJUALAN TAHUNAN\n";
    cout << "===================================================================================================\n";

    cout << left
         << setw(8)  << "ID"
         << setw(15) << "Tanggal"
         << right
         << setw(18) << "Total"
         << setw(18) << "Diskon"
         << setw(18) << "Final"
         << "   Member"
         << endl;

    cout << "---------------------------------------------------------------------------------------------------\n";

    while (getline(fin, line)) {

        if (line.empty())
            continue;

        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        size_t p5 = line.find('|', p4 + 1);
        size_t p6 = line.find('|', p5 + 1);
        size_t p7 = line.find('|', p6 + 1);
        size_t p8 = line.find('|', p7 + 1);
        size_t p9 = line.find('|', p8 + 1);

        if (p1 == string::npos ||
            p2 == string::npos ||
            p3 == string::npos ||
            p4 == string::npos ||
            p5 == string::npos)
            continue;

        string id = line.substr(0, p1);

        string date = line.substr(
            p1 + 1,
            p2 - p1 - 1
        );

        // Filter Tahun
        if (date.substr(0,4) != tahun)
            continue;

        int total = 0;

        for (size_t i = p2 + 1; i < p3; ++i) {
            if (line[i] >= '0' && line[i] <= '9')
                total = total * 10 + (line[i] - '0');
        }

        int diskon = 0;

        for (size_t i = p3 + 1; i < p4; ++i) {
            if (line[i] >= '0' && line[i] <= '9')
                diskon = diskon * 10 + (line[i] - '0');
        }

        int final = 0;

        for (size_t i = p4 + 1; i < p5; ++i) {
            if (line[i] >= '0' && line[i] <= '9')
                final = final * 10 + (line[i] - '0');
        }

        string member;

        if (p8 != string::npos)
            member = line.substr(p8 + 1, p9 - p8 - 1);
        else
            member = "Non-Member";

        cout << left
             << setw(8) << id
             << setw(15) << date
             << right
             << setw(18) << formatRupiah(total)
             << setw(18) << formatRupiah(diskon)
             << setw(18) << formatRupiah(final)
             << "   "
             << member
             << endl;

        totalRevenue += final;
        totalDiscount += diskon;
        txCount++;
    }

    fin.close();

    cout << "===================================================================================================\n";

    cout << "\nRINGKASAN LAPORAN\n";
    cout << "===================================================================================================\n";

    cout << "Tahun                    : " << tahun << endl;

    cout << "Total Transaksi          : " << txCount << endl;

    cout << "Total Diskon             : "
         << formatRupiah((int)totalDiscount)
         << endl;

    cout << "Total Pendapatan Bersih  : "
         << formatRupiah((int)totalRevenue)
         << endl;

    cout << "Rata-rata Transaksi      : "
         << formatRupiah(txCount > 0 ? (int)(totalRevenue / txCount) : 0)
         << endl;

    cout << "===================================================================================================\n";
}
        void tampilLaporan() { laporanPenjualanBulanan(); }
        void simpanLaporan() { laporanPenjualanBulanan(); }

};

class AnalisisProduk{
    private:
        string kodeProduk;
        string namaProduk;
        int jumlahTerjual;
        int stokProduk;
    public:
        void hitungBarangTerlaris() { analisisProdukTerlaris(); }
        void rankingProduk() { analisisProdukTerlaris(); }

void analisisMarginKeuntungan() {

    ifstream fprod(PRODUCTS_FILE.c_str());

    if (!fprod) {
        cout << "\nFile produk tidak ditemukan.\n";
        return;
    }

    string line;

    int totalProduk = 0;
    double totalMarginPersen = 0;
    long long totalKeuntunganStok = 0;

    int keuntunganUnitTerbesar = -1;
    int keuntunganUnitTerkecil = 999999999;

    long long keuntunganStokTerbesar = -1;
    long long keuntunganStokTerkecil = 999999999999LL;

    string produkUntungUnitTerbesar = "-";
    string produkUntungUnitTerkecil = "-";

    string produkUntungStokTerbesar = "-";
    string produkUntungStokTerkecil = "-";

    cout << "\n================================================================================================================================================\n";
    cout << "                                                       ANALISIS KEUNTUNGAN DAN MARGIN PRODUKK\n";
    cout << "=================================================================================================================================================\n";
    cout << left
        << setw(8)  << "Kode"
        << setw(25) << "Nama Produk"
        << setw(15) << "Brand"
        << right
        << setw(16) << "Harga Beli"
        << setw(16) << "Harga Jual"
        << setw(8)  << "Stok"
        << setw(20) << "Keuntungan/Unit"
        << setw(20) << "Keuntungan Stok"
        << setw(12) << "Margin %"
        << endl;

    cout << "=================================================================================================================================================\n";
    while (getline(fprod, line)) {

        if (line.empty())
            continue;

        stringstream ss(line);

        string kode;
        string nama;
        string brand;
        string kategori;
        string hargaModalStr;
        string hargaJualStr;
        string stok;
        string minimalStok;
        string expired;

        getline(ss, kode, '|');
        getline(ss, nama, '|');
        getline(ss, brand, '|');
        getline(ss, kategori, '|');
        getline(ss, hargaModalStr, '|');
        getline(ss, hargaJualStr, '|');
        getline(ss, stok, '|');
        getline(ss, minimalStok, '|');
        getline(ss, expired);

        int hargaModal = stoi(hargaModalStr);
        int hargaJual = stoi(hargaJualStr);
        int stokProduk = stoi(stok);

        int keuntunganUnit = hargaJual - hargaModal;
        long long keuntunganStok = (long long)keuntunganUnit * stokProduk;

        double marginPersen = 0.0;

        if (hargaJual != 0)
            marginPersen = (keuntunganUnit * 100.0) / hargaJual;

        totalProduk++;
        totalMarginPersen += marginPersen;
        totalKeuntunganStok += keuntunganStok;

        if (keuntunganUnit > keuntunganUnitTerbesar) {
            keuntunganUnitTerbesar = keuntunganUnit;
            produkUntungUnitTerbesar = nama;
        }

        if (keuntunganUnit < keuntunganUnitTerkecil) {
            keuntunganUnitTerkecil = keuntunganUnit;
            produkUntungUnitTerkecil = nama;
        }

        if (keuntunganStok > keuntunganStokTerbesar) {
            keuntunganStokTerbesar = keuntunganStok;
            produkUntungStokTerbesar = nama;
        }

        if (keuntunganStok < keuntunganStokTerkecil) {
            keuntunganStokTerkecil = keuntunganStok;
            produkUntungStokTerkecil = nama;
        }

    cout << left
        << setw(8)  << kode
        << setw(25) << nama
        << setw(18) << brand
        << right
        << setw(16) << formatRupiah(hargaModal)
        << setw(16) << formatRupiah(hargaJual)
        << setw(8)  << stokProduk
        << setw(20) << formatRupiah(keuntunganUnit)
        << setw(20) << formatRupiah(keuntunganStok)
        << setw(11) << fixed << setprecision(2)
        << marginPersen << "%"
        << endl;
    }

    cout << "=================================================================================================================================================\n";

    if (totalProduk > 0) {

        cout << "\n============================== RINGKASAN ANALISIS ==============================\n";

        cout << "Jumlah Produk                            : " << totalProduk << endl;

        cout << "Rata-rata Margin                         : "
             << fixed << setprecision(2)
             << (totalMarginPersen / totalProduk)
             << "%" << endl;

        cout << "Total Keuntungan Stok                    : "
             << formatRupiah(totalKeuntunganStok)
             << endl;

        cout << "Produk dengan Keuntungan/Unit Terbesar   : "
             << produkUntungUnitTerbesar
             << " (" << formatRupiah(keuntunganUnitTerbesar) << ")"
             << endl;

        cout << "Produk dengan Keuntungan/Unit Terkecil   : "
             << produkUntungUnitTerkecil
             << " (" << formatRupiah(keuntunganUnitTerkecil) << ")"
             << endl;

        cout << "Produk dengan Keuntungan Stok Terbesar   : "
             << produkUntungStokTerbesar
             << " (" << formatRupiah(keuntunganStokTerbesar) << ")"
             << endl;

        cout << "Produk dengan Keuntungan Stok Terkecil   : "
             << produkUntungStokTerkecil
             << " (" << formatRupiah(keuntunganStokTerkecil) << ")"
             << endl;

        cout << "===============================================================================\n";
    }

    fprod.close();
}

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
            KeranjangMenu();
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
            listTransaksi();
        } else if (choice == "7") {
            menuDashboardKeuangan();
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

int jumlahHariBulan(int bulan, int tahun) {

    switch (bulan) {

    case 1: case 3: case 5: case 7:
    case 8: case 10: case 12:
        return 31;

    case 4: case 6: case 9: case 11:
        return 30;

    case 2:
        if ((tahun % 400 == 0) ||
            (tahun % 4 == 0 && tahun % 100 != 0))
            return 29;
        return 28;

    default:
        return 30;
    }
}

int tanggalKeHari(string tgl) {

    int tahun = atoi(tgl.substr(0,4).c_str());
    int bulan = atoi(tgl.substr(5,2).c_str());
    int hari  = atoi(tgl.substr(8,2).c_str());

    int totalHari = 0;

    for (int t = 1; t < tahun; t++) {

        if ((t % 400 == 0) || (t % 4 == 0 && t % 100 != 0))
            totalHari += 366;
        else
            totalHari += 365;
    }

    for (int b = 1; b < bulan; b++) {
        totalHari += jumlahHariBulan(b, tahun);
    }

    totalHari += hari;

    return totalHari;
}

string statusExpired(string hariIni, string expired) {

    int sekarang = tanggalKeHari(hariIni);
    int exp = tanggalKeHari(expired);

    int selisih = exp - sekarang;

    if (selisih <= 0)
        return "EXPIRED";

    if (selisih <= 30)
        return "HAMPIR EXPIRED";

    return "AMAN";
}

string toLowerCase(string text) {
    for (size_t i = 0; i < text.length(); i++) {
        if (text[i] >= 'A' && text[i] <= 'Z') {
            text[i] = text[i] - 'A' + 'a';
        }
    }
    return text;
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
            if (password.length() < 8) {
            cout << "Password minimal 8 karakter.\n";
            continue;
}
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
            if (password.length() < 8) {
             cout << "Password minimal 8 karakter.\n";
            continue;
        }
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
    // sample products across categories
        pf << "P0001|Lipstick Red|Wardah|Makeup|50000|75000|25|5|2028-12-31\n";
        pf << "P0002|Lipstick Pink|Implora|Makeup|52000|70000|30|5|2028-12-31\n";
        pf << "P0003|Mascara Black|Maybelline|Makeup|70000|90000|15|3|2028-12-31\n";
        pf << "P0004|Eyeliner Liquid|Pinkflash|Makeup|45000|65000|18|4|2028-12-31\n";
        pf << "P0005|Face Cleanser|Hanasui|Skincare|30000|45000|40|8|2028-12-31\n";
        pf << "P0006|Moisturizer|Skintific|Skincare|90000|120000|22|5|2028-12-31\n";
        pf << "P0007|Serum Vitamin C|Somethinc|Skincare|120000|150000|12|3|2028-12-31\n";
        pf << "P0008|Facial Toner|Avoskin|Skincare|35000|50000|45|8|2028-12-31\n";
        pf << "P0009|Body Lotion|Vaseline|Bodycare|45000|60000|35|7|2028-12-31\n";
        pf << "P0010|Hand Cream|Nivea|Bodycare|25000|40000|50|10|2028-12-31\n";
        pf << "P0011|Body Scrub|Purbasari|Bodycare|35000|48000|26|5|2028-12-31\n";
        pf << "P0012|Shampoo Repair|Pantene|Haircare|40000|58000|30|6|2028-12-31\n";
        pf << "P0013|Hair Serum|L'Oreal|Haircare|65000|90000|18|4|2028-12-31\n";
        pf << "P0014|Hair Mask|Makarizo|Haircare|55000|78000|20|5|2028-12-31\n";
        pf << "P0015|Perfume Floral|HMNS|Fragrance|150000|195000|10|2|2028-12-31\n";
        pf << "P0016|Body Mist Fresh|Victoria's Secret|Fragrance|90000|120000|24|5|2028-12-31\n";
        pf << "P0017|Facial Cotton|Selection|Beauty Tools|12000|18000|80|15|2028-12-31\n";
        pf << "P0018|Makeup Sponge|Real Techniques|Beauty Tools|35000|50000|32|6|2028-12-31\n";
        pf << "P0019|Sheet Mask Aloe|Nature Republic|Personal Care|18000|25000|60|10|2028-12-31\n";
        pf << "P0020|Wet Tissue|Dettol|Personal Care|10000|15000|70|15|2028-12-31\n";
           for (int i = 21; i <= 40; i++) {

            char code[8];
            sprintf(code, "P%04d", i);

            string kode(code);
            string nama = "Sample Product " + to_string(i);

            string brand;

            switch (i % 8) {
                case 0: brand = "Wardah"; break;
                case 1: brand = "Implora"; break;
                case 2: brand = "Maybelline"; break;
                case 3: brand = "Pinkflash"; break;
                case 4: brand = "Hanasui"; break;
                case 5: brand = "Skintific"; break;
                case 6: brand = "Emina"; break;
                default: brand = "Somethinc";
        }

        string kategori;

            switch (i % 7) {
                case 0: kategori = "Makeup"; break;
                case 1: kategori = "Skincare"; break;
                case 2: kategori = "Bodycare"; break;
                case 3: kategori = "Haircare"; break;
                case 4: kategori = "Fragrance"; break;
                case 5: kategori = "Beauty Tools"; break;
                default: kategori = "Personal Care";
    }

            int hargaBeli = 20000 + (i * 1500);
            int hargaJual = hargaBeli + (10000 + (i % 5) * 5000);

            int stok = 10 + (i % 7) * 5;
            int minimalStok = 5;

        string expired = "2028-12-31";

        pf << kode << "|"
            << nama << "|"
            << brand << "|"
            << kategori << "|"
            << hargaBeli << "|"
            << hargaJual << "|"
            << stok << "|"
            << minimalStok << "|"
            << expired
            << "\n";
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

void menuproduk() {
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
             produk.tampilProdukExpired();
        }
        else if (c == "7") {
            break;
        }
        else cout << "Pilihan tidak dikenali.\n";
    }
}

void kelolaKategori() {
    Produk produk;
    while (true) {
        cout << "\n--- Menu Kategori ---\n";
        cout << "1. Tambah kategori\n";
        cout << "2. Lihat kategori\n";
        cout << "3. Kembali\n";
        cout << "Pilih: ";
        string c; getline(cin, c);
        if (c == "1") produk.tambahkankategori();
        else if (c == "2") produk.daftarKategori();
        else if (c == "3") break;
        else cout << "Pilihan tidak dikenali.\n";
    }
}

void kelolaPromo() {
    Promo promo;
    while (true) {
        cout << "\n--- Menu Promo ---\n";
        cout << "1. Tambah promo\n";
        cout << "2. Edit promo\n";
        cout << "3. Hapus promo\n";
        cout << "4. Lihat promo\n";
        cout << "5. Kembali\n";
        string c = inputLine("Pilih: ");
        if (c=="1") promo.tambahPromo();
        else if (c=="2") promo.editPromo();
        else if (c=="3") promo.hapusPromo();
        else if (c=="4") promo.tampilPromo();
        else if (c=="5") break;
        else cout<<"Pilihan tidak dikenali.\n";
    }
}

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
        size_t p6 = allProducts.find('|', p5 + 1);
        size_t p7 = allProducts.find('|', p6 + 1);
        size_t p8 = allProducts.find('|', p7 + 1);

        if (p2==string::npos || p3==string::npos ||
            p4==string::npos || p5==string::npos ||
            p6==string::npos || p7==string::npos ||
            p8==string::npos) {
            cout << "Format file produk rusak.\n";
            return false;
        }

        string stokStr = allProducts.substr(p6 + 1, p7 - p6 - 1);

        size_t nlpos = allProducts.find('\n', p8 + 1);

        string expired;

        if (nlpos == string::npos)
            expired = allProducts.substr(p8 + 1);
        else
            expired = allProducts.substr(p8 + 1, nlpos - p8 - 1);
        int stokVal = 0; for (size_t i=0;i<stokStr.size();++i) if (stokStr[i]>='0'&&stokStr[i]<='9') stokVal = stokVal*10 + (stokStr[i]-'0');
        if (!curDate.empty() && isExpired(expired, curDate)) { cout << "Produk " << code << " sudah expired/tidak layak jual.\n"; return false; }
        if (stokVal < needQty) { cout << "Stok tidak cukup untuk produk " << code << ". (butuh " << needQty << ", tersedia " << stokVal << ")\n"; return false; }
        // update stock in allProducts string
        int newStock = stokVal - needQty;
        string newStockStr = to_string(newStock);
        allProducts.replace(p6 + 1, p7 - p6 - 1, newStockStr);
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
    bool promoValid = false; 
    string appliedPromoCode;
    Promo promo;
    double promoPct = 0.0;
    if (!promoIn.empty()) {
        promoPct = promo.applyPromo(promoIn, curDate, isMember, promoValid, appliedPromoCode);
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
    cout << "\n=========================================\n";
    cout << "               TOKO KOSMETIK\n";
    cout << " Barang yang sudah dibeli tidak dapat\n";
    cout << "         dikembalikan\n";
    cout << "=========================================\n";

    cout << "Sleman, DI Yogyakarta\n";
    cout << "Indonesia\n";
    cout << "Telp : 081391391313\n";
    cout << "-----------------------------------------\n";

    cout << "Tanggal : " << curDate << endl;
    cout << "Kasir   : Admin" << endl;
    cout << "-----------------------------------------\n";

    cout << "Qty  Nama Barang             Subtotal\n";
    cout << "-----------------------------------------\n";

    for (int i = 0; i < cartCount; i++)
{
        cout << cartQty[i] << "    "
         << cartName[i]
         << "    Rp"
         << (int)cartSubtotal[i]
         << endl;
}

    cout << "-----------------------------------------\n";
    cout << "Subtotal      : Rp" << (int)total << endl;
    cout << "Diskon        : Rp" << (int)totalDiscount << endl;
    cout << "Grand Total   : Rp" << (int)finalTotal << endl;
    cout << "Tunai         : Rp" << (int)paid << endl;
    cout << "Kembalian     : Rp" << (int)change << endl;

    if (!member.empty())
    cout << "Member        : " << member << endl;

    if (!appliedPromoCode.empty())
    cout << "Promo         : " << appliedPromoCode << endl;

    cout << "=========================================\n";
    cout << "      TERIMA KASIH TELAH BERBELANJA\n";
    cout << "=========================================\n";

    krj.kosongkanKeranjang();
    return true;
}

// Transaction history
void listTransaksi() {

    ifstream fin(TRANSACTIONS_FILE.c_str());

    if (!fin) {
        cout << "Belum ada transaksi.\n";
        return;
    }

    string line;

    cout << "\nDaftar Transaksi:\n";

    while (getline(fin, line)) {

        if (line.empty())
            continue;

        // Format:
        // id|date|total|discount|final|paid|change|promo|member|items

        size_t p1 = line.find('|');
        if (p1 == string::npos)
            continue;

        size_t p2 = line.find('|', p1 + 1);
        if (p2 == string::npos)
            continue;

        size_t p3 = line.find('|', p2 + 1);
        if (p3 == string::npos)
            continue;

        size_t p4 = line.find('|', p3 + 1);
        if (p4 == string::npos)
            continue;

        size_t p5 = line.find('|', p4 + 1);
        if (p5 == string::npos)
            continue;

        size_t p6 = line.find('|', p5 + 1);
        if (p6 == string::npos)
            continue;

        size_t p7 = line.find('|', p6 + 1);
        if (p7 == string::npos)
            continue;

        size_t p8 = line.find('|', p7 + 1);
        if (p8 == string::npos)
            continue;

        size_t p9 = line.find('|', p8 + 1);

        string id = line.substr(0, p1);
        string date = line.substr(p1 + 1, p2 - p1 - 1);
        string finalAmt = line.substr(p4 + 1, p5 - p4 - 1);

        string member = (p8 == string::npos)
                            ? string("")
                            : line.substr(p7 + 1, p8 - p7 - 1);

        cout << "- ID: " << id
             << " | " << date
             << " | Rp" << finalAmt
             << " | Member: " << member
             << endl;
    }

    fin.close();

    // Lihat detail transaksi
    string r = inputLine(
        "Lihat detail transaksi ID (masukkan nomor) atau kosong untuk kembali: ");

    if (r.empty())
        return;

    int id = 0;

    for (size_t i = 0; i < r.size(); ++i) {
        if (r[i] >= '0' && r[i] <= '9')
            id = id * 10 + (r[i] - '0');
    }

    if (id > 0) {
        viewTransactionDetails(id);
    }
}

void viewTransactionDetails(int id) {

    ifstream fin(TRANSACTIONS_FILE.c_str());

    if (!fin) {
        cout << "Belum ada transaksi.\n";
        return;
    }

    string line;
    bool found = false;

    while (getline(fin, line)) {

        if (line.empty())
            continue;

        size_t p1 = line.find('|');

        if (p1 == string::npos)
            continue;

        string sid = line.substr(0, p1);

        int iid = 0;

        for (size_t i = 0; i < sid.size(); ++i) {
            if (sid[i] >= '0' && sid[i] <= '9')
                iid = iid * 10 + (sid[i] - '0');
        }

        if (iid != id)
            continue;

        found = true;

        // Parse data transaksi
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        size_t p5 = line.find('|', p4 + 1);
        size_t p6 = line.find('|', p5 + 1);
        size_t p7 = line.find('|', p6 + 1);
        size_t p8 = line.find('|', p7 + 1);
        size_t p9 = line.find('|', p8 + 1);

        string date = (p2 == string::npos)
                        ? string("")
                        : line.substr(p1 + 1, p2 - p1 - 1);

        string total = (p3 == string::npos)
                        ? string("")
                        : line.substr(p2 + 1, p3 - p2 - 1);

        string discount = (p4 == string::npos)
                        ? string("")
                        : line.substr(p3 + 1, p4 - p3 - 1);

        string finalAmt = (p5 == string::npos)
                        ? string("")
                        : line.substr(p4 + 1, p5 - p4 - 1);

        string paid = (p6 == string::npos)
                        ? string("")
                        : line.substr(p5 + 1, p6 - p5 - 1);

        string change = (p7 == string::npos)
                        ? string("")
                        : line.substr(p6 + 1, p7 - p6 - 1);

        string promo = (p8 == string::npos)
                        ? string("")
                        : line.substr(p7 + 1, p8 - p7 - 1);

        string member = (p9 == string::npos)
                        ? string("")
                        : line.substr(p8 + 1, p9 - p8 - 1);

        string items = (p9 == string::npos)
                        ? string("")
                        : line.substr(p9 + 1);

        // =============================
        // CETAK STRUK
        // =============================

        cout << "\n=========================================================\n";
        cout << "                 TOKO KOSMETIK TPalPro\n";
        cout << "=========================================================\n";
        cout << "Alamat       : Jl. Contoh No.1\n";
        cout << "ID Transaksi : " << id << endl;
        cout << "Tanggal      : " << date << endl;
        cout << "Member       : " << member << endl;
        cout << "=========================================================\n";

        cout << left
             << setw(15) << "Kode"
             << setw(10) << "Qty"
             << setw(15) << "Harga"
             << endl;

        cout << "---------------------------------------------------------\n";

        // Format item:
        // kode:qty:harga,kode2:qty:harga,...

        size_t pos = 0;

        while (pos < items.size()) {

            size_t comma = items.find(',', pos);

            string chunk = (comma == string::npos)
                            ? items.substr(pos)
                            : items.substr(pos, comma - pos);

            size_t a = chunk.find(':');

            size_t b = (a == string::npos)
                        ? string::npos
                        : chunk.find(':', a + 1);

            string code = (a == string::npos)
                            ? chunk
                            : chunk.substr(0, a);

            string qty = (a == string::npos || b == string::npos)
                            ? string("")
                            : chunk.substr(a + 1, b - a - 1);

            string price = (b == string::npos)
                            ? string("")
                            : chunk.substr(b + 1);

            cout << left
                 << setw(15) << code
                 << setw(10) << qty
                 << ("Rp" + price)
                 << endl;

            if (comma == string::npos)
                break;

            pos = comma + 1;
        }

        cout << "=========================================================\n";

        cout << "Subtotal      : Rp" << total << endl;
        cout << "Diskon        : Rp" << discount << endl;
        cout << "Total Bayar   : Rp" << finalAmt << endl;
        cout << "Tunai         : Rp" << paid << endl;
        cout << "Kembalian     : Rp" << change << endl;
        cout << "Promo         : " << promo << endl;

        cout << "=========================================================\n";
        cout << "          Terima kasih telah berbelanja.\n";
        cout << "=========================================================\n";

        string pr = inputLine("Cetak struk ke file? (y/n): ");

        Struk struk;

        if (pr == "y" || pr == "Y") {
            struk.cetakKeFile(id);
        }

        break;
    }

    fin.close();

    if (!found) {
        cout << "Transaksi tidak ditemukan.\n";
    }
}

void searchTransactions(const string& q) {

    ifstream fin(TRANSACTIONS_FILE.c_str());

    if (!fin) {
        cout << "Belum ada transaksi.\n";
        return;
    }

    string line;
    int found = 0;

    cout << "\n=====================================================\n";
    cout << "           HASIL PENCARIAN TRANSAKSI\n";
    cout << "=====================================================\n";
    cout << "Kata Kunci : " << q << endl;
    cout << "-----------------------------------------------------\n";

    while (getline(fin, line)) {

        if (line.empty())
            continue;

        if (line.find(q) != string::npos) {

            // Format:
            // id|date|total|discount|final|paid|change|promo|member|items

            size_t p1 = line.find('|');
            size_t p2 = line.find('|', p1 + 1);
            size_t p3 = line.find('|', p2 + 1);
            size_t p4 = line.find('|', p3 + 1);
            size_t p5 = line.find('|', p4 + 1);

            string id = line.substr(0, p1);
            string date = line.substr(p1 + 1, p2 - p1 - 1);

            string finalAmt = (p5 == string::npos)
                                ? string("")
                                : line.substr(p4 + 1, p5 - p4 - 1);

            cout << "ID Transaksi : " << id << endl;
            cout << "Tanggal      : " << date << endl;
            cout << "Total Bayar  : Rp" << finalAmt << endl;
            cout << "-----------------------------------------------------\n";

            found++;
        }
    }

    if (!found) {
        cout << "Tidak ditemukan riwayat yang cocok.\n";
    }

    cout << "=====================================================\n";

    fin.close();
}

void KeranjangMenu() {

    Keranjang krj;
    Produk produk;

    while (true) {

        cout << "\n==============================\n";
        cout << "     MENU KERANJANG BELANJA\n";
        cout << "==============================\n";
        cout << "1. Tambah Barang ke Keranjang\n";
        cout << "2. Hapus Barang dari Keranjang\n";
        cout << "3. Ubah Jumlah Barang\n";
        cout << "4. Lihat Isi Keranjang\n";
        cout << "5. Checkout\n";
        cout << "6. Kosongkan Keranjang\n";
        cout << "7. Kembali\n";
        cout << "==============================\n";
        cout << "Pilih Menu : ";

        string ch;
        getline(cin, ch);

        if (ch == "1") {

            cout << "\nDaftar Produk Tersedia\n";
            produk.tampilProduk();

            string code = inputLine("Kode Produk : ");
            string q = inputLine("Jumlah      : ");

            int qty = 0;

            for (size_t i = 0; i < q.size(); ++i) {
                if (q[i] >= '0' && q[i] <= '9')
                    qty = qty * 10 + (q[i] - '0');
            }

            if (qty <= 0) {
                cout << "Jumlah tidak valid.\n";
                continue;
            }

            if (krj.tambahBarang(code, qty))
                cout << "Barang berhasil ditambahkan ke keranjang.\n";
            else
                cout << "Gagal menambahkan barang (cek kode atau stok).\n";
        }

        else if (ch == "2") {

            krj.tampilKeranjang();

            string idxs = inputLine("Nomor Item : ");

            int idx = 0;

            for (size_t i = 0; i < idxs.size(); ++i) {
                if (idxs[i] >= '0' && idxs[i] <= '9')
                    idx = idx * 10 + (idxs[i] - '0');
            }

            if (krj.hapusBarang(idx))
                cout << "Item berhasil dihapus.\n";
            else
                cout << "Gagal menghapus item.\n";
        }

        else if (ch == "3") {

            krj.tampilKeranjang();

            string idxs = inputLine("Nomor Item  : ");

            int idx = 0;

            for (size_t i = 0; i < idxs.size(); ++i) {
                if (idxs[i] >= '0' && idxs[i] <= '9')
                    idx = idx * 10 + (idxs[i] - '0');
            }

            string q = inputLine("Jumlah Baru : ");

            int qty = 0;

            for (size_t i = 0; i < q.size(); ++i) {
                if (q[i] >= '0' && q[i] <= '9')
                    qty = qty * 10 + (q[i] - '0');
            }

            if (krj.ubahJumlah(idx, qty))
                cout << "Jumlah berhasil diperbarui.\n";
            else
                cout << "Gagal memperbarui jumlah.\n";
        }

        else if (ch == "4") {

            krj.tampilKeranjang();

        }

        else if (ch == "5") {

            string date = inputLine("Tanggal Transaksi (YYYY-MM-DD): ");

            if (checkoutCart(date))
                cout << "Transaksi berhasil disimpan.\n";
            else
                cout << "Checkout gagal.\n";
        }

        else if (ch == "6") {

            krj.kosongkanKeranjang();
            cout << "Keranjang berhasil dikosongkan.\n";

        }

        else if (ch == "7") {

            break;

        }

        else {

            cout << "Pilihan tidak dikenali.\n";

        }
    }
}

// shifts
void listShifts() {

    ifstream fin(SHIFTS_FILE.c_str());

    if (!fin) {
        cout << "Belum ada jadwal shift.\n";
        return;
    }

    string line;

    cout << "\n========================================================\n";
    cout << "                 DAFTAR JADWAL SHIFT\n";
    cout << "========================================================\n";

    cout << left
         << setw(8)  << "ID"
         << setw(15) << "Tanggal"
         << setw(15) << "Shift"
         << setw(20) << "Karyawan"
         << endl;

    cout << "--------------------------------------------------------\n";

    while (getline(fin, line)) {

        if (line.empty())
            continue;

        // Format: id|date|shift|employee
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);

        if (p1 == string::npos ||
            p2 == string::npos ||
            p3 == string::npos)
            continue;

        string id = line.substr(0, p1);

        string date = line.substr(
            p1 + 1,
            p2 - p1 - 1
        );

        string shift = line.substr(
            p2 + 1,
            p3 - p2 - 1
        );

        string emp = line.substr(p3 + 1);

        cout << left
             << setw(8)  << id
             << setw(15) << date
             << setw(15) << shift
             << setw(20) << emp
             << endl;
    }

    cout << "========================================================\n";

    fin.close();
}


void kelolaShift() {
    Shift shift;

    while (true) {
        cout << "\n========== MENU JADWAL SHIFT ==========\n";
        cout << "1. Tambah Jadwal Shift\n";
        cout << "2. Lihat Jadwal Shift\n";
        cout << "3. Edit Jadwal Shift\n";
        cout << "4. Hapus Jadwal Shift\n";
        cout << "5. Kembali\n";
        cout << "=======================================\n";

        string pilihan = inputLine("Pilih menu: ");

        if (pilihan == "1") {
            shift.tambahShift();
        }
        else if (pilihan == "2") {
            shift.tampilShift();
        }
        else if (pilihan == "3") {
            shift.editShift();
        }
        else if (pilihan == "4") {
            shift.hapusShift();
        }
        else if (pilihan == "5") {
            break;
        }
        else {
            cout << "Pilihan tidak valid.\n";
        }
    }
}

void editShift() {

    string rid = inputLine("Masukkan ID jadwal yang akan diedit: ");

    if (rid.empty()) {
        cout << "ID kosong.\n";
        return;
    }

    int id = 0;

    for (size_t i = 0; i < rid.size(); ++i) {
        if (rid[i] >= '0' && rid[i] <= '9')
            id = id * 10 + (rid[i] - '0');
    }

    if (id == 0) {
        cout << "ID tidak valid.\n";
        return;
    }

    ifstream fin(SHIFTS_FILE.c_str());

    if (!fin) {
        cout << "File shift tidak ditemukan.\n";
        return;
    }

    string all;
    string line;
    bool found = false;

    while (getline(fin, line)) {

        if (line.empty())
            continue;

        size_t p1 = line.find('|');

        if (p1 == string::npos) {
            all += line + '\n';
            continue;
        }

        int cid = 0;

        for (size_t i = 0; i < p1; ++i) {
            if (line[i] >= '0' && line[i] <= '9')
                cid = cid * 10 + (line[i] - '0');
        }

        if (cid != id) {
            all += line + '\n';
            continue;
        }

        found = true;

        cout << "\n========================================\n";
        cout << "         EDIT JADWAL SHIFT\n";
        cout << "========================================\n";

        string date = inputLine("Tanggal Baru (YYYY-MM-DD) : ");
        string s = inputLine("Tipe Shift (1=Opening, 2=Middle, 3=Closing) : ");
        string emp = inputLine("Username Pegawai Baru : ");

        string shiftName;

        if (s == "1")
            shiftName = "Opening";
        else if (s == "2")
            shiftName = "Middle";
        else if (s == "3")
            shiftName = "Closing";
        else
            shiftName = "Opening";

        all += rid + '|' + date + '|' + shiftName + '|' + emp + '\n';
    }

    fin.close();

    if (!found) {
        cout << "Jadwal dengan ID tersebut tidak ditemukan.\n";
        return;
    }

    ofstream fout(SHIFTS_FILE.c_str(), ios::trunc);

    if (!fout) {
        cout << "Gagal menyimpan jadwal.\n";
        return;
    }

    fout << all;
    fout.close();

    cout << "\nJadwal shift berhasil diperbarui.\n";
}

void deleteShift() {

    string rid = inputLine("Masukkan ID jadwal yang akan dihapus: ");

    if (rid.empty()) {
        cout << "ID kosong.\n";
        return;
    }

    int id = 0;

    for (size_t i = 0; i < rid.size(); ++i) {
        if (rid[i] >= '0' && rid[i] <= '9')
            id = id * 10 + (rid[i] - '0');
    }

    if (id == 0) {
        cout << "ID tidak valid.\n";
        return;
    }

    if (!confirmAction(string("Konfirmasi hapus jadwal ID ") + rid + "?"))
        return;

    ifstream fin(SHIFTS_FILE.c_str());

    if (!fin) {
        cout << "File shift tidak ditemukan.\n";
        return;
    }

    string all;
    string line;
    bool found = false;

    while (getline(fin, line)) {

        if (line.empty())
            continue;

        size_t p1 = line.find('|');

        if (p1 == string::npos) {
            all += line + '\n';
            continue;
        }

        int cid = 0;

        for (size_t i = 0; i < p1; ++i) {
            if (line[i] >= '0' && line[i] <= '9')
                cid = cid * 10 + (line[i] - '0');
        }

        if (cid == id) {
            found = true;
            continue;
        }

        all += line + '\n';
    }

    fin.close();

    if (!found) {
        cout << "Jadwal tidak ditemukan.\n";
        return;
    }

    ofstream fout(SHIFTS_FILE.c_str(), ios::trunc);

    if (!fout) {
        cout << "Gagal menyimpan file shift.\n";
        return;
    }

    fout << all;
    fout.close();

    cout << "\nJadwal shift berhasil dihapus.\n";
}

// attendance: attendance format id|date|username|status
void attendanceMenuForUser(const string& username) {

    while (true) {

        cout << "\n=========================================\n";
        cout << "          MENU PRESENSI PEGAWAI\n";
        cout << "=========================================\n";
        cout << "Username : " << username << endl;
        cout << "-----------------------------------------\n";
        cout << "1. Input Presensi Hari Ini\n";
        cout << "2. Lihat Riwayat Presensi\n";
        cout << "3. Kembali\n";
        cout << "=========================================\n";

        string c = inputLine("Pilih Menu : ");

        if (c == "1") {

            recordAttendance(username);

        }
        else if (c == "2") {

            listAttendanceForEmployee(username);

        }
        else if (c == "3") {

            break;

        }
        else {

            cout << "Pilihan tidak dikenali.\n";

        }
    }
}

void recordAttendance(const string& username) {

    string date = inputLine("Tanggal Presensi (YYYY-MM-DD): ");

    if (date.empty()) {
        cout << "Tanggal tidak boleh kosong.\n";
        return;
    }

    cout << "\n=========================================\n";
    cout << "            INPUT PRESENSI\n";
    cout << "=========================================\n";
    cout << "1. Hadir\n";
    cout << "2. Izin\n";
    cout << "3. Sakit\n";
    cout << "4. Alfa\n";
    cout << "=========================================\n";

    string s = inputLine("Pilih Status (1/2/3/4): ");

    string status;

    if (s == "1")
        status = "Hadir";
    else if (s == "2")
        status = "Izin";
    else if (s == "3")
        status = "Sakit";
    else
        status = "Alfa";

    int id = 1;

    ifstream fin(ATTENDANCE_FILE.c_str());

    string line;

    while (fin && getline(fin, line)) {

        if (!line.empty())
            id++;

    }

    fin.close();

    ofstream fout(ATTENDANCE_FILE.c_str(), ios::app);

    if (!fout) {
        cout << "Gagal menulis file presensi.\n";
        return;
    }

    fout << id
         << '|'
         << date
         << '|'
         << username
         << '|'
         << status
         << '\n';

    fout.close();

    cout << "\n=========================================\n";
    cout << "Presensi berhasil disimpan.\n";
    cout << "ID Presensi : " << id << endl;
    cout << "Tanggal     : " << date << endl;
    cout << "Username    : " << username << endl;
    cout << "Status      : " << status << endl;
    cout << "=========================================\n";
}

void listAttendance() {

    ifstream fin(ATTENDANCE_FILE.c_str());

    if (!fin) {
        cout << "Belum ada data presensi.\n";
        return;
    }

    string line;

    cout << "\n==============================================================\n";
    cout << "                    DAFTAR DATA PRESENSI\n";
    cout << "==============================================================\n";

    cout << left
         << setw(8)  << "ID"
         << setw(15) << "Tanggal"
         << setw(20) << "Username"
         << setw(15) << "Status"
         << endl;

    cout << "--------------------------------------------------------------\n";

    int totalPresensi = 0;

    while (getline(fin, line)) {

        if (line.empty())
            continue;

        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);

        if (p1 == string::npos ||
            p2 == string::npos ||
            p3 == string::npos ||
            p4 == string::npos)
            continue;

        string id = line.substr(0, p1);

        string date = line.substr(
            p1 + 1,
            p2 - p1 - 1
        );

        string user = line.substr(
            p2 + 1,
            p3 - p2 - 1
        );

        string status = line.substr(p3 + 1);

        cout << left
             << setw(8)  << id
             << setw(15) << date
             << setw(20) << user
             << setw(15) << status
             << endl;

        totalPresensi++;
    }

    cout << "==============================================================\n";
    cout << "Total Data Presensi : " << totalPresensi << endl;
    cout << "==============================================================\n";

    fin.close();
}

void listAttendanceForEmployee(const string& username) {

    if (username.empty()) {
        listAttendance();
        return;
    }

    ifstream fin(ATTENDANCE_FILE.c_str());

    if (!fin) {
        cout << "Belum ada data presensi.\n";
        return;
    }

    string line;
    int found = 0;

    cout << "\n=====================================================\n";
    cout << "           RIWAYAT PRESENSI PEGAWAI\n";
    cout << "=====================================================\n";
    cout << "Username : " << username << endl;
    cout << "-----------------------------------------------------\n";

    cout << left
         << setw(15) << "Tanggal"
         << setw(15) << "Status"
         << endl;

    cout << "-----------------------------------------------------\n";

    while (getline(fin, line)) {

        if (line.empty())
            continue;

        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);

        if (p1 == string::npos ||
            p2 == string::npos ||
            p3 == string::npos ||
            p4 == string::npos)
            continue;

        string date = line.substr(
            p1 + 1,
            p2 - p1 - 1
        );

        string user = line.substr(
            p2 + 1,
            p3 - p2 - 1
        );

        string status = line.substr(p3 + 1);

        if (user == username) {

            cout << left
                 << setw(15) << date
                 << setw(15) << status
                 << endl;

            found++;
        }
    }

    cout << "-----------------------------------------------------\n";

    if (found == 0) {

        cout << "Tidak ditemukan riwayat presensi.\n";

    } else {

        cout << "Total Riwayat Presensi : "
             << found
             << endl;

    }

    cout << "=====================================================\n";

    fin.close();
}

// ======== FINANCIAL ANALYSIS FUNCTIONS ========

void laporanPenjualanBulanan() {

    ifstream fin(TRANSACTIONS_FILE.c_str());

    if (!fin) {
        cout << "\nBelum ada data transaksi.\n";
        return;
    }

    string line;
    string tahun = inputLine("Masukkan Tahun (YYYY): ");
    string bulan = inputLine("Masukkan Bulan (MM): ");

    double totalRevenue = 0;
    double totalDiscount = 0;
    int txCount = 0;

    cout << "\n===================================================================================================\n";
    cout << "                              LAPORAN PENJUALAN BULANAN\n";
    cout << "===================================================================================================\n";

    cout << left
         << setw(8)  << "ID"
         << setw(15) << "Tanggal"
         << right
         << setw(18) << "Total"
         << setw(18) << "Diskon"
         << setw(18) << "Final"
         << "   Member"
         << endl;

    cout << "---------------------------------------------------------------------------------------------------\n";

    while (getline(fin, line)) {

        if (line.empty())
            continue;

        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        size_t p5 = line.find('|', p4 + 1);
        size_t p6 = line.find('|', p5 + 1);
        size_t p7 = line.find('|', p6 + 1);
        size_t p8 = line.find('|', p7 + 1);
        size_t p9 = line.find('|', p8 + 1);

        if (p1 == string::npos ||
            p2 == string::npos ||
            p3 == string::npos ||
            p4 == string::npos ||
            p5 == string::npos)
            continue;

        string id = line.substr(0, p1);

        string date = line.substr(
            p1 + 1,
            p2 - p1 - 1
        );

        // Filter Bulan & Tahun
        string tahunTransaksi = date.substr(0,4);
        string bulanTransaksi = date.substr(5,2);

        if (tahunTransaksi != tahun || bulanTransaksi != bulan)
            continue;

        int total = 0;

        for (size_t i = p2 + 1; i < p3; ++i) {
            if (line[i] >= '0' && line[i] <= '9')
                total = total * 10 + (line[i] - '0');
        }

        int diskon = 0;

        for (size_t i = p3 + 1; i < p4; ++i) {
            if (line[i] >= '0' && line[i] <= '9')
                diskon = diskon * 10 + (line[i] - '0');
        }

        int final = 0;

        for (size_t i = p4 + 1; i < p5; ++i) {
            if (line[i] >= '0' && line[i] <= '9')
                final = final * 10 + (line[i] - '0');
        }

        string member;

        if (p8 != string::npos)
            member = line.substr(p8 + 1, p9 - p8 - 1);
        else
            member = "Non-Member";

        cout << left
             << setw(8) << id
             << setw(15) << date
             << right
             << setw(18) << formatRupiah(total)
             << setw(18) << formatRupiah(diskon)
             << setw(18) << formatRupiah(final)
             << "   "
             << member
             << endl;

        totalRevenue += final;
        totalDiscount += diskon;
        txCount++;
    }

    fin.close();

    cout << "===================================================================================================\n";

    cout << "\nRINGKASAN LAPORAN\n";
    cout << "===================================================================================================\n";
    
    cout << "Periode                : "
    << bulan << "-" << tahun 
    << endl;

    cout << "Total Transaksi           : " << txCount << endl;

    cout << "Total Diskon             : "
         << formatRupiah((int)totalDiscount)
         << endl;

    cout << "Total Pendapatan Bersih  : "
         << formatRupiah((int)totalRevenue)
         << endl;

    cout << "Rata-rata Transaksi      : "
         << formatRupiah(txCount > 0 ? (int)(totalRevenue / txCount) : 0)
         << endl;

    cout << "===================================================================================================\n";
}

void analisisProdukTerlaris() {

    ifstream fprod(PRODUCTS_FILE.c_str());

    if (!fprod) {
        cout << "\nFile produk tidak ditemukan.\n";
        return;
    }

    ifstream fin(TRANSACTIONS_FILE.c_str());

    if (!fin) {
        cout << "\nBelum ada data transaksi.\n";
        return;
    }

    // ============================
    // Membaca Data Produk
    // ============================

    StringStringEntry productName[MAX_ENTRIES];
    int productNameSize = 0;

    string line;

    while (getline(fprod, line)) {

        if (line.empty())
            continue;

        stringstream ss(line);

        string kode;
        string nama;
        string kategori;
        string hargaModal;
        string hargaJual;
        string stok;
        string expired;

        getline(ss, kode, '|');
        getline(ss, nama, '|');
        getline(ss, kategori, '|');
        getline(ss, hargaModal, '|');
        getline(ss, hargaJual, '|');
        getline(ss, stok, '|');
        getline(ss, expired);

        addStringStringEntry(
            productName,
            productNameSize,
            kode,
            nama
        );
    }

    fprod.close();

    // ============================
    // Data Hasil Analisis
    // ============================

    StringIntEntry qtySold[MAX_ENTRIES];
    int qtySoldSize = 0;

    StringDoubleEntry revenueSold[MAX_ENTRIES];
    int revenueSoldSize = 0;

    int totalBarangTerjual = 0;
    double totalPendapatan = 0;

    // ============================
    // Membaca Seluruh Transaksi
    // ============================

    while (getline(fin, line)) {

        if (line.empty())
            continue;

        size_t pos = 0;

        for (int i = 0; i < 9; i++) {

            pos = line.find('|', pos);

            if (pos == string::npos)
                break;

            pos++;
        }

        if (pos == string::npos)
            continue;

        string items = line.substr(pos);

        stringstream itemStream(items);

        string item;

                while (getline(itemStream, item, ',')) {

            if (item.empty())
                continue;

            stringstream ss(item);

            string kodeProduk;
            string qtyStr;
            string hargaStr;

            getline(ss, kodeProduk, ':');
            getline(ss, qtyStr, ':');
            getline(ss, hargaStr);

            if (kodeProduk.empty() ||
                qtyStr.empty() ||
                hargaStr.empty())
                continue;

            int qty = stoi(qtyStr);
            double harga = stod(hargaStr);

            addStringIntEntry(
                qtySold,
                qtySoldSize,
                kodeProduk,
                qty
            );

            addStringDoubleEntry(
                revenueSold,
                revenueSoldSize,
                kodeProduk,
                qty * harga
            );

            totalBarangTerjual += qty;
            totalPendapatan += qty * harga;
        }
    }

    fin.close();

    // ============================
    // Tidak ada data penjualan
    // ============================

    if (qtySoldSize == 0) {

        cout << "\nBelum ada data penjualan.\n";
        return;
    }
        // ============================
    // Sorting Produk Terlaris
    // ============================
for (int i = 0; i < qtySoldSize - 1; i++) {

    for (int j = i + 1; j < qtySoldSize; j++) {

        if (qtySold[j].value > qtySold[i].value) {

            StringIntEntry tempQty = qtySold[i];
            qtySold[i] = qtySold[j];
            qtySold[j] = tempQty;
        }
    }
}

    // ============================
    // Header Laporan
    // ============================

    cout << "\n=========================================================================================================\n";
    cout << "                                  ANALISIS PRODUK TERLARIS\n";
    cout << "=========================================================================================================\n";

    cout << left
         << setw(5)  << "No"
         << setw(10) << "Kode"
         << setw(30) << "Nama Produk"
         << right
         << setw(15) << "Terjual"
         << setw(22) << "Pendapatan"
         << endl;

    cout << "---------------------------------------------------------------------------------------------------------\n";

        string produkTerlaris = "-";
    string produkTersedikit = "-";

    int jumlahTerlaris = -1;
    int jumlahTersedikit = 999999999;

    for (int i = 0; i < qtySoldSize; i++) {

        string namaProduk = "-";

        int idxNama = findStringStringEntry(
            productName,
            productNameSize,
            qtySold[i].key
        );

        if (idxNama >= 0)
            namaProduk = productName[idxNama].value;

        double pendapatan = 0;

        int idxRevenue = findStringDoubleEntry(
            revenueSold,
            revenueSoldSize,
            qtySold[i].key
        );

        if (idxRevenue >= 0)
            pendapatan = revenueSold[idxRevenue].value;

        if (qtySold[i].value > jumlahTerlaris) {
            jumlahTerlaris = qtySold[i].value;
            produkTerlaris = namaProduk;
        }

        if (qtySold[i].value < jumlahTersedikit) {
            jumlahTersedikit = qtySold[i].value;
            produkTersedikit = namaProduk;
        }

        cout << left
             << setw(5) << i + 1
             << setw(10) << qtySold[i].key
             << setw(30) << namaProduk
             << right
             << setw(15) << (to_string(qtySold[i].value) + " pcs")
             << setw(22) << formatRupiah((int)pendapatan)
             << endl;
    }

    cout << "---------------------------------------------------------------------------------------------------------\n";

        cout << "\nRINGKASAN ANALISIS\n";
    cout << "=========================================================================================================\n";

    cout << left
         << setw(35) << "Total Jenis Produk"
         << ": "
         << qtySoldSize
         << endl;

    cout << left
         << setw(35) << "Total Barang Terjual"
         << ": "
         << totalBarangTerjual
         << " pcs"
         << endl;

    cout << left
         << setw(35) << "Total Pendapatan"
         << ": "
         << formatRupiah((int)totalPendapatan)
         << endl;

    cout << left
         << setw(35) << "Produk Terlaris"
         << ": "
         << produkTerlaris
         << " ("
         << jumlahTerlaris
         << " pcs)"
         << endl;

    cout << left
         << setw(35) << "Produk Penjualan Terendah"
         << ": "
         << produkTersedikit
         << " ("
         << jumlahTersedikit
         << " pcs)"
         << endl;

    double rataRataPenjualan = 0;

    if (qtySoldSize > 0)
        rataRataPenjualan =
            (double) totalBarangTerjual / qtySoldSize;

    cout << left
         << setw(35) << "Rata-rata Penjualan"
         << ": "
         << fixed
         << setprecision(2)
         << rataRataPenjualan
         << " pcs / produk"
         << endl;

    cout << "=========================================================================================================\n";
}

void analisisPenjualanPerKategori() {

    ifstream fprod(PRODUCTS_FILE.c_str());

    if (!fprod) {
        cout << "\nFile produk tidak ditemukan.\n";
        return;
    }

    ifstream fin(TRANSACTIONS_FILE.c_str());

    if (!fin) {
        cout << "\nBelum ada data transaksi.\n";
        return;
    }

    // Membaca data kategori produk
    StringStringEntry productCategory[MAX_ENTRIES];
    int productCategorySize = 0;

    string line;

    while (getline(fprod, line)) {

        if (line.empty())
            continue;

        stringstream ss(line);

        string kode;
        string nama;
        string brand;
        string kategori;
        string hargaBeli;
        string hargaJual;
        string stok;
        string minimalStok;
        string expired;

        getline(ss, kode, '|');
        getline(ss, nama, '|');
        getline(ss, brand, '|');
        getline(ss, kategori, '|');
        getline(ss, hargaBeli, '|');
        getline(ss, hargaJual, '|');
        getline(ss, stok, '|');
        getline(ss, minimalStok, '|');
        getline(ss, expired);

        addStringStringEntry(
            productCategory,
            productCategorySize,
            kode,
            kategori
        );
    }

    fprod.close();

    // Analisis penjualan
    StringDoubleEntry catRevenue[MAX_ENTRIES];
    int catRevenueSize = 0;

    StringIntEntry catQuantity[MAX_ENTRIES];
    int catQuantitySize = 0;

    while (getline(fin, line)) {

        if (line.empty())
            continue;

        size_t pos = 0;

        for (int i = 0; i < 9; i++) {

            pos = line.find('|', pos);

            if (pos == string::npos)
                break;

            pos++;
        }

        if (pos == string::npos)
            continue;

        string items = line.substr(pos);

        stringstream itemStream(items);

        string item;

        while (getline(itemStream, item, ',')) {

            stringstream ss(item);

            string kodeProduk;
            string qtyStr;
            string hargaStr;

            getline(ss, kodeProduk, ':');
            getline(ss, qtyStr, ':');
            getline(ss, hargaStr);

            if (kodeProduk.empty() ||
                qtyStr.empty() ||
                hargaStr.empty())
                continue;

            int qty = stoi(qtyStr);
            double harga = stod(hargaStr);

            int idx = findStringStringEntry(
                productCategory,
                productCategorySize,
                kodeProduk
            );

            if (idx >= 0) {

                string kategori = productCategory[idx].value;

                addStringIntEntry(
                    catQuantity,
                    catQuantitySize,
                    kategori,
                    qty
                );

                addStringDoubleEntry(
                    catRevenue,
                    catRevenueSize,
                    kategori,
                    qty * harga
                );
            }
        }
    }

    fin.close();

    // Hitung Total Pendapatan
    double totalPendapatan = 0;

    for (int i = 0; i < catRevenueSize; i++) {
        totalPendapatan += catRevenue[i].value;
    }

    // Output
    cout << "\n=========================================================================================\n";
    cout << "                         ANALISIS PENJUALAN PER KATEGORI\n";
    cout << "=========================================================================================\n";

    cout << left
         << setw(20) << "Kategori"
         << setw(18) << "Item Terjual"
         << setw(20) << "Pendapatan"
         << "Kontribusi"
         << endl;

    cout << "-----------------------------------------------------------------------------------------\n";

    string kategoriTerlaris = "-";
    string kategoriTersedikit = "-";

    int itemTerbanyak = -1;
    int itemTersedikit = 999999999;

    for (int i = 0; i < catRevenueSize; i++) {

        int jumlahItem =
            getStringIntValue(
                catQuantity,
                catQuantitySize,
                catRevenue[i].key
            );

        double persen = 0;

        if (totalPendapatan != 0)
            persen =
                (catRevenue[i].value * 100.0)
                / totalPendapatan;

        if (jumlahItem > itemTerbanyak) {
            itemTerbanyak = jumlahItem;
            kategoriTerlaris = catRevenue[i].key;
        }

        if (jumlahItem < itemTersedikit) {
            itemTersedikit = jumlahItem;
            kategoriTersedikit = catRevenue[i].key;
        }

        cout << left
             << setw(20) << catRevenue[i].key
             << setw(18) << jumlahItem
             << setw(20) << formatRupiah((int)catRevenue[i].value)
             << fixed << setprecision(2)
             << persen << "%"
             << endl;
    }

    cout << "-----------------------------------------------------------------------------------------\n";

    cout << "Total Pendapatan Seluruh Kategori : "
         << formatRupiah((int)totalPendapatan)
         << endl;

    cout << "\nRINGKASAN ANALISIS\n";
    cout << "-----------------------------------------------------------------------------------------\n";

    cout << "Total Kategori              : "
         << catRevenueSize
         << endl;

    cout << "Kategori Terlaris           : "
         << kategoriTerlaris
         << " ("
         << itemTerbanyak
         << " item)"
         << endl;

    cout << "Kategori Penjualan Terendah : "
         << kategoriTersedikit
         << " ("
         << itemTersedikit
         << " item)"
         << endl;

    cout << "Total Pendapatan            : "
         << formatRupiah((int)totalPendapatan)
         << endl;

    cout << "=========================================================================================\n";
}

void laporanPendapatanPerPeriode() {

    string startDate = inputLine("Masukkan tanggal mulai (YYYY-MM-DD): ");
    string endDate   = inputLine("Masukkan tanggal akhir (YYYY-MM-DD): ");

    ifstream fin(TRANSACTIONS_FILE.c_str());

    if (!fin) {
        cout << "\nBelum ada data transaksi.\n";
        return;
    }

    string line;

    double revenue = 0;
    double discount = 0;
    int count = 0;

    int transaksiTerbesar = 0;
    int transaksiTerkecil = 999999999;

    cout << "\n=====================================================================================================\n";
    cout << "                           LAPORAN PENDAPATAN PER PERIODE\n";
    cout << "=====================================================================================================\n";

    cout << "Periode : "
         << startDate
         << " s/d "
         << endDate
         << endl;

    cout << "-----------------------------------------------------------------------------------------------------\n";

    cout << left
         << setw(8)  << "ID"
         << setw(15) << "Tanggal"
         << right
         << setw(18) << "Total"
         << setw(18) << "Diskon"
         << setw(18) << "Pendapatan"
         << endl;

    cout << "-----------------------------------------------------------------------------------------------------\n";

    while (getline(fin, line)) {

        if (line.empty())
            continue;

        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        size_t p5 = line.find('|', p4 + 1);

        if (p5 == string::npos)
            continue;

        string id = line.substr(0, p1);

        string date = line.substr(
            p1 + 1,
            p2 - p1 - 1
        );

        if (date >= startDate &&
            date <= endDate) {

            int total = 0;

            for (size_t i = p2 + 1; i < p3; i++) {

                if (line[i] >= '0' &&
                    line[i] <= '9')

                    total =
                        total * 10 +
                        (line[i] - '0');
            }

            int diskon = 0;

            for (size_t i = p3 + 1; i < p4; i++) {

                if (line[i] >= '0' &&
                    line[i] <= '9')

                    diskon =
                        diskon * 10 +
                        (line[i] - '0');
            }

            int final = 0;

            for (size_t i = p4 + 1; i < p5; i++) {

                if (line[i] >= '0' &&
                    line[i] <= '9')

                    final =
                        final * 10 +
                        (line[i] - '0');
            }

            cout << left
                 << setw(8) << id
                 << setw(15) << date
                 << right
                 << setw(18) << formatRupiah(total)
                 << setw(18) << formatRupiah(diskon)
                 << setw(18) << formatRupiah(final)
                 << endl;

            revenue += final;
            discount += diskon;
            count++;

            if (final > transaksiTerbesar)
                transaksiTerbesar = final;

            if (final < transaksiTerkecil)
                transaksiTerkecil = final;
        }
    }

    fin.close();

    cout << "=====================================================================================================\n";

    cout << "\nRINGKASAN LAPORAN\n";
    cout << "=====================================================================================================\n";

    cout << "Total Transaksi           : "
         << count
         << endl;

    cout << "Total Diskon             : "
         << formatRupiah((int)discount)
         << endl;

    cout << "Total Pendapatan         : "
         << formatRupiah((int)revenue)
         << endl;

    cout << "Rata-rata Transaksi      : "
         << formatRupiah(
                count > 0 ?
                (int)(revenue / count) :
                0
            )
         << endl;

    if (count > 0) {

        cout << "Transaksi Terbesar       : "
             << formatRupiah(transaksiTerbesar)
             << endl;

        cout << "Transaksi Terkecil       : "
             << formatRupiah(transaksiTerkecil)
             << endl;
    }

    cout << "=====================================================================================================\n";
}

void analisisPenjualanMember() {

    ifstream fin(TRANSACTIONS_FILE.c_str());

    if (!fin) {
        cout << "\nBelum ada data transaksi.\n";
        return;
    }

    string line;

    MemberSummary memberRevenue[MAX_ENTRIES];
    int memberRevenueSize = 0;

    double nonMemberRevenue = 0;
    int nonMemberCount = 0;

    while (getline(fin, line)) {

        if (line.empty())
            continue;

        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        size_t p5 = line.find('|', p4 + 1);
        size_t p6 = line.find('|', p5 + 1);
        size_t p7 = line.find('|', p6 + 1);
        size_t p8 = line.find('|', p7 + 1);
        size_t p9 = line.find('|', p8 + 1);

        if (p9 == string::npos)
            continue;

        int final = 0;

        for (size_t i = p4 + 1; i < p5; i++) {

            if (line[i] >= '0' &&
                line[i] <= '9')

                final =
                    final * 10 +
                    (line[i] - '0');
        }

        string member =
            line.substr(
                p8 + 1,
                p9 - p8 - 1
            );

        if (member.empty() ||
            member == "Non-Member") {

            nonMemberRevenue += final;
            nonMemberCount++;
        }
        else {

            int idx = -1;

            for (int i = 0; i < memberRevenueSize; i++) {

                if (memberRevenue[i].key == member) {

                    idx = i;
                    break;
                }
            }

            if (idx == -1) {

                memberRevenue[memberRevenueSize].key = member;
                memberRevenue[memberRevenueSize].revenue = final;
                memberRevenue[memberRevenueSize].count = 1;

                memberRevenueSize++;
            }
            else {

                memberRevenue[idx].revenue += final;
                memberRevenue[idx].count++;
            }
        }
    }

    fin.close();

    double totalMemberRevenue = 0;
    int totalMemberCount = 0;

    string memberTerbaik = "-";
    double revenueTerbesar = 0;

    cout << "\n====================================================================================================\n";
    cout << "                               ANALISIS PENJUALAN MEMBER\n";
    cout << "====================================================================================================\n";

    cout << left
         << setw(25) << "Nama Member"
         << setw(20) << "Transaksi"
         << setw(20) << "Pendapatan"
         << "Rata-rata"
         << endl;

    cout << "----------------------------------------------------------------------------------------------------\n";

    for (int i = 0; i < memberRevenueSize; i++) {

        double rata =
            memberRevenue[i].revenue /
            memberRevenue[i].count;

        cout << left
             << setw(25) << memberRevenue[i].key
             << setw(20) << memberRevenue[i].count
             << setw(20) << formatRupiah((int)memberRevenue[i].revenue)
             << formatRupiah((int)rata)
             << endl;

        totalMemberRevenue +=
            memberRevenue[i].revenue;

        totalMemberCount +=
            memberRevenue[i].count;

        if (memberRevenue[i].revenue >
            revenueTerbesar) {

            revenueTerbesar =
                memberRevenue[i].revenue;

            memberTerbaik =
                memberRevenue[i].key;
        }
    }

    cout << "====================================================================================================\n";

    cout << "\nRINGKASAN MEMBER\n";
    cout << "----------------------------------------------------------------------------------------------------\n";

    cout << "Jumlah Member Aktif        : "
         << memberRevenueSize
         << endl;

    cout << "Total Transaksi Member     : "
         << totalMemberCount
         << endl;

    cout << "Total Pendapatan Member    : "
         << formatRupiah((int)totalMemberRevenue)
         << endl;

    if (totalMemberCount > 0) {

        cout << "Rata-rata Belanja Member   : "
             << formatRupiah(
                    (int)(totalMemberRevenue /
                    totalMemberCount)
                )
             << endl;
    }

    cout << "Member Terbaik             : "
         << memberTerbaik
         << endl;

    cout << "\nDATA NON-MEMBER\n";
    cout << "----------------------------------------------------------------------------------------------------\n";

    cout << "Jumlah Transaksi           : "
         << nonMemberCount
         << endl;

    cout << "Pendapatan Non-Member      : "
         << formatRupiah((int)nonMemberRevenue)
         << endl;

    cout << "\nKESELURUHAN\n";
    cout << "----------------------------------------------------------------------------------------------------\n";

    cout << "Total Seluruh Transaksi    : "
         << totalMemberCount + nonMemberCount
         << endl;

    cout << "Total Seluruh Pendapatan   : "
         << formatRupiah(
                (int)(totalMemberRevenue +
                nonMemberRevenue)
            )
         << endl;

    double persenMember = 0;

    if ((totalMemberRevenue + nonMemberRevenue) != 0) {

        persenMember =
            (totalMemberRevenue * 100.0) /
            (totalMemberRevenue + nonMemberRevenue);
    }

    cout << "Kontribusi Member          : "
         << fixed
         << setprecision(2)
         << persenMember
         << "%"
         << endl;

    cout << "Kontribusi Non-Member      : "
         << fixed
         << setprecision(2)
         << (100.0 - persenMember)
         << "%"
         << endl;

    cout << "====================================================================================================\n";
}

void menuDashboardKeuangan() {

    LaporanPenjualan laporan;
    AnalisisProduk analisis;

    while (true) {

        cout << "\n============================================================\n";
        cout << "        DASHBOARD LAPORAN & ANALISIS PENJUALAN\n";
        cout << "============================================================\n";
        cout << "                 MENU LAPORAN PENJUALAN\n";
        cout << "------------------------------------------------------------\n";
        cout << "1. Laporan Penjualan Harian\n";
        cout << "2. Laporan Penjualan Mingguan\n";
        cout << "3. Laporan Penjualan Bulanan\n";
        cout << "4. Laporan Penjualan Tahunan\n";
        cout << "5. Laporan Pendapatan Per Periode\n";
        cout << "------------------------------------------------------------\n";
        cout << "                  MENU ANALISIS\n";
        cout << "------------------------------------------------------------\n";
        cout << "6. Analisis Barang Terlaris\n";
        cout << "7. Analisis Penjualan Per Kategori\n";
        cout << "8. Analisis Margin Keuntungan\n";
        cout << "9. Analisis Penjualan Member\n";
        cout << "0. Kembali\n";
        cout << "============================================================\n";

        string choice = inputLine("Pilih menu (0-9): ");

        if (choice == "1") {
            laporan.hitungPenjualanHarian();
        }
        else if (choice == "2") {
            laporan.hitungPenjualanMingguan();
        }
        else if (choice == "3") {
            laporanPenjualanBulanan();
        }
        else if (choice == "4") {
            laporan.hitungPenjualanTahunan();
        }
        else if (choice == "5") {
            laporanPendapatanPerPeriode();
        }
        else if (choice == "6") {
            analisisProdukTerlaris();
        }
        else if (choice == "7") {
            analisisPenjualanPerKategori();
        }
        else if (choice == "8") {
            analisis.analisisMarginKeuntungan();
        }
        else if (choice == "9") {
            analisisPenjualanMember();
        }
        else if (choice == "0") {
            break;
        }
        else {
            cout << "Pilihan tidak dikenali.\n";
        }
    }
}
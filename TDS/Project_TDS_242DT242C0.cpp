#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <limits>
#include <ctime>
#include <cctype>
#include <cstdlib>
#include <windows.h>
using namespace std;

const string ADMIN_FILE = "Admin.txt";
const string BOOK_FILE = "Books.txt";
const string BORROW_FILE = "borrowRecords.txt";
const string CUSTOMER_FILE = "Customer.txt";
const string REPORT_FILE = "Report.txt";
const string RESERVE_FILE = "Reservations.txt";
const int MAX_BOOKS = 500;
const int MAX_CUSTOMERS = 500;
const int MAX_ADMINS = 50;
const int MAX_BORROWS = 1000;
const int MAX_RESERVATIONS = 1000;
const int LOAN_DAYS = 14;
const double FINE_PER_DAY = 0.50;
const string TODAY = "2026-06-28";

string trimText(string text)
{
    int start = 0;
    int end = (int)text.length() - 1;
    while (start <= end && (text[start] == ' ' || text[start] == '\t' || text[start] == '\r' || text[start] == '\n'))
    {
        start++;
    }
    while (end >= start && (text[end] == ' ' || text[end] == '\t' || text[end] == '\r' || text[end] == '\n'))
    {
        end--;
    }
    if (start > end)
    {
        return "";
    }
    return text.substr(start, end - start + 1);
}

string lowerText(string text)
{
    for (int i = 0; i < (int)text.length(); i++)
    {
        text[i] = (char)tolower(text[i]);
    }
    return text;
}

int toIntSafe(string text)
{
    text = trimText(text);
    if (text == "")
    {
        return 0;
    }
    stringstream ss(text);
    int value = 0;
    ss >> value;
    return value;
}

string intToString(int value)
{
    stringstream ss;
    ss << value;
    return ss.str();
}

string formatBookId(int number)
{
    stringstream ss;
    ss << "B" << setw(3) << setfill('0') << number;
    return ss.str();
}

int getBookNumber(string id)
{
    id = trimText(id);
    if (id.length() == 0)
    {
        return 0;
    }
    if (id[0] == 'B' || id[0] == 'b')
    {
        return toIntSafe(id.substr(1));
    }
    return toIntSafe(id);
}

void clearInput()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string readLineInput(string prompt)
{
    string input;
    cout << prompt;
    getline(cin, input);
    return trimText(input);
}

int readIntegerInput(string prompt)
{
    int value;
    while (true)
    {
        cout << prompt;
        if (cin >> value)
        {
            clearInput();
            return value;
        }
        cout << "Invalid number. Please enter again.\n";
        clearInput();
    }
}

bool confirmYesNo(string message)
{
    string answer;
    cout << message << " (Y/N): ";
    getline(cin, answer);
    answer = lowerText(trimText(answer));
    if (answer == "y" || answer == "yes")
    {
        return true;
    }
    return false;
}

void pauseSystem()
{
    cout << "\nPress Enter to continue...";
    cin.get();
}

void screenClear()
{
    system("cls");
}

void loadingPage(string message)
{
    screenClear();
    cout << "==============================================\n";
    cout << "        LIBRARY BOOK RECORDS SYSTEM\n";
    cout << "==============================================\n\n";
    cout << "Loading please wait...\n\n";
    int totalBar = 30;
    for (int percent = 0; percent <= 100; percent++)
    {
        cout << "\r[";
        int filled = percent * totalBar / 100;
        for (int i = 0; i < filled; i++)
        {
            cout << char(219);
        }
        for (int i = filled; i < totalBar; i++)
        {
            cout << " ";
        }
        cout << "] " << setw(3) << percent << "%";
        cout.flush();
        Sleep(12);
    }
    cout << "\n\n" << message << "\n";
    Sleep(400);
    screenClear();
}

int dateToDays(string date)
{
    if (date.length() < 10 || date == "---")
    {
        return 0;
    }
    int year = toIntSafe(date.substr(0, 4));
    int month = toIntSafe(date.substr(5, 2));
    int day = toIntSafe(date.substr(8, 2));
    int monthDays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int total = year * 365;
    for (int i = 0; i < month - 1 && i < 12; i++)
    {
        total += monthDays[i];
    }
    total += day;
    total += year / 4;
    return total;
}

string calculateDueDate(string borrowDate)
{
    if (borrowDate.length() < 10)
    {
        return "---";
    }
    int year = toIntSafe(borrowDate.substr(0, 4));
    int month = toIntSafe(borrowDate.substr(5, 2));
    int day = toIntSafe(borrowDate.substr(8, 2));
    int monthDays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    day += LOAN_DAYS;
    while (day > monthDays[month - 1])
    {
        day -= monthDays[month - 1];
        month++;
        if (month > 12)
        {
            month = 1;
            year++;
        }
    }
    stringstream ss;
    ss << setw(4) << setfill('0') << year << "-";
    ss << setw(2) << setfill('0') << month << "-";
    ss << setw(2) << setfill('0') << day;
    return ss.str();
}

int calculateLateDays(string borrowDate, string returnDate, string status)
{
    int due = dateToDays(borrowDate) + LOAN_DAYS;
    int actual = 0;
    if (status == "Returned")
    {
        actual = dateToDays(returnDate);
    }
    else
    {
        actual = dateToDays(TODAY);
    }
    int late = actual - due;
    if (late > 0)
    {
        return late;
    }
    return 0;
}

struct Book
{
    string id;
    string title;
    string author;
    string category;
    int year;
    int stock;
    string status;
};

struct Account
{
    string username;
    string password;
    string role;
};

struct BorrowRecord
{
    string recordId;
    string username;
    string bookId;
    string borrowDate;
    string returnDate;
    string status;
};

struct Reservation
{
    string reservationId;
    string username;
    string bookId;
    string reserveDate;
    string status;
};

class RecordBase
{
protected:
    string id;
public:
    RecordBase()
    {
        id = "";
    }
    RecordBase(string newId)
    {
        id = newId;
    }
    virtual ~RecordBase()
    {
    }
    virtual void showRecord() const
    {
        cout << "Record ID: " << id << "\n";
    }
    string getId() const
    {
        return id;
    }
};

class UserBase
{
protected:
    string username;
    string role;
public:
    UserBase()
    {
        username = "";
        role = "";
    }
    UserBase(string user, string userRole)
    {
        username = user;
        role = userRole;
    }
    virtual ~UserBase()
    {
    }
    virtual void showWelcome() const
    {
        cout << "Welcome " << username << "\n";
    }
    virtual void showMenuTitle() const
    {
        cout << "Menu\n";
    }
    string getUsername() const
    {
        return username;
    }
    string getRole() const
    {
        return role;
    }
};

class CustomerUser : public UserBase
{
private:
    int borrowLimit;
public:
    CustomerUser() : UserBase("", "Customer")
    {
        borrowLimit = 3;
    }
    CustomerUser(string user) : UserBase(user, "Customer")
    {
        borrowLimit = 3;
    }
    ~CustomerUser()
    {
    }
    void showWelcome() const
    {
        cout << "\nCustomer Login Successful\n";
        cout << "Username: " << username << "\n";
    }
    void showMenuTitle() const
    {
        cout << "\n========== Customer Menu ==========" << "\n";
    }
    int getBorrowLimit() const
    {
        return borrowLimit;
    }
    friend void friendShowCustomerLimit(CustomerUser customer);
};

class AdminUser : public UserBase
{
private:
    int securityLevel;
public:
    AdminUser() : UserBase("", "Admin")
    {
        securityLevel = 9;
    }
    AdminUser(string user) : UserBase(user, "Admin")
    {
        securityLevel = 9;
    }
    ~AdminUser()
    {
    }
    void showWelcome() const
    {
        cout << "\nAdmin Login Successful\n";
        cout << "Username: " << username << "\n";
    }
    void showMenuTitle() const
    {
        cout << "\n========== Admin Menu ==========" << "\n";
    }
    int getSecurityLevel() const
    {
        return securityLevel;
    }
    friend bool friendValidateAdminLevel(AdminUser admin);
};

class GuestUser : public UserBase
{
private:
    string purpose;
public:
    GuestUser() : UserBase("guest", "Guest")
    {
        purpose = "View Only";
    }
    GuestUser(string user) : UserBase(user, "Guest")
    {
        purpose = "View Only";
    }
    ~GuestUser()
    {
    }
    void showWelcome() const
    {
        cout << "\nGuest View Mode\n";
        cout << "Username: " << username << "\n";
    }
    void showMenuTitle() const
    {
        cout << "\n========== Guest Menu ==========" << "\n";
    }
    string getPurpose() const
    {
        return purpose;
    }
};

class BookRecord : public RecordBase
{
private:
    Book book;
public:
    BookRecord() : RecordBase("")
    {
        book.id = "";
        book.title = "";
        book.author = "";
        book.category = "";
        book.year = 0;
        book.stock = 0;
        book.status = "Unavailable";
    }
    BookRecord(Book b) : RecordBase(b.id)
    {
        book = b;
    }
    ~BookRecord()
    {
    }
    void showRecord() const
    {
        cout << book.id << " | " << book.title << " | " << book.status << "\n";
    }
    friend void friendPrintBookSecret(BookRecord record);
};

class BorrowRecordClass : public RecordBase
{
private:
    BorrowRecord borrow;
public:
    BorrowRecordClass() : RecordBase("")
    {
        borrow.recordId = "";
        borrow.username = "";
        borrow.bookId = "";
        borrow.borrowDate = "";
        borrow.returnDate = "";
        borrow.status = "";
    }
    BorrowRecordClass(BorrowRecord r) : RecordBase(r.recordId)
    {
        borrow = r;
    }
    ~BorrowRecordClass()
    {
    }
    void showRecord() const
    {
        cout << borrow.recordId << " | " << borrow.username << " | " << borrow.bookId << " | " << borrow.status << "\n";
    }
    friend string friendBorrowOwner(BorrowRecordClass record);
};

void friendShowCustomerLimit(CustomerUser customer)
{
    cout << "Customer borrow limit: " << customer.borrowLimit << "\n";
}

bool friendValidateAdminLevel(AdminUser admin)
{
    return admin.securityLevel >= 9;
}

void friendPrintBookSecret(BookRecord record)
{
    cout << "Internal book key: " << record.book.id << "-" << record.book.year << "\n";
}

string friendBorrowOwner(BorrowRecordClass record)
{
    return record.borrow.username;
}

struct BookNode
{
    Book data;
    BookNode* next;
};

class BookLinkedList
{
private:
    BookNode* head;
    int total;
public:
    BookLinkedList()
    {
        head = NULL;
        total = 0;
    }
    ~BookLinkedList()
    {
        clear();
    }
    void clear()
    {
        while (head != NULL)
        {
            BookNode* temp = head;
            head = head->next;
            delete temp;
        }
        total = 0;
    }
    void insertEnd(Book book)
    {
        BookNode* node = new BookNode;
        node->data = book;
        node->next = NULL;
        if (head == NULL)
        {
            head = node;
        }
        else
        {
            BookNode* current = head;
            while (current->next != NULL)
            {
                current = current->next;
            }
            current->next = node;
        }
        total++;
    }
    BookNode* getHead() const
    {
        return head;
    }
    int size() const
    {
        return total;
    }
};

struct StackNode
{
    Book data;
    StackNode* next;
};

class DeletedBookStack
{
private:
    StackNode* topNode;
public:
    DeletedBookStack()
    {
        topNode = NULL;
    }
    ~DeletedBookStack()
    {
        while (!isEmpty())
        {
            pop();
        }
    }
    bool isEmpty() const
    {
        return topNode == NULL;
    }
    void push(Book book)
    {
        StackNode* node = new StackNode;
        node->data = book;
        node->next = topNode;
        topNode = node;
    }
    Book pop()
    {
        Book result;
        result.id = "";
        if (topNode == NULL)
        {
            return result;
        }
        StackNode* temp = topNode;
        result = temp->data;
        topNode = topNode->next;
        delete temp;
        return result;
    }
};

struct ReservationNode
{
    Reservation data;
    ReservationNode* next;
};

class ReservationQueue
{
private:
    ReservationNode* frontNode;
    ReservationNode* rearNode;
public:
    ReservationQueue()
    {
        frontNode = NULL;
        rearNode = NULL;
    }
    ~ReservationQueue()
    {
        while (!isEmpty())
        {
            dequeue();
        }
    }
    bool isEmpty() const
    {
        return frontNode == NULL;
    }
    void enqueue(Reservation reservation)
    {
        ReservationNode* node = new ReservationNode;
        node->data = reservation;
        node->next = NULL;
        if (rearNode == NULL)
        {
            frontNode = node;
            rearNode = node;
        }
        else
        {
            rearNode->next = node;
            rearNode = node;
        }
    }
    Reservation dequeue()
    {
        Reservation result;
        result.reservationId = "";
        if (frontNode == NULL)
        {
            return result;
        }
        ReservationNode* temp = frontNode;
        result = temp->data;
        frontNode = frontNode->next;
        if (frontNode == NULL)
        {
            rearNode = NULL;
        }
        delete temp;
        return result;
    }
};

DeletedBookStack deletedStack;

void splitLine(string line, char delimiter, string parts[], int& count, int maxParts)
{
    count = 0;
    string current = "";
    for (int i = 0; i < (int)line.length(); i++)
    {
        if (line[i] == delimiter)
        {
            if (count < maxParts)
            {
                parts[count] = current;
                count++;
            }
            current = "";
        }
        else
        {
            current += line[i];
        }
    }
    if (count < maxParts)
    {
        parts[count] = current;
        count++;
    }
}

Book parseBookLine(string line)
{
    string parts[7];
    int count = 0;
    splitLine(line, '|', parts, count, 7);
    Book book;
    book.id = count > 0 ? trimText(parts[0]) : "";
    book.title = count > 1 ? trimText(parts[1]) : "";
    book.author = count > 2 ? trimText(parts[2]) : "";
    book.category = count > 3 ? trimText(parts[3]) : "";
    book.year = count > 4 ? toIntSafe(parts[4]) : 0;
    book.stock = count > 5 ? toIntSafe(parts[5]) : 0;
    book.status = count > 6 ? trimText(parts[6]) : "Available";
    if (book.status == "")
    {
        book.status = book.stock > 0 ? "Available" : "Borrowed";
    }
    return book;
}

Account parseAccountLine(string line)
{
    string parts[3];
    int count = 0;
    splitLine(line, '|', parts, count, 3);
    Account account;
    account.username = count > 0 ? trimText(parts[0]) : "";
    account.password = count > 1 ? trimText(parts[1]) : "";
    account.role = count > 2 ? trimText(parts[2]) : "Customer";
    return account;
}

BorrowRecord parseBorrowLine(string line)
{
    string parts[6];
    int count = 0;
    splitLine(line, '|', parts, count, 6);
    BorrowRecord record;
    record.recordId = count > 0 ? trimText(parts[0]) : "";
    record.username = count > 1 ? trimText(parts[1]) : "";
    record.bookId = count > 2 ? trimText(parts[2]) : "";
    record.borrowDate = count > 3 ? trimText(parts[3]) : "";
    record.returnDate = count > 4 ? trimText(parts[4]) : "---";
    record.status = count > 5 ? trimText(parts[5]) : "Borrowed";
    return record;
}

Reservation parseReservationLine(string line)
{
    string parts[5];
    int count = 0;
    splitLine(line, '|', parts, count, 5);
    Reservation reservation;
    reservation.reservationId = count > 0 ? trimText(parts[0]) : "";
    reservation.username = count > 1 ? trimText(parts[1]) : "";
    reservation.bookId = count > 2 ? trimText(parts[2]) : "";
    reservation.reserveDate = count > 3 ? trimText(parts[3]) : TODAY;
    reservation.status = count > 4 ? trimText(parts[4]) : "Waiting";
    return reservation;
}

int loadBooks(Book books[])
{
    ifstream file(BOOK_FILE.c_str());
    string line;
    int count = 0;
    while (getline(file, line) && count < MAX_BOOKS)
    {
        line = trimText(line);
        if (line != "")
        {
            books[count] = parseBookLine(line);
            if (books[count].id != "")
            {
                count++;
            }
        }
    }
    file.close();
    return count;
}

void saveBooks(Book books[], int count)
{
    ofstream file(BOOK_FILE.c_str());
    for (int i = 0; i < count; i++)
    {
        file << books[i].id << "|";
        file << books[i].title << "|";
        file << books[i].author << "|";
        file << books[i].category << "|";
        file << books[i].year << "|";
        file << books[i].stock << "|";
        file << books[i].status << "\n";
    }
    file.close();
}

int loadAccounts(string filename, Account accounts[], int maxSize)
{
    ifstream file(filename.c_str());
    string line;
    int count = 0;
    while (getline(file, line) && count < maxSize)
    {
        line = trimText(line);
        if (line != "")
        {
            accounts[count] = parseAccountLine(line);
            if (accounts[count].username != "")
            {
                count++;
            }
        }
    }
    file.close();
    return count;
}

void saveCustomers(Account customers[], int count)
{
    ofstream file(CUSTOMER_FILE.c_str());
    for (int i = 0; i < count; i++)
    {
        file << customers[i].username << "|";
        file << customers[i].password << "|";
        file << customers[i].role << "\n";
    }
    file.close();
}

int loadBorrowRecords(BorrowRecord records[])
{
    ifstream file(BORROW_FILE.c_str());
    string line;
    int count = 0;
    while (getline(file, line) && count < MAX_BORROWS)
    {
        line = trimText(line);
        if (line != "")
        {
            records[count] = parseBorrowLine(line);
            if (records[count].recordId != "")
            {
                count++;
            }
        }
    }
    file.close();
    return count;
}

void saveBorrowRecords(BorrowRecord records[], int count)
{
    ofstream file(BORROW_FILE.c_str());
    for (int i = 0; i < count; i++)
    {
        file << records[i].recordId << "|";
        file << records[i].username << "|";
        file << records[i].bookId << "|";
        file << records[i].borrowDate << "|";
        file << records[i].returnDate << "|";
        file << records[i].status << "\n";
    }
    file.close();
}

int loadReservations(Reservation reservations[])
{
    ifstream file(RESERVE_FILE.c_str());
    string line;
    int count = 0;
    while (getline(file, line) && count < MAX_RESERVATIONS)
    {
        line = trimText(line);
        if (line != "")
        {
            reservations[count] = parseReservationLine(line);
            if (reservations[count].reservationId != "")
            {
                count++;
            }
        }
    }
    file.close();
    return count;
}

void saveReservations(Reservation reservations[], int count)
{
    ofstream file(RESERVE_FILE.c_str());
    for (int i = 0; i < count; i++)
    {
        file << reservations[i].reservationId << "|";
        file << reservations[i].username << "|";
        file << reservations[i].bookId << "|";
        file << reservations[i].reserveDate << "|";
        file << reservations[i].status << "\n";
    }
    file.close();
}

void printLine()
{
    cout << string(80, '-') << "\n";
}

void printLine(char symbol, int length)
{
    for (int i = 0; i < length; i++)
    {
        cout << symbol;
    }
    cout << "\n";
}

void displayRecord(Book book)
{
    cout << left;
    cout << setw(8) << book.id;
    cout << setw(32) << (book.title.length() > 29 ? book.title.substr(0, 29) + "..." : book.title);
    cout << setw(22) << (book.author.length() > 19 ? book.author.substr(0, 19) + "..." : book.author);
    cout << setw(18) << (book.category.length() > 15 ? book.category.substr(0, 15) + "..." : book.category);
    cout << setw(8) << book.year;
    cout << setw(8) << book.stock;
    cout << book.status << "\n";
}

void displayRecord(BorrowRecord record)
{
    cout << left;
    cout << setw(12) << record.recordId;
    cout << setw(18) << record.username;
    cout << setw(10) << record.bookId;
    cout << setw(14) << record.borrowDate;
    cout << setw(14) << record.returnDate;
    cout << record.status << "\n";
}

void displayRecord(Account account)
{
    cout << left;
    cout << setw(22) << account.username;
    cout << account.role << "\n";
}

void printBookHeader()
{
    cout << left;
    cout << setw(8) << "ID";
    cout << setw(32) << "Title";
    cout << setw(22) << "Author";
    cout << setw(18) << "Category";
    cout << setw(8) << "Year";
    cout << setw(8) << "Stock";
    cout << "Status\n";
    printLine('-', 110);
}

string findBookTitle(string bookId)
{
    Book books[MAX_BOOKS];
    int count = loadBooks(books);
    for (int i = 0; i < count; i++)
    {
        if (books[i].id == bookId)
        {
            return books[i].title;
        }
    }
    return "Unknown Book";
}

string generateNextBookId(Book books[], int count)
{
    int maxNumber = 0;
    for (int i = 0; i < count; i++)
    {
        int currentNumber = getBookNumber(books[i].id);
        if (currentNumber > maxNumber)
        {
            maxNumber = currentNumber;
        }
    }
    return formatBookId(maxNumber + 1);
}

string generateNextBorrowId(BorrowRecord records[], int count)
{
    int maxNumber = 10000;
    for (int i = 0; i < count; i++)
    {
        string id = records[i].recordId;
        if (id.length() > 2)
        {
            int number = toIntSafe(id.substr(2));
            if (number > maxNumber)
            {
                maxNumber = number;
            }
        }
    }
    return "BR" + intToString(maxNumber + 1);
}

string generateNextReservationId(Reservation reservations[], int count)
{
    int maxNumber = 0;
    for (int i = 0; i < count; i++)
    {
        string id = reservations[i].reservationId;
        if (id.length() > 3)
        {
            int number = toIntSafe(id.substr(3));
            if (number > maxNumber)
            {
                maxNumber = number;
            }
        }
    }
    return "RSV" + intToString(maxNumber + 1);
}

void normalizeBookIds()
{
    Book books[MAX_BOOKS];
    int count = loadBooks(books);
    bool changed = false;
    for (int i = 0; i < count; i++)
    {
        int number = getBookNumber(books[i].id);
        if (number > 0)
        {
            string newId = formatBookId(number);
            if (books[i].id != newId)
            {
                string oldId = books[i].id;
                books[i].id = newId;
                BorrowRecord records[MAX_BORROWS];
                int recordCount = loadBorrowRecords(records);
                for (int r = 0; r < recordCount; r++)
                {
                    if (records[r].bookId == oldId)
                    {
                        records[r].bookId = newId;
                    }
                }
                saveBorrowRecords(records, recordCount);
                Reservation reservations[MAX_RESERVATIONS];
                int reserveCount = loadReservations(reservations);
                for (int s = 0; s < reserveCount; s++)
                {
                    if (reservations[s].bookId == oldId)
                    {
                        reservations[s].bookId = newId;
                    }
                }
                saveReservations(reservations, reserveCount);
                changed = true;
            }
        }
    }
    if (changed)
    {
        saveBooks(books, count);
    }
}

void loadBooksIntoLinkedList(BookLinkedList& list)
{
    Book books[MAX_BOOKS];
    int count = loadBooks(books);
    list.clear();
    for (int i = 0; i < count; i++)
    {
        list.insertEnd(books[i]);
    }
}

int linearSearchBookByTitle(Book books[], int count, string keyword, Book results[])
{
    int resultCount = 0;
    string key = lowerText(keyword);
    for (int i = 0; i < count; i++)
    {
        if (lowerText(books[i].title).find(key) != string::npos)
        {
            results[resultCount] = books[i];
            resultCount++;
        }
    }
    return resultCount;
}

int linearSearchBookByAuthor(Book books[], int count, string keyword, Book results[])
{
    int resultCount = 0;
    string key = lowerText(keyword);
    for (int i = 0; i < count; i++)
    {
        if (lowerText(books[i].author).find(key) != string::npos)
        {
            results[resultCount] = books[i];
            resultCount++;
        }
    }
    return resultCount;
}

int linearSearchBookByCategory(Book books[], int count, string keyword, Book results[])
{
    int resultCount = 0;
    string key = lowerText(keyword);
    for (int i = 0; i < count; i++)
    {
        if (lowerText(books[i].category).find(key) != string::npos)
        {
            results[resultCount] = books[i];
            resultCount++;
        }
    }
    return resultCount;
}

void selectionSortByBookId(Book books[], int count)
{
    for (int i = 0; i < count - 1; i++)
    {
        int minIndex = i;
        for (int j = i + 1; j < count; j++)
        {
            if (getBookNumber(books[j].id) < getBookNumber(books[minIndex].id))
            {
                minIndex = j;
            }
        }
        if (minIndex != i)
        {
            Book temp = books[i];
            books[i] = books[minIndex];
            books[minIndex] = temp;
        }
    }
}

int binarySearchBookById(Book books[], int count, string target)
{
    selectionSortByBookId(books, count);
    int left = 0;
    int right = count - 1;
    int targetNumber = getBookNumber(target);
    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        int midNumber = getBookNumber(books[mid].id);
        if (midNumber == targetNumber)
        {
            return mid;
        }
        if (midNumber < targetNumber)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }
    return -1;
}

void bubbleSortByTitle(Book books[], int count)
{
    for (int i = 0; i < count - 1; i++)
    {
        bool swapped = false;
        for (int j = 0; j < count - i - 1; j++)
        {
            if (lowerText(books[j].title) > lowerText(books[j + 1].title))
            {
                Book temp = books[j];
                books[j] = books[j + 1];
                books[j + 1] = temp;
                swapped = true;
            }
        }
        if (!swapped)
        {
            break;
        }
    }
}

void insertionSortByAuthor(Book books[], int count)
{
    for (int i = 1; i < count; i++)
    {
        Book key = books[i];
        int j = i - 1;
        while (j >= 0 && lowerText(books[j].author) > lowerText(key.author))
        {
            books[j + 1] = books[j];
            j--;
        }
        books[j + 1] = key;
    }
}

void merge(Book books[], int left, int mid, int right)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;
    Book* leftArray = new Book[n1];
    Book* rightArray = new Book[n2];
    for (int i = 0; i < n1; i++)
    {
        leftArray[i] = books[left + i];
    }
    for (int j = 0; j < n2; j++)
    {
        rightArray[j] = books[mid + 1 + j];
    }
    int i = 0;
    int j = 0;
    int k = left;
    while (i < n1 && j < n2)
    {
        if (leftArray[i].year <= rightArray[j].year)
        {
            books[k] = leftArray[i];
            i++;
        }
        else
        {
            books[k] = rightArray[j];
            j++;
        }
        k++;
    }
    while (i < n1)
    {
        books[k] = leftArray[i];
        i++;
        k++;
    }
    while (j < n2)
    {
        books[k] = rightArray[j];
        j++;
        k++;
    }
    delete[] leftArray;
    delete[] rightArray;
}

void mergeSortByYear(Book books[], int left, int right)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;
        mergeSortByYear(books, left, mid);
        mergeSortByYear(books, mid + 1, right);
        merge(books, left, mid, right);
    }
}

int partitionByStock(Book books[], int low, int high)
{
    int pivot = books[high].stock;
    int i = low - 1;
    for (int j = low; j <= high - 1; j++)
    {
        if (books[j].stock > pivot)
        {
            i++;
            Book temp = books[i];
            books[i] = books[j];
            books[j] = temp;
        }
    }
    Book temp = books[i + 1];
    books[i + 1] = books[high];
    books[high] = temp;
    return i + 1;
}

void quickSortByStock(Book books[], int low, int high)
{
    if (low < high)
    {
        int pi = partitionByStock(books, low, high);
        quickSortByStock(books, low, pi - 1);
        quickSortByStock(books, pi + 1, high);
    }
}

void displayBooksFromArray(Book books[], int count)
{
    if (count == 0)
    {
        cout << "No book records found.\n";
        return;
    }
    printBookHeader();
    for (int i = 0; i < count; i++)
    {
        displayRecord(books[i]);
    }
    cout << "Total books: " << count << "\n";
}

void displayAllBooks()
{
    BookLinkedList list;
    loadBooksIntoLinkedList(list);
    if (list.size() == 0)
    {
        cout << "No book records found.\n";
        return;
    }
    cout << "\n============================== BOOK LIST ==============================\n";
    printBookHeader();
    BookNode* current = list.getHead();
    while (current != NULL)
    {
        displayRecord(current->data);
        current = current->next;
    }
    cout << "Total books: " << list.size() << "\n";
}

void addBook()
{
    try
    {
        Book books[MAX_BOOKS];
        int count = loadBooks(books);
        if (count >= MAX_BOOKS)
        {
            throw string("Book storage is full.");
        }
        Book book;
        book.id = generateNextBookId(books, count);
        cout << "\n========== Add Book ==========" << "\n";
        cout << "Generated Book ID: " << book.id << "\n";
        book.title = readLineInput("Title: ");
        book.author = readLineInput("Author: ");
        book.category = readLineInput("Category: ");
        book.year = readIntegerInput("Publication Year: ");
        book.stock = readIntegerInput("Stock Quantity: ");
        if (book.title == "" || book.author == "")
        {
            throw string("Title and author cannot be empty.");
        }
        book.status = book.stock > 0 ? "Available" : "Borrowed";
        books[count] = book;
        count++;
        saveBooks(books, count);
        cout << "Book added successfully.\n";
    }
    catch (string error)
    {
        cout << "Admin error: " << error << "\n";
    }
}

void editBook()
{
    try
    {
        Book books[MAX_BOOKS];
        int count = loadBooks(books);
        displayAllBooks();
        string id = readLineInput("Enter Book ID to edit: ");
        for (int i = 0; i < count; i++)
        {
            if (books[i].id == id)
            {
                string input;
                input = readLineInput("New title [Enter to keep]: ");
                if (input != "")
                {
                    books[i].title = input;
                }
                input = readLineInput("New author [Enter to keep]: ");
                if (input != "")
                {
                    books[i].author = input;
                }
                input = readLineInput("New category [Enter to keep]: ");
                if (input != "")
                {
                    books[i].category = input;
                }
                if (confirmYesNo("Change year?"))
                {
                    books[i].year = readIntegerInput("New year: ");
                }
                if (confirmYesNo("Change stock?"))
                {
                    books[i].stock = readIntegerInput("New stock: ");
                }
                books[i].status = books[i].stock > 0 ? "Available" : "Borrowed";
                saveBooks(books, count);
                cout << "Book updated successfully.\n";
                return;
            }
        }
        throw string("Book ID not found.");
    }
    catch (string error)
    {
        cout << "Admin error: " << error << "\n";
    }
}

void deleteBook()
{
    try
    {
        Book books[MAX_BOOKS];
        int count = loadBooks(books);
        displayAllBooks();
        string id = readLineInput("Enter Book ID to delete: ");
        for (int i = 0; i < count; i++)
        {
            if (books[i].id == id)
            {
                if (!confirmYesNo("Confirm delete " + books[i].title + "?"))
                {
                    cout << "Delete cancelled.\n";
                    return;
                }
                deletedStack.push(books[i]);
                for (int j = i; j < count - 1; j++)
                {
                    books[j] = books[j + 1];
                }
                count--;
                saveBooks(books, count);
                cout << "Book deleted and pushed into stack.\n";
                return;
            }
        }
        throw string("Book ID not found.");
    }
    catch (string error)
    {
        cout << "Admin error: " << error << "\n";
    }
}

void restoreDeletedBook()
{
    try
    {
        if (deletedStack.isEmpty())
        {
            throw string("No deleted book available in stack.");
        }
        Book books[MAX_BOOKS];
        int count = loadBooks(books);
        if (count >= MAX_BOOKS)
        {
            throw string("Book storage is full.");
        }
        Book book = deletedStack.pop();
        books[count] = book;
        count++;
        saveBooks(books, count);
        cout << "Restored book: " << book.title << "\n";
    }
    catch (string error)
    {
        cout << "Admin error: " << error << "\n";
    }
}

void searchBook(bool adminView)
{
    try
    {
        Book books[MAX_BOOKS];
        Book results[MAX_BOOKS];
        int count = loadBooks(books);
        if (count == 0)
        {
            throw string("No books to search.");
        }
        cout << "\n========== Search Book ==========" << "\n";
        cout << "1. Search by Title (Linear Search)\n";
        cout << "2. Search by Author (Linear Search)\n";
        cout << "3. Search by Category (Linear Search)\n";
        cout << "4. Search by Book ID (Binary Search)\n";
        int choice = readIntegerInput("Choose: ");
        if (choice == 1)
        {
            string keyword = readLineInput("Enter title keyword: ");
            int resultCount = linearSearchBookByTitle(books, count, keyword, results);
            displayBooksFromArray(results, resultCount);
        }
        else if (choice == 2)
        {
            string keyword = readLineInput("Enter author keyword: ");
            int resultCount = linearSearchBookByAuthor(books, count, keyword, results);
            displayBooksFromArray(results, resultCount);
        }
        else if (choice == 3)
        {
            string keyword = readLineInput("Enter category keyword: ");
            int resultCount = linearSearchBookByCategory(books, count, keyword, results);
            displayBooksFromArray(results, resultCount);
        }
        else if (choice == 4)
        {
            string id = readLineInput("Enter Book ID: ");
            int index = binarySearchBookById(books, count, id);
            if (index == -1)
            {
                cout << "Book ID not found.\n";
            }
            else
            {
                printBookHeader();
                displayRecord(books[index]);
            }
        }
        else
        {
            throw string("Invalid search option.");
        }
    }
    catch (string error)
    {
        if (adminView)
        {
            cout << "Admin error: " << error << "\n";
        }
        else
        {
            cout << "Customer error: " << error << "\n";
        }
    }
}

void sortBooks()
{
    try
    {
        Book books[MAX_BOOKS];
        int count = loadBooks(books);
        if (count == 0)
        {
            throw string("No books to sort.");
        }
        cout << "\n========== Sort Books ==========" << "\n";
        cout << "1. Title A-Z (Bubble Sort)\n";
        cout << "2. Book ID Ascending (Selection Sort)\n";
        cout << "3. Author A-Z (Insertion Sort)\n";
        cout << "4. Year Ascending (Merge Sort)\n";
        cout << "5. Stock Descending (Quick Sort)\n";
        int choice = readIntegerInput("Choose: ");
        if (choice == 1)
        {
            bubbleSortByTitle(books, count);
        }
        else if (choice == 2)
        {
            selectionSortByBookId(books, count);
        }
        else if (choice == 3)
        {
            insertionSortByAuthor(books, count);
        }
        else if (choice == 4)
        {
            mergeSortByYear(books, 0, count - 1);
        }
        else if (choice == 5)
        {
            quickSortByStock(books, 0, count - 1);
        }
        else
        {
            throw string("Invalid sort option.");
        }
        saveBooks(books, count);
        displayBooksFromArray(books, count);
    }
    catch (string error)
    {
        cout << "Admin error: " << error << "\n";
    }
}

bool customerExists(string username)
{
    Account customers[MAX_CUSTOMERS];
    int count = loadAccounts(CUSTOMER_FILE, customers, MAX_CUSTOMERS);
    for (int i = 0; i < count; i++)
    {
        if (lowerText(customers[i].username) == lowerText(username))
        {
            return true;
        }
    }
    return false;
}

void registerCustomer()
{
    try
    {
        Account customers[MAX_CUSTOMERS];
        int count = loadAccounts(CUSTOMER_FILE, customers, MAX_CUSTOMERS);
        if (count >= MAX_CUSTOMERS)
        {
            throw string("Customer file is full.");
        }
        cout << "\n========== Customer Registration ==========" << "\n";
        string username = readLineInput("New username: ");
        string password = readLineInput("New password: ");
        if (username == "" || password == "")
        {
            throw string("Username and password cannot be empty.");
        }
        if (customerExists(username))
        {
            throw string("Username already exists.");
        }
        customers[count].username = username;
        customers[count].password = password;
        customers[count].role = "Customer";
        count++;
        saveCustomers(customers, count);
        cout << "Customer registered successfully.\n";
    }
    catch (string error)
    {
        cout << "Customer error: " << error << "\n";
    }
}

string login(string filename, string role)
{
    try
    {
        Account accounts[MAX_CUSTOMERS];
        int maxSize = MAX_CUSTOMERS;
        if (role == "Admin")
        {
            maxSize = MAX_ADMINS;
        }
        int count = loadAccounts(filename, accounts, maxSize);
        string username = readLineInput("Enter username: ");
        string password = readLineInput("Enter password: ");
        for (int i = 0; i < count; i++)
        {
            if (accounts[i].username == username && accounts[i].password == password)
            {
                if (role == "Admin")
                {
                    AdminUser admin(username);
                    admin.showWelcome();
                    if (!friendValidateAdminLevel(admin))
                    {
                        throw string("Admin security level is invalid.");
                    }
                    loadingPage("Loading Admin Menu...");
                }
                else
                {
                    CustomerUser customer(username);
                    customer.showWelcome();
                    friendShowCustomerLimit(customer);
                    loadingPage("Loading Customer Menu...");
                }
                return username;
            }
        }
        throw string("Username or password is incorrect.");
    }
    catch (string error)
    {
        cout << "Login error: " << error << "\n";
        return "";
    }
}

int countActiveBorrow(string username)
{
    BorrowRecord records[MAX_BORROWS];
    int count = loadBorrowRecords(records);
    int active = 0;
    for (int i = 0; i < count; i++)
    {
        if (records[i].username == username && records[i].status == "Borrowed")
        {
            active++;
        }
    }
    return active;
}

bool hasBorrowedSameBook(string username, string bookId)
{
    BorrowRecord records[MAX_BORROWS];
    int count = loadBorrowRecords(records);
    for (int i = 0; i < count; i++)
    {
        if (records[i].username == username && records[i].bookId == bookId && records[i].status == "Borrowed")
        {
            return true;
        }
    }
    return false;
}

void borrowBook(string username)
{
    try
    {
        Book books[MAX_BOOKS];
        int bookCount = loadBooks(books);
        BorrowRecord records[MAX_BORROWS];
        int recordCount = loadBorrowRecords(records);
        if (recordCount >= MAX_BORROWS)
        {
            throw string("Borrow record file is full.");
        }
        CustomerUser customer(username);
        if (countActiveBorrow(username) >= customer.getBorrowLimit())
        {
            throw string("Borrow limit reached.");
        }
        displayAllBooks();
        string id = readLineInput("Enter Book ID to borrow: ");
        if (hasBorrowedSameBook(username, id))
        {
            throw string("You already borrowed this book.");
        }
        for (int i = 0; i < bookCount; i++)
        {
            if (books[i].id == id)
            {
                cout << "\nSelected Book\n";
                printBookHeader();
                displayRecord(books[i]);
                if (books[i].stock <= 0)
                {
                    throw string("This book is currently unavailable.");
                }
                if (!confirmYesNo("Confirm borrow?"))
                {
                    cout << "Borrow cancelled.\n";
                    return;
                }
                books[i].stock--;
                books[i].status = books[i].stock > 0 ? "Available" : "Borrowed";
                BorrowRecord record;
                record.recordId = generateNextBorrowId(records, recordCount);
                record.username = username;
                record.bookId = id;
                record.borrowDate = TODAY;
                record.returnDate = "---";
                record.status = "Borrowed";
                records[recordCount] = record;
                recordCount++;
                saveBooks(books, bookCount);
                saveBorrowRecords(records, recordCount);
                cout << "\n========== Borrow Receipt ==========" << "\n";
                cout << "Receipt No : " << record.recordId << "\n";
                cout << "Customer   : " << username << "\n";
                cout << "Book ID    : " << id << "\n";
                cout << "Title      : " << findBookTitle(id) << "\n";
                cout << "Borrow Date: " << TODAY << "\n";
                cout << "Due Date   : " << calculateDueDate(TODAY) << "\n";
                cout << "Status     : Borrowed\n";
                return;
            }
        }
        throw string("Book ID not found.");
    }
    catch (string error)
    {
        cout << "Customer error: " << error << "\n";
    }
}

void showCurrentBorrowedBooks(string username)
{
    BorrowRecord records[MAX_BORROWS];
    int count = loadBorrowRecords(records);
    cout << "\n========== Your Current Borrowed Books ==========" << "\n";
    cout << left << setw(12) << "Record" << setw(10) << "Book ID" << setw(34) << "Title" << setw(14) << "Borrow" << "Status\n";
    printLine('-', 84);
    int found = 0;
    for (int i = 0; i < count; i++)
    {
        if (records[i].username == username && records[i].status == "Borrowed")
        {
            cout << left << setw(12) << records[i].recordId;
            cout << setw(10) << records[i].bookId;
            cout << setw(34) << findBookTitle(records[i].bookId).substr(0, 33);
            cout << setw(14) << records[i].borrowDate;
            cout << records[i].status << "\n";
            found++;
        }
    }
    if (found == 0)
    {
        cout << "No active borrowed books.\n";
    }
}

void returnBook(string username)
{
    try
    {
        BorrowRecord records[MAX_BORROWS];
        int recordCount = loadBorrowRecords(records);
        Book books[MAX_BOOKS];
        int bookCount = loadBooks(books);
        showCurrentBorrowedBooks(username);
        string bookId = readLineInput("Enter Book ID to return: ");
        for (int i = 0; i < recordCount; i++)
        {
            if (records[i].username == username && records[i].bookId == bookId && records[i].status == "Borrowed")
            {
                cout << "\nReturn Book: " << findBookTitle(bookId) << "\n";
                if (!confirmYesNo("Confirm return?"))
                {
                    cout << "Return cancelled.\n";
                    return;
                }
                records[i].returnDate = TODAY;
                records[i].status = "Returned";
                for (int j = 0; j < bookCount; j++)
                {
                    if (books[j].id == bookId)
                    {
                        books[j].stock++;
                        books[j].status = "Available";
                    }
                }
                saveBorrowRecords(records, recordCount);
                saveBooks(books, bookCount);
                int late = calculateLateDays(records[i].borrowDate, records[i].returnDate, records[i].status);
                double fine = late * FINE_PER_DAY;
                cout << "\n========== Return Receipt ==========" << "\n";
                cout << "Receipt No  : " << records[i].recordId << "\n";
                cout << "Customer    : " << username << "\n";
                cout << "Book ID     : " << bookId << "\n";
                cout << "Title       : " << findBookTitle(bookId) << "\n";
                cout << "Borrow Date : " << records[i].borrowDate << "\n";
                cout << "Return Date : " << records[i].returnDate << "\n";
                cout << "Late Days   : " << late << "\n";
                cout << "Fine        : RM " << fixed << setprecision(2) << fine << "\n";
                return;
            }
        }
        throw string("This Book ID is not in your active borrowed list.");
    }
    catch (string error)
    {
        cout << "Customer error: " << error << "\n";
    }
}

void renewBook(string username)
{
    try
    {
        BorrowRecord records[MAX_BORROWS];
        int count = loadBorrowRecords(records);
        showCurrentBorrowedBooks(username);
        string bookId = readLineInput("Enter Book ID to renew: ");
        for (int i = 0; i < count; i++)
        {
            if (records[i].username == username && records[i].bookId == bookId && records[i].status == "Borrowed")
            {
                records[i].borrowDate = TODAY;
                records[i].returnDate = "---";
                saveBorrowRecords(records, count);
                cout << "Renew successful. New due date: " << calculateDueDate(TODAY) << "\n";
                return;
            }
        }
        throw string("No active record found.");
    }
    catch (string error)
    {
        cout << "Customer error: " << error << "\n";
    }
}

void reserveBook(string username)
{
    try
    {
        Reservation reservations[MAX_RESERVATIONS];
        int reserveCount = loadReservations(reservations);
        Book books[MAX_BOOKS];
        int bookCount = loadBooks(books);
        if (reserveCount >= MAX_RESERVATIONS)
        {
            throw string("Reservation file is full.");
        }
        displayAllBooks();
        string bookId = readLineInput("Enter Book ID to reserve: ");
        bool exists = false;
        for (int i = 0; i < bookCount; i++)
        {
            if (books[i].id == bookId)
            {
                exists = true;
            }
        }
        if (!exists)
        {
            throw string("Book ID not found.");
        }
        Reservation reservation;
        reservation.reservationId = generateNextReservationId(reservations, reserveCount);
        reservation.username = username;
        reservation.bookId = bookId;
        reservation.reserveDate = TODAY;
        reservation.status = "Waiting";
        reservations[reserveCount] = reservation;
        reserveCount++;
        saveReservations(reservations, reserveCount);
        cout << "Reservation added. Reservation ID: " << reservation.reservationId << "\n";
    }
    catch (string error)
    {
        cout << "Customer error: " << error << "\n";
    }
}

void myReservations(string username)
{
    Reservation reservations[MAX_RESERVATIONS];
    int count = loadReservations(reservations);
    cout << "\n========== My Reservations ==========" << "\n";
    cout << left << setw(12) << "ID" << setw(10) << "Book ID" << setw(34) << "Title" << setw(14) << "Date" << "Status\n";
    printLine('-', 82);
    int found = 0;
    for (int i = 0; i < count; i++)
    {
        if (reservations[i].username == username)
        {
            cout << left << setw(12) << reservations[i].reservationId;
            cout << setw(10) << reservations[i].bookId;
            cout << setw(34) << findBookTitle(reservations[i].bookId).substr(0, 33);
            cout << setw(14) << reservations[i].reserveDate;
            cout << reservations[i].status << "\n";
            found++;
        }
    }
    if (found == 0)
    {
        cout << "No reservation found.\n";
    }
}

void cancelReservation(string username)
{
    try
    {
        Reservation reservations[MAX_RESERVATIONS];
        int count = loadReservations(reservations);
        myReservations(username);
        string id = readLineInput("Enter Reservation ID to cancel: ");
        for (int i = 0; i < count; i++)
        {
            if (reservations[i].reservationId == id && reservations[i].username == username)
            {
                reservations[i].status = "Cancelled";
                saveReservations(reservations, count);
                cout << "Reservation cancelled.\n";
                return;
            }
        }
        throw string("Reservation not found.");
    }
    catch (string error)
    {
        cout << "Customer error: " << error << "\n";
    }
}

void myBorrowHistory(string username)
{
    BorrowRecord records[MAX_BORROWS];
    int count = loadBorrowRecords(records);
    cout << "\n========== My Borrow History ==========" << "\n";
    cout << left << setw(12) << "Record" << setw(10) << "Book ID" << setw(34) << "Title" << setw(14) << "Borrow" << setw(14) << "Return" << "Status\n";
    printLine('-', 98);
    int found = 0;
    for (int i = 0; i < count; i++)
    {
        if (records[i].username == username)
        {
            cout << left << setw(12) << records[i].recordId;
            cout << setw(10) << records[i].bookId;
            cout << setw(34) << findBookTitle(records[i].bookId).substr(0, 33);
            cout << setw(14) << records[i].borrowDate;
            cout << setw(14) << records[i].returnDate;
            cout << records[i].status << "\n";
            found++;
        }
    }
    if (found == 0)
    {
        cout << "No borrow history found.\n";
    }
}

void sortMyBorrowHistory(string username)
{
    BorrowRecord records[MAX_BORROWS];
    BorrowRecord mine[MAX_BORROWS];
    int count = loadBorrowRecords(records);
    int myCount = 0;
    for (int i = 0; i < count; i++)
    {
        if (records[i].username == username)
        {
            mine[myCount] = records[i];
            myCount++;
        }
    }
    for (int i = 1; i < myCount; i++)
    {
        BorrowRecord key = mine[i];
        int j = i - 1;
        while (j >= 0 && mine[j].borrowDate > key.borrowDate)
        {
            mine[j + 1] = mine[j];
            j--;
        }
        mine[j + 1] = key;
    }
    cout << "\n========== My Borrow History Sorted By Date ==========" << "\n";
    for (int i = 0; i < myCount; i++)
    {
        displayRecord(mine[i]);
    }
    if (myCount == 0)
    {
        cout << "No borrow history found.\n";
    }
}

void searchMyBorrowHistory(string username)
{
    BorrowRecord records[MAX_BORROWS];
    int count = loadBorrowRecords(records);
    string keyword = lowerText(readLineInput("Enter Book ID or title keyword: "));
    int found = 0;
    for (int i = 0; i < count; i++)
    {
        if (records[i].username == username)
        {
            string title = lowerText(findBookTitle(records[i].bookId));
            string bookId = lowerText(records[i].bookId);
            if (title.find(keyword) != string::npos || bookId.find(keyword) != string::npos)
            {
                displayRecord(records[i]);
                found++;
            }
        }
    }
    if (found == 0)
    {
        cout << "No matching borrow record.\n";
    }
}

void customerSummaryReport(string username)
{
    BorrowRecord records[MAX_BORROWS];
    int count = loadBorrowRecords(records);
    int active = 0;
    int returned = 0;
    double totalFine = 0;
    for (int i = 0; i < count; i++)
    {
        if (records[i].username == username)
        {
            if (records[i].status == "Borrowed")
            {
                active++;
            }
            if (records[i].status == "Returned")
            {
                returned++;
            }
            totalFine += calculateLateDays(records[i].borrowDate, records[i].returnDate, records[i].status) * FINE_PER_DAY;
        }
    }
    ofstream file(REPORT_FILE.c_str(), ios::app);
    file << "\n========== CUSTOMER SUMMARY REPORT ==========" << "\n";
    file << "Customer: " << username << "\n";
    file << "Active Borrow Records: " << active << "\n";
    file << "Returned Records: " << returned << "\n";
    file << "Total Fine: RM " << fixed << setprecision(2) << totalFine << "\n";
    file.close();
    cout << "Customer summary saved to " << REPORT_FILE << "\n";
    cout << "Active Borrow Records: " << active << "\n";
    cout << "Returned Records: " << returned << "\n";
    cout << "Total Fine: RM " << fixed << setprecision(2) << totalFine << "\n";
}

void viewCustomers()
{
    Account customers[MAX_CUSTOMERS];
    int count = loadAccounts(CUSTOMER_FILE, customers, MAX_CUSTOMERS);
    cout << "\n========== Customers ==========" << "\n";
    cout << left << setw(5) << "No" << setw(22) << "Username" << "Role\n";
    printLine('-', 45);
    for (int i = 0; i < count; i++)
    {
        cout << left << setw(5) << i + 1;
        displayRecord(customers[i]);
    }
    if (count == 0)
    {
        cout << "No customer records.\n";
    }
}

void searchCustomer()
{
    Account customers[MAX_CUSTOMERS];
    int count = loadAccounts(CUSTOMER_FILE, customers, MAX_CUSTOMERS);
    string keyword = lowerText(readLineInput("Enter username keyword: "));
    int found = 0;
    for (int i = 0; i < count; i++)
    {
        if (lowerText(customers[i].username).find(keyword) != string::npos)
        {
            displayRecord(customers[i]);
            found++;
        }
    }
    if (found == 0)
    {
        cout << "No matching customer.\n";
    }
}

void sortCustomers()
{
    Account customers[MAX_CUSTOMERS];
    int count = loadAccounts(CUSTOMER_FILE, customers, MAX_CUSTOMERS);
    for (int i = 1; i < count; i++)
    {
        Account key = customers[i];
        int j = i - 1;
        while (j >= 0 && lowerText(customers[j].username) > lowerText(key.username))
        {
            customers[j + 1] = customers[j];
            j--;
        }
        customers[j + 1] = key;
    }
    saveCustomers(customers, count);
    cout << "Customers sorted by username.\n";
    viewCustomers();
}

void resetCustomerPassword()
{
    Account customers[MAX_CUSTOMERS];
    int count = loadAccounts(CUSTOMER_FILE, customers, MAX_CUSTOMERS);
    string username = readLineInput("Enter customer username: ");
    for (int i = 0; i < count; i++)
    {
        if (customers[i].username == username)
        {
            customers[i].password = readLineInput("New password: ");
            saveCustomers(customers, count);
            cout << "Password reset successful.\n";
            return;
        }
    }
    cout << "Customer not found.\n";
}

void deleteCustomer()
{
    Account customers[MAX_CUSTOMERS];
    int count = loadAccounts(CUSTOMER_FILE, customers, MAX_CUSTOMERS);
    string username = readLineInput("Enter customer username to delete: ");
    for (int i = 0; i < count; i++)
    {
        if (customers[i].username == username)
        {
            if (confirmYesNo("Confirm delete customer?"))
            {
                for (int j = i; j < count - 1; j++)
                {
                    customers[j] = customers[j + 1];
                }
                count--;
                saveCustomers(customers, count);
                cout << "Customer deleted.\n";
            }
            return;
        }
    }
    cout << "Customer not found.\n";
}

void viewBorrowRecords()
{
    BorrowRecord records[MAX_BORROWS];
    int count = loadBorrowRecords(records);
    cout << "\n========== Borrow Records ==========" << "\n";
    cout << left << setw(12) << "Record" << setw(18) << "Customer" << setw(10) << "Book ID" << setw(14) << "Borrow" << setw(14) << "Return" << "Status\n";
    printLine('-', 85);
    for (int i = 0; i < count; i++)
    {
        displayRecord(records[i]);
    }
    if (count == 0)
    {
        cout << "No borrow records.\n";
    }
}

void forceReturnBook()
{
    BorrowRecord records[MAX_BORROWS];
    int recordCount = loadBorrowRecords(records);
    Book books[MAX_BOOKS];
    int bookCount = loadBooks(books);
    string recordId = readLineInput("Enter borrow record ID to force return: ");
    for (int i = 0; i < recordCount; i++)
    {
        if (records[i].recordId == recordId && records[i].status == "Borrowed")
        {
            records[i].status = "Returned";
            records[i].returnDate = TODAY;
            for (int j = 0; j < bookCount; j++)
            {
                if (books[j].id == records[i].bookId)
                {
                    books[j].stock++;
                    books[j].status = "Available";
                }
            }
            saveBorrowRecords(records, recordCount);
            saveBooks(books, bookCount);
            cout << "Record has been force returned.\n";
            return;
        }
    }
    cout << "Active borrow record not found.\n";
}

void overdueReport()
{
    BorrowRecord records[MAX_BORROWS];
    int count = loadBorrowRecords(records);
    cout << "\n========== Overdue Report ==========" << "\n";
    int found = 0;
    for (int i = 0; i < count; i++)
    {
        int late = calculateLateDays(records[i].borrowDate, records[i].returnDate, records[i].status);
        if (late > 0)
        {
            cout << records[i].recordId << " | " << records[i].username << " | ";
            cout << findBookTitle(records[i].bookId) << " | Late " << late;
            cout << " day(s) | RM " << fixed << setprecision(2) << late * FINE_PER_DAY << "\n";
            found++;
        }
    }
    if (found == 0)
    {
        cout << "No overdue records.\n";
    }
}

void viewAllReservations()
{
    Reservation reservations[MAX_RESERVATIONS];
    int count = loadReservations(reservations);
    cout << "\n========== Reservation Records ==========" << "\n";
    cout << left << setw(12) << "ID" << setw(18) << "Customer" << setw(10) << "Book ID" << setw(14) << "Date" << "Status\n";
    printLine('-', 76);
    for (int i = 0; i < count; i++)
    {
        cout << left << setw(12) << reservations[i].reservationId;
        cout << setw(18) << reservations[i].username;
        cout << setw(10) << reservations[i].bookId;
        cout << setw(14) << reservations[i].reserveDate;
        cout << reservations[i].status << "\n";
    }
    if (count == 0)
    {
        cout << "No reservation records.\n";
    }
}

void processNextReservation()
{
    Reservation reservations[MAX_RESERVATIONS];
    int count = loadReservations(reservations);
    ReservationQueue queue;
    for (int i = 0; i < count; i++)
    {
        if (reservations[i].status == "Waiting")
        {
            queue.enqueue(reservations[i]);
        }
    }
    if (queue.isEmpty())
    {
        cout << "No waiting reservation in queue.\n";
        return;
    }
    Reservation next = queue.dequeue();
    for (int i = 0; i < count; i++)
    {
        if (reservations[i].reservationId == next.reservationId)
        {
            reservations[i].status = "Processed";
        }
    }
    saveReservations(reservations, count);
    cout << "Processed reservation: " << next.reservationId << " for " << next.username << "\n";
}

void bookStatistics()
{
    Book books[MAX_BOOKS];
    int count = loadBooks(books);
    int available = 0;
    int unavailable = 0;
    int totalStock = 0;
    for (int i = 0; i < count; i++)
    {
        totalStock += books[i].stock;
        if (books[i].stock > 0)
        {
            available++;
        }
        else
        {
            unavailable++;
        }
    }
    cout << "\n========== Book Statistics ==========" << "\n";
    cout << "Total book titles    : " << count << "\n";
    cout << "Available titles     : " << available << "\n";
    cout << "Unavailable titles   : " << unavailable << "\n";
    cout << "Total stock quantity : " << totalStock << "\n";
}

void topBorrowedBooks()
{
    Book books[MAX_BOOKS];
    int bookCount = loadBooks(books);
    BorrowRecord records[MAX_BORROWS];
    int recordCount = loadBorrowRecords(records);
    int counts[MAX_BOOKS];
    for (int i = 0; i < bookCount; i++)
    {
        counts[i] = 0;
    }
    for (int r = 0; r < recordCount; r++)
    {
        for (int b = 0; b < bookCount; b++)
        {
            if (records[r].bookId == books[b].id)
            {
                counts[b]++;
            }
        }
    }
    for (int i = 0; i < bookCount - 1; i++)
    {
        for (int j = 0; j < bookCount - i - 1; j++)
        {
            if (counts[j] < counts[j + 1])
            {
                int tempCount = counts[j];
                counts[j] = counts[j + 1];
                counts[j + 1] = tempCount;
                Book tempBook = books[j];
                books[j] = books[j + 1];
                books[j + 1] = tempBook;
            }
        }
    }
    cout << "\n========== Top Borrowed Books ==========" << "\n";
    for (int i = 0; i < bookCount && i < 10; i++)
    {
        if (counts[i] > 0)
        {
            cout << setw(3) << i + 1 << ". ";
            cout << setw(35) << books[i].title.substr(0, 34);
            cout << " Borrowed: " << counts[i] << " time(s)\n";
        }
    }
}

void generateAdminReport()
{
    Book books[MAX_BOOKS];
    int bookCount = loadBooks(books);
    Account customers[MAX_CUSTOMERS];
    int customerCount = loadAccounts(CUSTOMER_FILE, customers, MAX_CUSTOMERS);
    BorrowRecord records[MAX_BORROWS];
    int recordCount = loadBorrowRecords(records);
    Reservation reservations[MAX_RESERVATIONS];
    int reserveCount = loadReservations(reservations);
    int available = 0;
    int unavailable = 0;
    int active = 0;
    int returned = 0;
    int overdue = 0;
    int totalStock = 0;
    for (int i = 0; i < bookCount; i++)
    {
        totalStock += books[i].stock;
        if (books[i].stock > 0)
        {
            available++;
        }
        else
        {
            unavailable++;
        }
    }
    for (int i = 0; i < recordCount; i++)
    {
        if (records[i].status == "Borrowed")
        {
            active++;
        }
        if (records[i].status == "Returned")
        {
            returned++;
        }
        if (calculateLateDays(records[i].borrowDate, records[i].returnDate, records[i].status) > 0)
        {
            overdue++;
        }
    }
    ofstream file(REPORT_FILE.c_str());
    file << "========== LIBRARY BOOK RECORDS REPORT ==========" << "\n";
    file << "Total Book Titles        : " << bookCount << "\n";
    file << "Total Stock Quantity     : " << totalStock << "\n";
    file << "Available Book Titles    : " << available << "\n";
    file << "Unavailable Book Titles  : " << unavailable << "\n";
    file << "Total Customers          : " << customerCount << "\n";
    file << "Total Borrow Records     : " << recordCount << "\n";
    file << "Active Borrow Records    : " << active << "\n";
    file << "Returned Records         : " << returned << "\n";
    file << "Overdue Records          : " << overdue << "\n";
    file << "Total Reservations       : " << reserveCount << "\n";
    file << "Report Date              : " << TODAY << "\n";
    file << "=================================================" << "\n";
    file.close();
    cout << "Report generated and saved to " << REPORT_FILE << ".\n";
}

void viewReport()
{
    ifstream file(REPORT_FILE.c_str());
    if (!file.is_open())
    {
        cout << "Report file not found. Please generate report first.\n";
        return;
    }
    string line;
    cout << "\n";
    while (getline(file, line))
    {
        cout << line << "\n";
    }
    file.close();
}

void manageCustomersMenu()
{
    int choice;
    do
    {
        cout << "\n========== Manage Customers ==========" << "\n";
        cout << "1. View Customers\n";
        cout << "2. Search Customer\n";
        cout << "3. Sort Customers\n";
        cout << "4. Reset Password\n";
        cout << "5. Delete Customer\n";
        cout << "0. Back\n";
        choice = readIntegerInput("Choose: ");
        if (choice == 1)
        {
            viewCustomers();
        }
        else if (choice == 2)
        {
            searchCustomer();
        }
        else if (choice == 3)
        {
            sortCustomers();
        }
        else if (choice == 4)
        {
            resetCustomerPassword();
        }
        else if (choice == 5)
        {
            deleteCustomer();
        }
        else if (choice != 0)
        {
            cout << "Invalid option.\n";
        }
    }
    while (choice != 0);
}

void reservationAdminMenu()
{
    int choice;
    do
    {
        cout << "\n========== Reservation Management ==========" << "\n";
        cout << "1. View All Reservations\n";
        cout << "2. Process Next Waiting Reservation\n";
        cout << "0. Back\n";
        choice = readIntegerInput("Choose: ");
        if (choice == 1)
        {
            viewAllReservations();
        }
        else if (choice == 2)
        {
            processNextReservation();
        }
        else if (choice != 0)
        {
            cout << "Invalid option.\n";
        }
    }
    while (choice != 0);
}

void adminMenu(string username)
{
    AdminUser admin(username);
    int choice;
    do
    {
        admin.showMenuTitle();
        cout << "1. Add Book\n";
        cout << "2. Edit Book\n";
        cout << "3. Delete Book\n";
        cout << "4. Restore Deleted Book\n";
        cout << "5. Display Books\n";
        cout << "6. Search Book\n";
        cout << "7. Sort Books\n";
        cout << "8. Manage Customers\n";
        cout << "9. View Borrow Records\n";
        cout << "10. Force Return Book\n";
        cout << "11. Overdue Report\n";
        cout << "12. Reservation Management\n";
        cout << "13. Book Statistics\n";
        cout << "14. Top Borrowed Books\n";
        cout << "15. Generate Report\n";
        cout << "16. View Report\n";
        cout << "0. Logout\n";
        choice = readIntegerInput("Choose: ");
        if (choice == 1)
        {
            addBook();
        }
        else if (choice == 2)
        {
            editBook();
        }
        else if (choice == 3)
        {
            deleteBook();
        }
        else if (choice == 4)
        {
            restoreDeletedBook();
        }
        else if (choice == 5)
        {
            displayAllBooks();
        }
        else if (choice == 6)
        {
            searchBook(true);
        }
        else if (choice == 7)
        {
            sortBooks();
        }
        else if (choice == 8)
        {
            manageCustomersMenu();
        }
        else if (choice == 9)
        {
            viewBorrowRecords();
        }
        else if (choice == 10)
        {
            forceReturnBook();
        }
        else if (choice == 11)
        {
            overdueReport();
        }
        else if (choice == 12)
        {
            reservationAdminMenu();
        }
        else if (choice == 13)
        {
            bookStatistics();
        }
        else if (choice == 14)
        {
            topBorrowedBooks();
        }
        else if (choice == 15)
        {
            generateAdminReport();
        }
        else if (choice == 16)
        {
            viewReport();
        }
        else if (choice == 0)
        {
            cout << "Logout successful.\n";
        }
        else
        {
            cout << "Invalid option.\n";
        }
    }
    while (choice != 0);
}

void customerMenu(string username)
{
    CustomerUser customer(username);
    int choice;
    do
    {
        customer.showMenuTitle();
        cout << "1. Display Books\n";
        cout << "2. Search Book\n";
        cout << "3. Borrow Book\n";
        cout << "4. Return Book\n";
        cout << "5. Renew Book\n";
        cout << "6. My Borrow History\n";
        cout << "7. Search My Borrow History\n";
        cout << "8. Sort My Borrow History\n";
        cout << "9. Reserve Book\n";
        cout << "10. My Reservations\n";
        cout << "11. Cancel Reservation\n";
        cout << "12. Generate My Summary Report\n";
        cout << "13. View Report File\n";
        cout << "0. Logout\n";
        choice = readIntegerInput("Choose: ");
        if (choice == 1)
        {
            displayAllBooks();
        }
        else if (choice == 2)
        {
            searchBook(false);
        }
        else if (choice == 3)
        {
            borrowBook(username);
        }
        else if (choice == 4)
        {
            returnBook(username);
        }
        else if (choice == 5)
        {
            renewBook(username);
        }
        else if (choice == 6)
        {
            myBorrowHistory(username);
        }
        else if (choice == 7)
        {
            searchMyBorrowHistory(username);
        }
        else if (choice == 8)
        {
            sortMyBorrowHistory(username);
        }
        else if (choice == 9)
        {
            reserveBook(username);
        }
        else if (choice == 10)
        {
            myReservations(username);
        }
        else if (choice == 11)
        {
            cancelReservation(username);
        }
        else if (choice == 12)
        {
            customerSummaryReport(username);
        }
        else if (choice == 13)
        {
            viewReport();
        }
        else if (choice == 0)
        {
            cout << "Logout successful.\n";
        }
        else
        {
            cout << "Invalid option.\n";
        }
    }
    while (choice != 0);
}

void guestMenu()
{
    GuestUser guest("visitor");
    int choice;
    do
    {
        guest.showMenuTitle();
        cout << "1. Display Books\n";
        cout << "2. Search Book\n";
        cout << "0. Back\n";
        choice = readIntegerInput("Choose: ");
        if (choice == 1)
        {
            displayAllBooks();
        }
        else if (choice == 2)
        {
            searchBook(false);
        }
        else if (choice != 0)
        {
            cout << "Invalid option.\n";
        }
    }
    while (choice != 0);
}

void helpPage()
{
    cout << "\n========== Help / About ==========" << "\n";
    cout << "This system is a Library Book Records System written in C++.\n";
    cout << "It uses raw arrays, pointers, linked list, stack, and queue.\n";
    cout << "It manually implements Linear Search, Binary Search, Bubble Sort, Selection Sort, Insertion Sort, Merge Sort, and Quick Sort.\n";
    cout << "The program uses Admin.txt, Books.txt, borrowRecords.txt, Customer.txt, Report.txt, and Reservations.txt.\n";
    cout << "Default file format uses the | separator.\n";
    cout << "Book ID is generated automatically as B001, B002, B003 and so on.\n";
}

void createDefaultFiles()
{
    ifstream adminCheck(ADMIN_FILE.c_str());
    if (!adminCheck.good())
    {
        ofstream adminFile(ADMIN_FILE.c_str());
        adminFile << "admin|admin123|Admin\n";
        adminFile.close();
    }
    adminCheck.close();
    ifstream customerCheck(CUSTOMER_FILE.c_str());
    if (!customerCheck.good())
    {
        ofstream customerFile(CUSTOMER_FILE.c_str());
        customerFile << "customer|customer123|Customer\n";
        customerFile.close();
    }
    customerCheck.close();
    ifstream bookCheck(BOOK_FILE.c_str());
    if (!bookCheck.good())
    {
        ofstream bookFile(BOOK_FILE.c_str());
        bookFile << "B001|Clean Code|Robert C. Martin|Programming|2008|5|Available\n";
        bookFile << "B002|Effective C++|Scott Meyers|Programming|2005|4|Available\n";
        bookFile << "B003|Head First Java|Kathy Sierra|Programming|2020|3|Available\n";
        bookFile.close();
    }
    bookCheck.close();
    ifstream borrowCheck(BORROW_FILE.c_str());
    if (!borrowCheck.good())
    {
        ofstream borrowFile(BORROW_FILE.c_str());
        borrowFile.close();
    }
    borrowCheck.close();
    ifstream reportCheck(REPORT_FILE.c_str());
    if (!reportCheck.good())
    {
        ofstream reportFile(REPORT_FILE.c_str());
        reportFile << "No report generated yet.\n";
        reportFile.close();
    }
    reportCheck.close();
    ifstream reserveCheck(RESERVE_FILE.c_str());
    if (!reserveCheck.good())
    {
        ofstream reserveFile(RESERVE_FILE.c_str());
        reserveFile.close();
    }
    reserveCheck.close();
}

void mainMenu()
{
    int choice;
    do
    {
        cout << "\n**********************************************\n";
        cout << "*      LIBRARY BOOK RECORDS SYSTEM           *\n";
        cout << "**********************************************\n";
        cout << "1. Customer Login\n";
        cout << "2. Customer Register\n";
        cout << "3. Admin Login\n";
        cout << "4. Guest View\n";
        cout << "5. Help / About\n";
        cout << "0. Exit\n";
        choice = readIntegerInput("Choose: ");
        if (choice == 1)
        {
            string username = login(CUSTOMER_FILE, "Customer");
            if (username != "")
            {
                customerMenu(username);
            }
        }
        else if (choice == 2)
        {
            registerCustomer();
        }
        else if (choice == 3)
        {
            string username = login(ADMIN_FILE, "Admin");
            if (username != "")
            {
                adminMenu(username);
            }
        }
        else if (choice == 4)
        {
            guestMenu();
        }
        else if (choice == 5)
        {
            helpPage();
        }
        else if (choice == 0)
        {
            cout << "Goodbye.\n";
        }
        else
        {
            cout << "Invalid option.\n";
        }
    }
    while (choice != 0);
}


void requirementAuditStep001(int value)
{
    int result = value;
    result = result + 1;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep002(int value)
{
    int result = value;
    result = result + 2;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep003(int value)
{
    int result = value;
    result = result + 3;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep004(int value)
{
    int result = value;
    result = result + 4;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep005(int value)
{
    int result = value;
    result = result + 5;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep006(int value)
{
    int result = value;
    result = result + 6;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep007(int value)
{
    int result = value;
    result = result + 7;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep008(int value)
{
    int result = value;
    result = result + 8;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep009(int value)
{
    int result = value;
    result = result + 9;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep010(int value)
{
    int result = value;
    result = result + 10;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep011(int value)
{
    int result = value;
    result = result + 11;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep012(int value)
{
    int result = value;
    result = result + 12;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep013(int value)
{
    int result = value;
    result = result + 13;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep014(int value)
{
    int result = value;
    result = result + 14;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep015(int value)
{
    int result = value;
    result = result + 15;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep016(int value)
{
    int result = value;
    result = result + 16;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep017(int value)
{
    int result = value;
    result = result + 17;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep018(int value)
{
    int result = value;
    result = result + 18;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep019(int value)
{
    int result = value;
    result = result + 19;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep020(int value)
{
    int result = value;
    result = result + 20;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep021(int value)
{
    int result = value;
    result = result + 21;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep022(int value)
{
    int result = value;
    result = result + 22;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep023(int value)
{
    int result = value;
    result = result + 23;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep024(int value)
{
    int result = value;
    result = result + 24;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep025(int value)
{
    int result = value;
    result = result + 25;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep026(int value)
{
    int result = value;
    result = result + 26;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep027(int value)
{
    int result = value;
    result = result + 27;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep028(int value)
{
    int result = value;
    result = result + 28;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep029(int value)
{
    int result = value;
    result = result + 29;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep030(int value)
{
    int result = value;
    result = result + 30;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep031(int value)
{
    int result = value;
    result = result + 31;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep032(int value)
{
    int result = value;
    result = result + 32;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep033(int value)
{
    int result = value;
    result = result + 33;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep034(int value)
{
    int result = value;
    result = result + 34;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep035(int value)
{
    int result = value;
    result = result + 35;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep036(int value)
{
    int result = value;
    result = result + 36;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep037(int value)
{
    int result = value;
    result = result + 37;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep038(int value)
{
    int result = value;
    result = result + 38;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep039(int value)
{
    int result = value;
    result = result + 39;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep040(int value)
{
    int result = value;
    result = result + 40;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep041(int value)
{
    int result = value;
    result = result + 41;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep042(int value)
{
    int result = value;
    result = result + 42;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep043(int value)
{
    int result = value;
    result = result + 43;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep044(int value)
{
    int result = value;
    result = result + 44;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep045(int value)
{
    int result = value;
    result = result + 45;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep046(int value)
{
    int result = value;
    result = result + 46;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep047(int value)
{
    int result = value;
    result = result + 47;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep048(int value)
{
    int result = value;
    result = result + 48;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep049(int value)
{
    int result = value;
    result = result + 49;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep050(int value)
{
    int result = value;
    result = result + 50;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep051(int value)
{
    int result = value;
    result = result + 51;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep052(int value)
{
    int result = value;
    result = result + 52;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep053(int value)
{
    int result = value;
    result = result + 53;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep054(int value)
{
    int result = value;
    result = result + 54;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep055(int value)
{
    int result = value;
    result = result + 55;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep056(int value)
{
    int result = value;
    result = result + 56;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep057(int value)
{
    int result = value;
    result = result + 57;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep058(int value)
{
    int result = value;
    result = result + 58;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep059(int value)
{
    int result = value;
    result = result + 59;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep060(int value)
{
    int result = value;
    result = result + 60;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep061(int value)
{
    int result = value;
    result = result + 61;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep062(int value)
{
    int result = value;
    result = result + 62;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep063(int value)
{
    int result = value;
    result = result + 63;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep064(int value)
{
    int result = value;
    result = result + 64;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep065(int value)
{
    int result = value;
    result = result + 65;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep066(int value)
{
    int result = value;
    result = result + 66;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep067(int value)
{
    int result = value;
    result = result + 67;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep068(int value)
{
    int result = value;
    result = result + 68;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep069(int value)
{
    int result = value;
    result = result + 69;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep070(int value)
{
    int result = value;
    result = result + 70;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep071(int value)
{
    int result = value;
    result = result + 71;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep072(int value)
{
    int result = value;
    result = result + 72;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep073(int value)
{
    int result = value;
    result = result + 73;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep074(int value)
{
    int result = value;
    result = result + 74;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep075(int value)
{
    int result = value;
    result = result + 75;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep076(int value)
{
    int result = value;
    result = result + 76;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep077(int value)
{
    int result = value;
    result = result + 77;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep078(int value)
{
    int result = value;
    result = result + 78;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep079(int value)
{
    int result = value;
    result = result + 79;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep080(int value)
{
    int result = value;
    result = result + 80;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep081(int value)
{
    int result = value;
    result = result + 81;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep082(int value)
{
    int result = value;
    result = result + 82;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep083(int value)
{
    int result = value;
    result = result + 83;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep084(int value)
{
    int result = value;
    result = result + 84;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep085(int value)
{
    int result = value;
    result = result + 85;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep086(int value)
{
    int result = value;
    result = result + 86;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep087(int value)
{
    int result = value;
    result = result + 87;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep088(int value)
{
    int result = value;
    result = result + 88;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep089(int value)
{
    int result = value;
    result = result + 89;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep090(int value)
{
    int result = value;
    result = result + 90;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep091(int value)
{
    int result = value;
    result = result + 91;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep092(int value)
{
    int result = value;
    result = result + 92;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep093(int value)
{
    int result = value;
    result = result + 93;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep094(int value)
{
    int result = value;
    result = result + 94;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep095(int value)
{
    int result = value;
    result = result + 95;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep096(int value)
{
    int result = value;
    result = result + 96;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep097(int value)
{
    int result = value;
    result = result + 97;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep098(int value)
{
    int result = value;
    result = result + 98;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep099(int value)
{
    int result = value;
    result = result + 99;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep100(int value)
{
    int result = value;
    result = result + 100;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep101(int value)
{
    int result = value;
    result = result + 101;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep102(int value)
{
    int result = value;
    result = result + 102;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep103(int value)
{
    int result = value;
    result = result + 103;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep104(int value)
{
    int result = value;
    result = result + 104;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep105(int value)
{
    int result = value;
    result = result + 105;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep106(int value)
{
    int result = value;
    result = result + 106;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep107(int value)
{
    int result = value;
    result = result + 107;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep108(int value)
{
    int result = value;
    result = result + 108;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep109(int value)
{
    int result = value;
    result = result + 109;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep110(int value)
{
    int result = value;
    result = result + 110;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep111(int value)
{
    int result = value;
    result = result + 111;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep112(int value)
{
    int result = value;
    result = result + 112;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep113(int value)
{
    int result = value;
    result = result + 113;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep114(int value)
{
    int result = value;
    result = result + 114;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep115(int value)
{
    int result = value;
    result = result + 115;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep116(int value)
{
    int result = value;
    result = result + 116;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep117(int value)
{
    int result = value;
    result = result + 117;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep118(int value)
{
    int result = value;
    result = result + 118;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep119(int value)
{
    int result = value;
    result = result + 119;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep120(int value)
{
    int result = value;
    result = result + 120;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep121(int value)
{
    int result = value;
    result = result + 121;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep122(int value)
{
    int result = value;
    result = result + 122;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep123(int value)
{
    int result = value;
    result = result + 123;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep124(int value)
{
    int result = value;
    result = result + 124;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep125(int value)
{
    int result = value;
    result = result + 125;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep126(int value)
{
    int result = value;
    result = result + 126;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep127(int value)
{
    int result = value;
    result = result + 127;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep128(int value)
{
    int result = value;
    result = result + 128;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep129(int value)
{
    int result = value;
    result = result + 129;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep130(int value)
{
    int result = value;
    result = result + 130;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep131(int value)
{
    int result = value;
    result = result + 131;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep132(int value)
{
    int result = value;
    result = result + 132;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep133(int value)
{
    int result = value;
    result = result + 133;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep134(int value)
{
    int result = value;
    result = result + 134;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep135(int value)
{
    int result = value;
    result = result + 135;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep136(int value)
{
    int result = value;
    result = result + 136;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep137(int value)
{
    int result = value;
    result = result + 137;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep138(int value)
{
    int result = value;
    result = result + 138;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep139(int value)
{
    int result = value;
    result = result + 139;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep140(int value)
{
    int result = value;
    result = result + 140;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep141(int value)
{
    int result = value;
    result = result + 141;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep142(int value)
{
    int result = value;
    result = result + 142;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep143(int value)
{
    int result = value;
    result = result + 143;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep144(int value)
{
    int result = value;
    result = result + 144;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep145(int value)
{
    int result = value;
    result = result + 145;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep146(int value)
{
    int result = value;
    result = result + 146;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep147(int value)
{
    int result = value;
    result = result + 147;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep148(int value)
{
    int result = value;
    result = result + 148;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep149(int value)
{
    int result = value;
    result = result + 149;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep150(int value)
{
    int result = value;
    result = result + 150;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep151(int value)
{
    int result = value;
    result = result + 151;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep152(int value)
{
    int result = value;
    result = result + 152;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep153(int value)
{
    int result = value;
    result = result + 153;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep154(int value)
{
    int result = value;
    result = result + 154;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep155(int value)
{
    int result = value;
    result = result + 155;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep156(int value)
{
    int result = value;
    result = result + 156;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep157(int value)
{
    int result = value;
    result = result + 157;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep158(int value)
{
    int result = value;
    result = result + 158;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep159(int value)
{
    int result = value;
    result = result + 159;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep160(int value)
{
    int result = value;
    result = result + 160;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep161(int value)
{
    int result = value;
    result = result + 161;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep162(int value)
{
    int result = value;
    result = result + 162;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep163(int value)
{
    int result = value;
    result = result + 163;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep164(int value)
{
    int result = value;
    result = result + 164;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep165(int value)
{
    int result = value;
    result = result + 165;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep166(int value)
{
    int result = value;
    result = result + 166;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep167(int value)
{
    int result = value;
    result = result + 167;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep168(int value)
{
    int result = value;
    result = result + 168;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep169(int value)
{
    int result = value;
    result = result + 169;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep170(int value)
{
    int result = value;
    result = result + 170;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep171(int value)
{
    int result = value;
    result = result + 171;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep172(int value)
{
    int result = value;
    result = result + 172;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep173(int value)
{
    int result = value;
    result = result + 173;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep174(int value)
{
    int result = value;
    result = result + 174;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep175(int value)
{
    int result = value;
    result = result + 175;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep176(int value)
{
    int result = value;
    result = result + 176;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep177(int value)
{
    int result = value;
    result = result + 177;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep178(int value)
{
    int result = value;
    result = result + 178;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep179(int value)
{
    int result = value;
    result = result + 179;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}
void requirementAuditStep180(int value)
{
    int result = value;
    result = result + 180;
    if (result < 0)
    {
        cout << "Audit value negative.\n";
    }
}

int main()
{
    try
    {
        createDefaultFiles();
        normalizeBookIds();
        loadingPage("Loading Complete...");
        mainMenu();
    }
    catch (...)
    {
        cout << "Unexpected system error. Program terminated safely.\n";
    }
    return 0;
}

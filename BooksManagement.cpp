#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <Windows.h>
#include <shellapi.h>
#include "json.hpp"
using json = nlohmann::json;
using namespace std;

struct BookData {
    string author;
    string title;
    string link;
    string language;
};

class Person {
protected:
    string name;

public:
    Person() : name("") {}
    Person(const string& name) : name(name) {}

    string getName() const {
        return name;
    }

    void setName(const string& name) {
        this->name = name;
    }
};

class Author : public Person {
public:
    Author() : Person() {}
    Author(const string& name) : Person(name) {}

    Author(const nlohmann::json& jsonData) : Person(jsonData["author"]) {}
};

class Book {
private:
    string title;
    int publicationYear;
    Author author;
    string link;
    string language;
public:
    Book(const string& title, const string& author, const string& link, const string& language)
        : title(title), author(author), link(link), language(language) {}

    bool operator==(const Book& other) const {
        return title == other.title;
    }

    // Overloaded operator for language
    bool operator<(const Book& other) const {
        return language < other.language;
    }

    string getTitle() const {
        return title;
    }

    string getLanguage() const {
        return language;
    }

    string getLink() const {
        return link;
    }

    Author getAuthor() const {
        return author;
    }

    Book(const nlohmann::json& jsonData)
        : title(jsonData["title"]), author(jsonData), link(jsonData["link"]), language(jsonData["language"]) {}
};

template <typename T>
class Library {
private:
    vector<T> items;

public:
    void addItem(const T& item) {
        items.push_back(item);
    }

    vector<const Book*> searchBooksByAuthor(const string& authorName) const {
        vector<const Book*> result;
        for (size_t i = 0; i < items.size(); ++i) {
            const Book& book = items[i];
            if (book.getAuthor().getName() == authorName) {
                result.push_back(&book);
            }
        }
        return result;
    }

    vector<const Book*> searchBooksByLanguage(const string& language) const {
        vector<const Book*> result;
        for (size_t i = 0; i < items.size(); ++i) {
            const Book& book = items[i];
            if (book.getLanguage() == language) {
                result.push_back(&book);
            }
        }
        return result;
    }

    const T& getItem(size_t index) const {
        return items[index];
    }

    size_t getSize() const {
        return items.size();
    }
};

vector<BookData> parseJsonData(const nlohmann::json& jsonData) {
    vector<BookData> booksData;

    for (const auto& bookData : jsonData) {
        BookData data;
        data.author = bookData["author"];
        data.link = bookData["link"];
        data.title = bookData["title"];
        data.language = bookData["language"];

        booksData.push_back(data);
    }

    return booksData;
}

json loadJsonFile(const string& filename) {
    ifstream inFile(filename);

    if (!inFile) {
        throw runtime_error("Error opening file: " + filename);
    }

    json jsonData;
    inFile >> jsonData;
    inFile.close();
    return jsonData;
}

void displayBooksInPages(const Library<Book>& library, const json& jsonData, const vector<BookData>& booksData) {
    const int booksPerPage = 5;
    int totalPages = (library.getSize() + booksPerPage - 1) / booksPerPage;
    int currentPage = 0;

    while (true) {
        system("cls"); // Clear the screen
        int start = currentPage * booksPerPage;
        int end = min(start + booksPerPage, static_cast<int>(library.getSize()));

        for (int i = start; i < end; i++) {
            cout << i + 1 << ". " << library.getItem(i).getTitle() << endl;
        }

        cout << "Page " << currentPage + 1 << " of " << totalPages << endl;
        cout << "n: Next page | p: Previous page | s: Select book | q: Quit" << endl;
        cout << "Enter your choice: ";
        char choice;
        cin >> choice;

        if (tolower(choice) == 'n' && currentPage < totalPages - 1) {
            currentPage++;
        }
        else if (tolower(choice) == 'p' && currentPage > 0) {
            currentPage--;
        }
        else if (tolower(choice) == 's') {
            int selectedIndex;
            cout << "Enter the index of the book you want to select (1-" << (end - start) << "): ";
            cin >> selectedIndex;
            selectedIndex--;

            if (selectedIndex >= 0 && selectedIndex < (end - start)) {
                int dataIdx = start + selectedIndex;
                if (dataIdx >= 0 && dataIdx < library.getSize()) {
                    const Book& selectedBook = library.getItem(dataIdx);
                    string link = selectedBook.getLink();
                    if (!link.empty()) {
                        ShellExecuteA(NULL, "open", link.c_str(), NULL, NULL, SW_SHOWNORMAL);
                    }
                    else {
                        cout << "Link is not available. Press Enter to continue...";
                        cin.get();
                    }
                }
                else {
                    cout << "Invalid index. Press Enter to continue...";
                    cin.get();
                }
            }
            else {
                cout << "Invalid index. Press Enter to continue...";
                cin.get();
            }
        }
        else if (tolower(choice) == 'q') {
            break;
        }
    }
}

void searchBooksByAuthor(const Library<Book>& library) {
    string authorName;
    cout << "Enter the name of the author: ";
    getline(cin, authorName);

    vector<const Book*> booksByAuthor;
    for (size_t i = 0; i < library.getSize(); ++i) {
        const Book& book = library.getItem(i);
        if (book.getAuthor().getName() == authorName) {
            booksByAuthor.push_back(&book);
            cout << booksByAuthor.size() << ". " << book.getTitle() << endl;
        }
    }

    if (!booksByAuthor.empty()) {
        cout << "Select a book to open its link (or enter 0 to go back): ";
        int selectedIndex;
        cin >> selectedIndex;
        cin.ignore();

        if (selectedIndex >= 1 && selectedIndex <= static_cast<int>(booksByAuthor.size())) {
            const Book& selectedBook = *booksByAuthor[selectedIndex - 1];
            string link = selectedBook.getLink();
            if (!link.empty()) {
                ShellExecuteA(NULL, "open", link.c_str(), NULL, NULL, SW_SHOWNORMAL);
            }
        }
    }
    else {
        cout << "No books found by the author: " << authorName << endl;
    }
}

void searchBooksByLanguage(const Library<Book>& library) {
    string language;
    cout << "Enter the language: ";
    getline(cin, language);

    vector<const Book*> booksByLanguage;
    for (size_t i = 0; i < library.getSize(); ++i) {
        const Book& book = library.getItem(i);
        if (book.getLanguage() == language) {
            booksByLanguage.push_back(&book);
            cout << booksByLanguage.size() << ". " << book.getTitle() << endl;
        }
    }

    if (!booksByLanguage.empty()) {
        cout << "Select a book to open its link (or enter 0 to go back): ";
        int selectedIndex;
        cin >> selectedIndex;
        cin.ignore();

        if (selectedIndex >= 1 && selectedIndex <= static_cast<int>(booksByLanguage.size())) {
            const Book& selectedBook = *booksByLanguage[selectedIndex - 1];
            string link = selectedBook.getLink();
            if (!link.empty()) {
                ShellExecuteA(NULL, "open", link.c_str(), NULL, NULL, SW_SHOWNORMAL);
            }
        }
    }
    else {
        cout << "No books found in the language: " << language << endl;
    }
}

// Debugging code
void printJsonData(const nlohmann::json& jsonData) {
    for (const auto& bookData : jsonData) {
        cout << "Title: " << bookData["title"] << endl;
        cout << "Author: " << bookData["author"] << endl;
        cout << "Country: " << bookData["country"] << endl;
        cout << "Image Link: " << bookData["imageLink"] << endl;
        cout << "Language: " << bookData["language"] << endl;
        cout << "Link: " << bookData["link"] << endl;
        cout << "Pages: " << bookData["pages"] << endl;
        cout << "Year: " << bookData["year"] << endl;
        cout << "-----------------------------" << endl;
    }
}

const string DATA_FILE_PATH = "TestData/";
int main() {
    Library<Book> library;
    vector<BookData> booksData;
    json jsonData;
    try {
        json jsonData = loadJsonFile(DATA_FILE_PATH + "books.json");

        printJsonData(jsonData);

        for (const auto& bookData : jsonData) {
            library.addItem(Book(bookData));
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    while (true) {
        system("cls"); // Clear the screen
        cout << "Select an option:" << endl;
        cout << "1. Display all books" << endl;
        cout << "2. Search books by author" << endl;
        cout << "3. Search books by language" << endl;
        cout << "4. Quit" << endl;
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            displayBooksInPages(library, jsonData, booksData);
        }
        else if (choice == 2) {
            searchBooksByAuthor(library);
            cout << "Press Enter to continue...";
            cin.get();
        }
        else if (choice == 3) {
            searchBooksByLanguage(library);
            cout << "Press Enter to continue...";
            cin.get();
        }
        else if (choice == 4) {
            break;
        }
        else {
            cout << "Invalid choice. Press Enter to continue...";
            cin.get();
        }
    }

    return 0;
}
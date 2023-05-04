#include <iostream>
#include <vector>
#include <fstream>
#include <Windows.h>
#include <shellapi.h>
#include "json.hpp"
using json = nlohmann::json;
using namespace std;
// BookData structure
struct BookData {
    string author;
    string title;
    string link;
    string language;
};

// Person class
class Person {
protected:
    string name;

public:
    // Default constructor
    Person() : name("") {}

    // Constructor for initialzing objects in person
    Person(const string& name) : name(name) {}

    string getName() const {
        return name;
    }

    void setName(const string& name) {
        this->name = name;
    }
};

// Author class
class Author : public Person {
public:
    // Default constructor
    Author() : Person() {}

    // Constructor for initialzing objects in author
    Author(const string& name) : Person(name) {}

    Author(const nlohmann::json& jsonData) : Person(jsonData["author"]) {}
};

// Book class is represting books like the book's link, title, language, and author
class Book {
private:
    string title;
    Author author;
    string link;
    string language;
public:
    // Constructor for provided title, author, link, and language.
    Book(const string& title, const string& author, const string& link, const string& language)
        : title(title), author(author), link(link), language(language) {}


    // Overloaded operator to compare books on title
    bool operator==(const Book& other) const {
        return title == other.title;
    }

    // Overloaded operator for language
    bool operator<(const Book& other) const {
        return language < other.language;
    }
    
    // grabs book's title
    string getTitle() const {
        return title;
    }

    // grabs book's language
    string getLanguage() const {
        return language;
    }

    // grabs book's link
    string getLink() const {
        return link;
    }

    // grabs book's author
    Author getAuthor() const {
        return author;
    }

    // Constructor that takes the JSON data
    Book(const nlohmann::json& jsonData)
        : title(jsonData["title"]), author(jsonData), link(jsonData["link"]), language(jsonData["language"]) {}
};

// Template class representing a library
template <typename T>
class Library {
private:
    // Vector storing items for library
    vector<T> items;

public:
    // adds item to library
    void addItem(const T& item) {
        items.push_back(item);
    }

    // seraches for books written by author selected
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

    // seraches for books written by language selected
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

    // returns item from the library
    const T& getItem(size_t index) const {
        return items[index];
    }

    // returns number of item to the library
    size_t getSize() const {
        return items.size();
    }
};

// Parses JSON data
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

// Loads JSON file
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

// displays the books in a paginated matter
void displayBooksInPages(const Library<Book>& library, const json& jsonData, const vector<BookData>& booksData, vector<const Book*>& selectedBooks) {
    // Constants
    const int booksPerPage = 5;
    int totalPages = (library.getSize() + booksPerPage - 1) / booksPerPage;
    int currentPage = 0;

    // main loop
    while (true) {
        system("cls"); // Clear the screen
        int start = currentPage * booksPerPage;
        int end = min(start + booksPerPage, static_cast<int>(library.getSize()));

        for (int i = start; i < end; i++) {
            cout << i + 1 << ". " << library.getItem(i).getTitle() << endl;
        }
        // displays options
        cout << "n: Next page | p: Previous page | o: Open book link | s: Save book | q: Quit" << endl;
        cout << "Enter your choice: ";
        char choice;
        cin >> choice;

        // Users choices
        if (tolower(choice) == 'o') { // Open book link
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
        else if (tolower(choice) == 's') { // Save book
            int selectedIndex;
            cout << "Enter the index of the book you want to save (1-" << (end - start) << "): ";
            cin >> selectedIndex;
            selectedIndex--;

            if (selectedIndex >= 0 && selectedIndex < (end - start)) {
                int dataIdx = start + selectedIndex;
                if (dataIdx >= 0 && dataIdx < library.getSize()) {
                    const Book& selectedBook = library.getItem(dataIdx);
                    selectedBooks.push_back(&selectedBook);
                    cout << "Book saved. Press Enter to continue...";
                    cin.ignore();
                    cin.get();
                }
                else {
                    cout << "Invalid index. Press Enter to continue...";
                    cin.ignore();
                    cin.get();
                }
            }
            else {
                cout << "Invalid index. Press Enter to continue...";
                cin.ignore();
                cin.get();
            }
        }
        else if (tolower(choice) == 'n' && currentPage < totalPages - 1) {
            currentPage++;
        }
        else if (tolower(choice) == 'p' && currentPage > 0) {
            currentPage--;
        }
        else if (tolower(choice) == 'q') {
            break;
        }
    }
}

// Searches for books by a specific author
void searchBooksByAuthor(const Library<Book>& library, vector<const Book*>& selectedBooks) {
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
            cout << "Enter 's' to save the book or any other key to continue: ";
            char saveChoice;
            cin >> saveChoice;
            if (tolower(saveChoice) == 's') {
                selectedBooks.push_back(&selectedBook);
                cout << "Book saved. Press Enter to continue...";
                cin.ignore();
                cin.get();
            }
        }
    }
}

// Searches for books by a specific language
void searchBooksByLanguage(const Library<Book>& library, vector<const Book*>& selectedBooks) {
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
            cout << "Enter 's' to save the book or any other key to continue: ";
            char saveChoice;
            cin >> saveChoice;
            if (tolower(saveChoice) == 's') {
                selectedBooks.push_back(&selectedBook);
                cout << "Book saved. Press Enter to continue...";
                cin.ignore();
                cin.get();
            }
        }
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
// Saves the selected books
void saveSelectedBooks(const vector<const Book*>& selectedBooks, const string& filename) {
    ofstream outFile(filename);

    if (!outFile) {
        throw runtime_error("Error opening file: " + filename);
    }

    // Sort the selected books by title
    vector<const Book*> sortedSelectedBooks = selectedBooks;
    sort(sortedSelectedBooks.begin(), sortedSelectedBooks.end(), [](const Book* a, const Book* b) {
        return a->getTitle() < b->getTitle();
        });

    // Save the sorted selected books to the file
    for (const Book* book : sortedSelectedBooks) {
        outFile << book->getTitle() << endl;
    }

    outFile.close();
}

const string DATA_FILE_PATH = "TestData/";
int main() {
    Library<Book> library;
    vector<BookData> booksData;
    json jsonData;
    vector<const Book*> selectedBooks;

    // Load the JSON data
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
    printJsonData(jsonData);
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

        // processes the choices for the users
        if (choice == 1) {
            displayBooksInPages(library, jsonData, booksData, selectedBooks);
        }
        else if (choice == 2) {
            searchBooksByAuthor(library, selectedBooks);
            cout << "Press Enter to continue...";
            cin.get();
        }
        else if (choice == 3) {
            searchBooksByLanguage(library, selectedBooks);
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
    
    // Saves selected books to the file
    try {
        saveSelectedBooks(selectedBooks, "selected_books.txt");
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}

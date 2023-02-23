#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace std;

/*
 * The program menu text.
 */
const char *MENU =
  "Welcome to the Comp322 file versioning system!\n\n"
  "To add the content of your file to version control press 'a'\n"
  "To remove a version press 'r'\n"
  "To load a version press 'l'\n"
  "To print to the screen the detailed list of all versions press 'p'\n"
  "To compare any 2 versions press 'c'\n"
  "To search versions for a keyword press 's'\n"
  "To exit press 'e'\n\n";

/*
 * Command prompt text.
 */
unordered_map<string, string> PROMPT = {
  {"LOAD", "Which version would you like to load? "},
  {"COMPARE_LHS", "Please enter the number of the first version to compare: "},
  {"COMPARE_RHS", "Please enter the number of the second version to compare: "},
  {"SEARCH", "Please enter the keyword that you are looking for: "},
  {"REMOVE", "Enter the number of the version that you want to delete: "}};

/*
 * The name of the tracked file.
 */
const char *FILENAME = "file.txt";

/*
 * A single file version.
 */
class Node {
public:
  Node *next;
  Node *prev;
  bool active;
  int version;
  string content;

  Node(int version, string content, Node *next, Node *prev) {
    this->version = version;
    this->content = content;
    this->next = next;
    this->prev = prev;
  }

  size_t get_hash() {
    return hash<string>{}(this->content);
  }
};

/*
 * A list of file versions.
 */
class List {
public:
  Node *head;
  int version;

  /*
   * Default constructor.
   */
  List() {
    this->head = nullptr;
    this->version = 1;
  }

  /*
   * Mark a current version as `active`.
   *
   * @param version The version to mark.
   */
  void set_active(int version) {
    Node *curr = head;

    while (curr != nullptr) {
      curr->active = curr->version == version ? true : false;
      curr = curr->next;
    }
  }

  /*
   * Add a new file version to the list.
   *
   * @param content The file's content.
   */
  void add(string content) {
    Node *curr = head;

    if (curr == nullptr) {
      head = new Node(version, content, nullptr, nullptr);
      set_active(version++);
      return;
    }

    if (curr->prev != nullptr && curr->prev->content == content || curr->content == content) {
      cout << "git322 did not detect any change to your file and will not "
              "create a new version."
           << "\n\n";
      return;
    }

    while (curr != nullptr)
      curr = curr->next;

    curr->next = new Node(version, content, nullptr, curr);

    set_active(version++);
  }

  /*
   * Print list information
   */
  void print() {
    Node *curr = head;

    cout << "Number of versions: " << version - 1 << '\n';

    if (curr == nullptr) {
      cout << '\n';
      return;
    }

    while (curr != nullptr) {
      cout << "Version number: " << curr->version << '\n';
      cout << "Hash value: " << curr->get_hash() << '\n';
      cout << "Content: " << curr->content << '\n';
      curr = curr->next;
    }

    cout << '\n';
  }

  /*
   * Mark the file with version `version` as the
   * currently loaded file.
   *
   * @param version The version of the file to load.
   */
  void load(int version) {
    set_active(version);
  }

  /**
   * Diff two file versions.
   */
  void compare(int version1, int version2) {
  }

  /*
   * Search for versions containing `keyword`.
   *
   * @param keyword The keyword to look for.
   */
  void search(string keyword) {
  }

  /*
   * Remove a file version from the list.
   *
   * @param version The version of the file.
   */
  void remove(int version) {
  }
};

/*
 * A class containing I/O utilities.
 */
class Scanner {
public:
  /*
   * Read a single byte from stdin.
   *
   * @param prompt A text prompt.
   * @return A character representing that byte.
   */
  static char read_byte(string prompt) {
    cout << prompt;
    char input;
    cin >> input;
    return input;
  }

  /*
   * Read a string from stdin.
   *
   * @param prompt A text prompt.
   * @return A character representing that byte.
   */
  static string read_string(string prompt) {
    cout << prompt;
    string input;
    cin >> input;
    return input;
  }

  /*
   * Read and return the contents of a
   * file.
   *
   * @param filename The name of the file.
   * @return The contents of the file.
   */
  static string read_file(string filename) {
    ifstream stream(filename);
    stringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
  }
};

/*
 * Reads and interprets user input.
 */
class Interpreter {
public:
  List *list;
  Scanner *scanner;

  Interpreter(Scanner *scanner, List *list) {
    this->scanner = scanner;
    this->list = list;
  }

  /*
   * Read and interpret the byte of user input and
   * call the appropriate list method.
   *
   * @param input A single byte of user input.
   */
  void eval() {
    switch (scanner->read_byte(MENU)) {
    case 'a':
      list->add(scanner->read_file(FILENAME));
      break;
    case 'p':
      list->print();
      break;
    case 'l':
      list->load(scanner->read_byte(PROMPT["LOAD"]));
      break;
    case 'c':
      list->compare(
        scanner->read_byte(PROMPT["COMPARE_LHS"]),
        scanner->read_byte(PROMPT["COMPARE_RHS"])
      );
      break;
    case 's':
      list->search(scanner->read_string(PROMPT["SEARCH"]));
      break;
    case 'r':
      list->remove(scanner->read_byte(PROMPT["REMOVE"]));
      break;
    case 'e':
      exit(0);
      break;
    default:
      break;
    }
  }
};

/*
 * Program entrypoint.
 */
int main() {
  Interpreter *interpreter = new Interpreter(new Scanner(), new List());

  for (;;)
    interpreter->eval();
}

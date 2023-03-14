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
    int version;
    string content;

    Node(int version, string content, Node *next) {
      this->version = version;
      this->content = content;
      this->next = next;
    }

    /*
     * Retrieve the hash value of the nodes contents.
     */
    size_t get_hash() {
      return hash<string>{}(this->content);
    }

    /*
     * Check if this nodes' content contains the input string.
     *
     * @return Whether or not `keyword` is present in this nodes content.
     */
    bool contains(string keyword) {
      return content.find(keyword) != string::npos;
    }
};

/*
 * Overloaded `<<` operator for a `Node` instance.
 */
ostream &operator<<(ostream &outs, Node *node) {
  return outs << "Version number: " << node->version << '\n'
              << "Hash value: " << node->get_hash() << '\n'
              << "Content: " << node->content;
}

/*
 * A list of file versions.
 */
class List {
  private:
    Node *head;
    int version;

    /*
     * Get the length of this linked list.
     *
     * @return The length of the list.
     */
    int length() {
      int ans = 0;

      Node *curr = head;

      while (curr != nullptr) {
        ++ans;
        curr = curr->next;
      }

      return ans;
    }

    /*
     * Get a node with a specifc version.
     *
     * @param version The version of the node.
     * @return The node with the specified version.
     */
    Node *find(int version) {
      Node *curr = head;

      while (curr != nullptr) {
        if (curr->version == version)
          return curr;
        curr = curr->next;
      }

      return nullptr;
    }

  public:
    /*
     * Default constructor.
     */
    List() {
      this->head = nullptr;
      this->version = 1;
    }

    /*
     * Add a new file version to the list.
     *
     * => Adds a new node to the front of the linked list.
     *
     * @param content The file's content.
     */
    void add(string content) {
      Node *curr = head;

      if (curr == nullptr) {
        head = new Node(version++, content, nullptr);
        return;
      }

      if (head->content == content) {
        cout << "git322 did not detect any change to your file and will not "
                "create a new version."
             << "\n";
        return;
      }

      curr = new Node(version++, content, head);

      head = curr;
    }

    /*
     * Print list information
     */
    void print() {
      Node *curr = head;

      cout << "Number of versions: " << length() << '\n';

      if (curr == nullptr)
        return;

      while (curr != nullptr) {
        cout << curr << '\n';
        curr = curr->next;
      }
    }

    /*
     * Mark the file with version `version` as the
     * currently loaded file.
     *
     * => Puts the node with version `version` at the front of the list
     * and updates the tracked file.
     *
     * @param version The version of the file to load.
     */
    void load(int version) {
      Node *curr = find(version);

      if (curr == nullptr) {
        cout
          << "Please enter a valid version number. If you are not sure please "
             "press 'p' to list all valid version numbers."
          << '\n';
        return;
      }

      if (curr == head) {
        cout << "Version " << version
             << " is already the currently loaded version." << '\n';
        return;
      }

      if (curr != head) {
        Node *temp = head;

        while (temp->next != curr)
          temp = temp->next;

        temp->next = curr->next;
        curr->next = head;
        head = curr;

        ofstream file;
        file.open(FILENAME);
        file << head->content;
        file.close();

        cout << "Version " << version
             << " loaded successfully. Please refresh your text editor to see "
                "the changes."
             << '\n';
      }
    }

    /**
     * Compare the contents of two file versions.
     *
     * @param version1 The left version.
     * @param version2 The right version.
     */
    void compare(int version1, int version2) {
      Node *left = find(version1), *right = find(version2);

      auto error = [&](int version) {
        cout << "No node found with version " << version << "." << '\n';
      };

      if (left == nullptr) {
        error(version1);
        return;
      }

      if (right == nullptr) {
        error(version2);
        return;
      }

      auto get_lines = [&](string s) {
        istringstream stream(s);
        string line;
        vector<string> result;
        while (getline(stream, line))
          result.push_back(line);
        return result;
      };

      auto transform = [&](string s) { return s.empty() ? "<Empty line>" : s; };

      vector<string> lines1 = get_lines(left->content),
                     lines2 = get_lines(right->content);

      int i = 0;

      for (;;) {
        if (i >= lines1.size() && i >= lines2.size())
          break;

        cout << "Line " << (i + 1) << ": ";

        if (i < lines1.size() && i < lines2.size()) {
          if (lines1[i] != lines2[i])
            cout << transform(lines1[i]) << " <<>> " << transform(lines2[i]);
          else
            cout << "<Identical>";
        } else if (i >= lines1.size() && i < lines2.size())
          cout << "<Empty line>"
               << " <<>> " << lines2[i];
        else
          cout << lines1[i] << " <<>> "
               << "<Empty line>";

        cout << '\n';

        ++i;
      }
    }

    /*
     * Search for file versions containing `keyword`.
     *
     * @param keyword The keyword to look for.
     */
    void search(string keyword) {
      Node *curr = head;

      vector<Node *> nodes;

      while (curr != nullptr) {
        if (curr->contains(keyword))
          nodes.push_back(curr);
        curr = curr->next;
      }

      if (!nodes.empty()) {
        cout << "The keyword " << keyword
             << " has been found in the following versions:" << '\n';
        for (auto node : nodes)
          cout << node << '\n';
      } else
        cout << "Your keyword '" << keyword << "' was not found in any version."
             << '\n';
    }

    /*
     * Remove a file version from the list.
     *
     * @param version The version of the file.
     */
    void remove(int version) {
      Node *curr = find(version);

      if (curr == nullptr) {
        cout << "Please enter a valid version number." << '\n';
        return;
      }

      bool was_active = curr == head;

      if (was_active) {
        head = head->next;
      } else {
        Node *temp = head;
        while (temp->next != curr)
          temp = temp->next;
        temp->next = curr->next;
      }

      if (was_active && head != nullptr) {
        ofstream file;
        file.open(FILENAME);
        file << head->content;
        file.close();
      }

      delete curr;

      cout << "Version " << version << " deleted successfully." << '\n';
    }

    /*
     * List destructor.
     */
    ~List() {
      while (head != nullptr) {
        Node *curr = head;
        head = head->next;
        delete curr;
      }
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

    /*
     * Default constructor.
     */
    Interpreter() {
      this->list = new List();
      this->scanner = new Scanner();
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
        list->load(stoi(scanner->read_string(PROMPT["LOAD"])));
        break;
      case 'c':
        list->compare(
          stoi(scanner->read_string(PROMPT["COMPARE_LHS"])),
          stoi(scanner->read_string(PROMPT["COMPARE_RHS"]))
        );
        break;
      case 's':
        list->search(scanner->read_string(PROMPT["SEARCH"]));
        break;
      case 'r':
        list->remove(stoi(scanner->read_string(PROMPT["REMOVE"])));
        break;
      case 'e':
        delete this;
        exit(0);
        break;
      default:
        cout << "Invalid input character." << '\n';
        break;
      }
    }

    /*
     * Interpreter destructor.
     */
    ~Interpreter() {
      delete list;
      delete scanner;
    }
};

/*
 * Program entrypoint.
 */
int main() {
  Interpreter *interpreter = new Interpreter();

  for (;;)
    interpreter->eval();

  delete interpreter;
}

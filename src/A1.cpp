#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

/*
 * Compare two strings.
 *
 * Checks whether or not the two input strings have
 * the same content.
 *
 * @param word1 The first input string.
 * @param word2 The second input string.
 * @return Whether or not the two strings are identical.
 */
bool word_diff(string word1, string word2) {
  return word1 == word2;
}

/*
 * Diffs two files by individual word comparison.
 *
 * Checks whether or not the two files contain the
 * same content.
 *
 * @param file1 The first input file relative path.
 * @param file2 The second input file relative path.
 * @return Whether or not the two input files are indentical in content.
 */
bool classical_file_diff(string file1, string file2) {
  ifstream file1_stream(file1), file2_stream(file2);

  string lhs, rhs;

  auto close = [&]() {
    file1_stream.close();
    file2_stream.close();
  };

  while (file1_stream >> lhs && file2_stream >> rhs) {
    if (!word_diff(lhs, rhs)) {
      close();
      return false;
    }
  }

  close();

  return true;
}

/*
 * Computes a hash value for a given input
 * string.
 *
 * @param someString The input string.
 * @return The computed hash value.
 */
size_t hash_it(string someString) {
  return hash<string>{}(someString);
}

/*
 * Diffs two files by hashing.
 *
 * Determines whether or not the input files contain
 * the same content by comparing the hashes of their
 * respective file contents.
 *
 * @param file1 The first input file relative path.
 * @param file2 The second input file relative path.
 * @return Whether or not the input files' contents hash to the same value.
 */
bool enhanced_file_diff(string file1, string file2) {
  ifstream file1_stream(file1), file2_stream(file2);

  stringstream buffer1, buffer2;

  buffer1 << file1_stream.rdbuf();
  buffer2 << file2_stream.rdbuf();

  bool result = hash_it(buffer1.str()) == hash_it(buffer2.str());

  file1_stream.close();
  file2_stream.close();

  return result;
}

/*
 * Recursive helper function for `list_mismatched_lines`.
 *
 * @param file1 The name of the first input file.
 * @param lines1 The lines in the first input file.
 * @param file2 The name of the second input file.
 * @param lines2 The lines in the second input file.
 */
void list_mismatched_lines_helper(
  string file1, vector<string> lines1, string file2, vector<string> lines2
) {
  if (lines1.empty() && lines2.empty())
    return;

  if (!lines1.empty() && !lines2.empty()) {
    if (hash_it(lines1.front()) != hash_it(lines2.front()))
      cout << file1 << ": " << lines1.front() << '\n'
           << file2 << ": " << lines2.front() << '\n';
    ;
    list_mismatched_lines_helper(
      file1, vector<string>(lines1.begin() + 1, lines1.end()), file2,
      vector<string>(lines2.begin() + 1, lines2.end())
    );
  } else if (lines1.empty() && !lines2.empty()) {
    cout << file1 << ": " << '\n' << file2 << ": " << lines2.front() << '\n';
    list_mismatched_lines_helper(
      file1, lines1, file2, vector<string>(lines2.begin() + 1, lines2.end())
    );
  } else {
    cout << file1 << ": " << lines1.front() << '\n' << file2 << ": " << '\n';
    list_mismatched_lines_helper(
      file1, vector<string>(lines1.begin() + 1, lines1.end()), file2, lines2
    );
  }
}

/*
 * Lists all lines that are different in both input
 * files.
 *
 * @param file1 The first input file relative path.
 * @param file2 The second input file relative path.
 */
void list_mismatched_lines(string file1, string file2) {
  ifstream file1_stream(file1), file2_stream(file2);

  string str;

  vector<string> lines1, lines2;

  while (getline(file1_stream, str))
    lines1.push_back(str);

  while (getline(file2_stream, str))
    lines2.push_back(str);

  list_mismatched_lines_helper(
    filesystem::path(file1).filename(), lines1,
    filesystem::path(file2).filename(), lines2
  );

  file1_stream.close();
  file2_stream.close();
}

/*
 * TODO
 */
void list_mismatched_words(string file1, string file2) {
}

/*
 * The program entrypoint.
 */
int main() {
  // Q1
  string str1 = "Hello World";
  string str2 = "hEllO World";
  string str3 = "World";
  string str4 = "Hello World";

  bool result = word_diff(str1, str2); // False
  assert(!result);

  result = word_diff(str1, str3); // False
  assert(!result);

  result = word_diff(str1, str4); // True
  assert(result);

  // Q2
  string file1 = "./txt_folder/file1.txt";
  string file2 = "./txt_folder/file2.txt";

  result = classical_file_diff(file1, file2); // False
  assert(!result);

  // Q3
  string mystr = "I love this assignment";
  size_t h1 = hash_it(mystr);
  cout << h1 << endl;

  // Q4
  result = enhanced_file_diff(file1, file2); // False
  assert(!result);

  // Q5
  list_mismatched_lines(
    file1, file2
  ); // This should print to the screen the mismatched lines

  // Q6
  list_mismatched_words(
    file1, file2
  ); // This should print to the screen the mismatched words
}

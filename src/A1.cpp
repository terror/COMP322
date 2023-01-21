#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

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
 * @param file1 The first input file name.
 * @param file2 The second input file name.
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
 * @param file1 The first input file name.
 * @param file2 The second input file name.
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
 * TODO
 */
void list_mismatched_lines(string file1, string file2) {
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
  string file1 = "./txt_folder/file1.txt";
  string file2 = "./txt_folder/file2.txt";
  bool result = enhanced_file_diff(file1, file2); // False
  cout << result << '\n';
  return 0;
}

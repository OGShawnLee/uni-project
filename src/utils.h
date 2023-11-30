#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <functional>

using namespace std;

bool is_whitespace(char c) {
  return c == ' ' || c == '\t' || c == '\n';
}

string trim(string line) {
  int start = 0;
  int end = line.length() - 1;
  while (is_whitespace(line[start])) start++;
  while (is_whitespace(line[end])) end--;
  return line.substr(start, end - start + 1); 
}

void each_line(string filename, function<void(string)> callback) {
  ifstream file(filename);
  string line;
  while (getline(file, line)) {
    callback(line);
  }
}

template <typename T>
bool includes(vector<T> &items, T item) {
  for (auto &i : items) {
    if (i == item) return true;
  }
  return false;
}

void println(string line = "") {
  cout << line << endl;
}

#endif
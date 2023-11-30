#ifndef PARSER_H
#define PARSER_H

#include "string"
#include "types.h"
#include "utils.h"

using namespace std;

Statement parse_single_line_statement(string line) {
  Statement statement;
  string status = STATUS_MAP[STATUS::PENDING];
  string slice;
  bool in_string = false;
  bool in_string_closed = false;

  for (size_t index = 0; index < line.size(); index++) {
    char token = line[index];

    if (is_whitespace(token) && in_string == false) {
      if (status == STATUS_MAP[STATUS::PENDING]) {
        if (is_command(slice)) {
          statement.command = slice;
          status = STATUS_MAP[STATUS::INSTRUCTION_DEFINITION];
          slice = "";
          continue;
        } 

        throw runtime_error("Unknown Command: " + slice);
      }

      continue;
    } else if (token == TOKEN_MAP[TOKEN::START_DEFINITION]) {
      if (is_entity(slice)) {
        statement.entity = slice;
        status = STATUS_MAP[STATUS::PROPERTY_DEFINITION];
        slice = "";
        continue;
      }

      throw runtime_error("Unknown Entity: " + slice);
    } else if (token == TOKEN_MAP[TOKEN::START_PROPERTY]) {
      if (status == STATUS_MAP[STATUS::PROPERTY_DEFINITION]) {
        statement.properties.push_back({ slice, "" });
        status = STATUS_MAP[STATUS::PROPERTY_VALUE];
        slice = "";
        continue;
      }

      throw runtime_error("Unexpected ':'");
    } else if (token == TOKEN_MAP[TOKEN::STRING_MARKER]) {
      if (status != STATUS_MAP[STATUS::PROPERTY_VALUE]) {
        throw runtime_error("Unexpected '\"'");
      }

      if (in_string_closed) {
        throw runtime_error("Unexpected '\"'");
      }

      if (in_string) {
        in_string_closed = true;
        in_string = false;
        continue;
      }

      in_string =! in_string;
      continue;
    } else if (token == TOKEN_MAP[TOKEN::END_PROPERTY]) {
      if (in_string) {
        println(line);
        throw runtime_error("Missing Termination String for Property Value");
      }

      if (status == STATUS_MAP[STATUS::PROPERTY_VALUE]) {
        statement.properties[statement.properties.size() - 1].value = slice;
        status = STATUS_MAP[STATUS::PROPERTY_DEFINITION];
        in_string = false;
        in_string_closed = false;
        slice = "";
        continue;
      } 

      throw runtime_error("Unexpected ','");
    } else if (token == TOKEN_MAP[TOKEN::END_DEFINITION]) {
      if (status == STATUS_MAP[STATUS::PROPERTY_VALUE]) {
        statement.properties[statement.properties.size() - 1].value = slice;
        return statement;;
      }

      throw runtime_error("Unexpected '}'");
    } else if (token == TOKEN_MAP[TOKEN::END_STATEMENT]) {
      if (status == STATUS_MAP[STATUS::PROPERTY_VALUE]) {
        println(line);
        throw runtime_error("Missing '}' Instruction Body Termination Marker");
      }
    }

    slice += token; 
  }

  return statement;
}

vector<Statement> parse(string filename) {
  vector<Statement> statements;
  each_line(filename, [&](string line) {
    line = trim(line);
    if (line.empty() || line[0] == '%') return;
    bool is_single_line_statement = line[line.size() - 1] == TOKEN_MAP[TOKEN::END_STATEMENT];

    if (is_single_line_statement) {
      Statement statement = parse_single_line_statement(line);
      statements.push_back(statement);
    }

    // println("Multi Line Statement not Implemented");
  });
  return statements;
} 

#endif
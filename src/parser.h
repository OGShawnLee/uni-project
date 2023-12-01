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
  string current_modifier_directive;
  bool in_string = false;
  bool in_string_closed = false;

  for (size_t index = 0; index < line.size(); index++) {
    char token = line[index];

    if (is_whitespace(token) && in_string == false) {
      if (status == STATUS_MAP[STATUS::PENDING]) {
        if (is_command(slice)) {
          statement.command = slice;
          status = STATUS_MAP[STATUS::BODY_DEFINITION];
          slice = "";
          continue;
        } 

        throw runtime_error("Unknown Command: " + slice);
      }

      if (status == STATUS_MAP[STATUS::MODIFIER_DEFINITION]) {
        if (current_modifier_directive.length() != 0) {
          throw runtime_error("Missing Modifier Value for: " + current_modifier_directive);
        }

        if (is_modifier_directive(slice)) {
          current_modifier_directive = slice;
          statement.modifiers.push_back({ slice, "", "", "" });
          slice = "";
          continue;
        }
      }

      if (status == STATUS_MAP[STATUS::MODIFIER_PROPERTY_REFERENCE]) {
        if (slice.empty()) {
          throw runtime_error("Invalid Property: " + slice);
        }

        statement.modifiers[statement.modifiers.size() - 1].property = slice;
        status = STATUS_MAP[STATUS::MODIFIER_OPERATOR];
        slice = "";
        continue;
      }

      if (status == STATUS_MAP[STATUS::MODIFIER_VALUE]) {
        if (slice.empty()) {
          continue;
        }

        statement.modifiers[statement.modifiers.size() - 1].value = slice;
        status = STATUS_MAP[STATUS::MODIFIER_DEFINITION];
        current_modifier_directive = "";
        slice = "";
        continue;
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
      if (
        status != STATUS_MAP[STATUS::PROPERTY_VALUE]
        && status != STATUS_MAP[STATUS::MODIFIER_VALUE]
      ) {
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
        if (slice.empty()) {
          println(line);
          string property = get_last(statement.properties).name;
          throw runtime_error("Missing Property Value: " + property);
        }

        statement.properties[statement.properties.size() - 1].value = slice;
        status = STATUS_MAP[STATUS::PROPERTY_DEFINITION];
        in_string = false;
        in_string_closed = false;
        slice = "";
        continue;
      } 

      if (statement.command == COMMAND_MAP[COMMAND::SELECT]) {
        if (slice.empty()) {
          throw runtime_error("Invalid Property: " + slice);
        }
        
        statement.properties.push_back({ slice, "true" });
        status = STATUS_MAP[STATUS::PROPERTY_DEFINITION];
        in_string = false;
        in_string_closed = false;
        slice = "";
        continue;
      }

      throw runtime_error("Unexpected ','");
    } else if (token == TOKEN_MAP[TOKEN::END_DEFINITION]) {
      if (status == STATUS_MAP[STATUS::PROPERTY_VALUE]) {
        if (slice.empty()) {
          println(line);
          string property = statement.properties[statement.properties.size() - 1].name;
          throw runtime_error("Missing Property Value: " + property);
        }
        
        statement.properties[statement.properties.size() - 1].value = slice;
        status = STATUS_MAP[STATUS::MODIFIER_DEFINITION];
        slice = "";

        if (statement.command == COMMAND_MAP[COMMAND::INSERT]) {
          return statement;
        }
      }

      if (status == STATUS_MAP[STATUS::PROPERTY_DEFINITION]) {
        if (statement.command == COMMAND_MAP[COMMAND::SELECT]) {
          if (slice.empty()) {
            throw runtime_error("Invalid Property: " + slice);
          }
          
          statement.properties.push_back({ slice, "true" });
          status = STATUS_MAP[STATUS::MODIFIER_DEFINITION];
          slice = "";
          continue;
        }   

        if (token == TOKEN_MAP[TOKEN::END_PROPERTY]) {
          println(line);
          throw runtime_error("Unexpected ','");
        }

        throw runtime_error("Property Missing Value: " + slice);
      }

      throw runtime_error("Unexpected '}'");
    } else if (token == TOKEN_MAP[TOKEN::PROPERTY_REFERENCE]) {
      if (status != STATUS_MAP[STATUS::MODIFIER_DEFINITION]) {
        throw runtime_error("Unexpected '.'");
      }

      if (current_modifier_directive == MODIFIER_DIRECTIVE_MAP[MODIFIER_DIRECTIVE::LIMIT]) {
        throw runtime_error("Modifier Directive 'limit' does not accept a property reference");
      }

      status = STATUS_MAP[STATUS::MODIFIER_PROPERTY_REFERENCE];
    } else if (is_operator_char(token)) {
      if (status != STATUS_MAP[STATUS::MODIFIER_OPERATOR]) {
        throw runtime_error("Unexpected Operator: " + string(1, token));
      }

      statement.modifiers[statement.modifiers.size() - 1].operator_str = string(1, token);
      status = STATUS_MAP[STATUS::MODIFIER_VALUE];
      slice = "";
      continue;
    } else if (token == TOKEN_MAP[TOKEN::END_STATEMENT]) {
      if (status == STATUS_MAP[STATUS::PROPERTY_VALUE]) {
        println(line);
        throw runtime_error("Missing '}' Instruction Body Termination Marker");
      }

      if (current_modifier_directive == MODIFIER_DIRECTIVE_MAP[MODIFIER_DIRECTIVE::LIMIT]) {
        if (slice.empty()) {
          println(line);
          throw runtime_error("Missing Modifier Value for: " + current_modifier_directive);
        }

        statement.modifiers[statement.modifiers.size() - 1].value = slice;
        return statement;
      }

      if (status == STATUS_MAP[STATUS::MODIFIER_VALUE]) {
        if (slice.empty()) {
          println(line);
          throw runtime_error("Missing Modifier Value for: " + current_modifier_directive);
        }

        statement.modifiers[statement.modifiers.size() - 1].value = slice;
        return statement;
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
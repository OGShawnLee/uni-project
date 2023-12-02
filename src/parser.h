#ifndef PARSER_H
#define PARSER_H

#include "string"
#include "types.h"
#include "utils.h"

using namespace std;

Statement parse_single_line_statement(string line) {
  Statement statement;
  string stage = STAGE_MAP[STAGE::PENDING];
  string current_modifier_directive;
  string slice;
  char previous_token;
  bool in_string = false;
  bool in_string_closed = false;

  for (size_t index = 0; index < line.length(); index++) {
    char token = line[index];

    if (is_whitespace(token) && in_string == false) {
      if (stage == STAGE_MAP[STAGE::BODY_DEFINITION]) continue;

      if (stage == STAGE_MAP[STAGE::PENDING]) {
        if (statement.command.empty()) {
          if (is_command(slice)) {
            statement.command = slice;
            slice = "";
            continue;
          }

          throw runtime_error("Unknown Command: " + slice);
        } 

        if (statement.entity.empty()) {
          if (is_entity(slice) == false) {
            throw runtime_error("Unknown Entity: " + slice);
          }

          statement.entity = slice;
          slice = "";
          continue;
        }
      }

      slice = trim(slice);
      if (is_modifier_directive(slice)) {
        stage = STAGE_MAP[STAGE::MODIFIER_DEFINITION];
        current_modifier_directive = slice;
        statement.modifiers.push_back({ slice, "", "", "" });
        slice = "";
        continue;
      }
      
      if (is_operator(slice)) {
        if (current_modifier_directive.empty()) {
          throw runtime_error("Unexpected Operator: " + slice);
        }

        if (previous_token != TOKEN_MAP[TOKEN::PROPERTY_REFERENCE]) {
          throw runtime_error("Missing Property Reference for: " + current_modifier_directive);
        }

        statement.modifiers[statement.modifiers.size() - 1].operator_str = slice;
        previous_token = TOKEN_MAP[TOKEN::OPERATOR_FOUND];
        slice = "";
        continue;
      }

      if (previous_token == TOKEN_MAP[TOKEN::PROPERTY_REFERENCE]) {          
        statement.modifiers[statement.modifiers.size() - 1].property = slice;
        slice = "";
        continue;
      }
    }

    if (token == TOKEN_MAP[TOKEN::START_DEFINITION] && in_string == false) {
      stage = STAGE_MAP[STAGE::BODY_DEFINITION];
      previous_token = TOKEN_MAP[TOKEN::START_DEFINITION];
      slice = "";
      continue;
    }

    if (token == TOKEN_MAP[TOKEN::START_PROPERTY] && in_string == false) {
      if (stage != STAGE_MAP[STAGE::BODY_DEFINITION]) {
        throw runtime_error("Unexpected ':'");
      }

      statement.properties.push_back({ slice, "" });
      previous_token = TOKEN_MAP[TOKEN::START_PROPERTY];
      slice = "";
      continue;
    }

    if (token == TOKEN_MAP[TOKEN::STRING_MARKER]) {
      in_string = !in_string;
    }

    if (token == TOKEN_MAP[TOKEN::END_PROPERTY] && in_string == false) {
      if (stage != STAGE_MAP[STAGE::BODY_DEFINITION]) {
        throw runtime_error("Unexpected ','");
      }

      if (slice.empty()) {
        throw runtime_error("Missing Property Value: " + get_last(statement.properties).name);
      }

      if (previous_token != TOKEN_MAP[TOKEN::START_PROPERTY] && statement.command != COMMAND_MAP[COMMAND::SELECT]) {
        throw runtime_error("Unexpected ','");
      } 

      if (previous_token == TOKEN_MAP[TOKEN::START_PROPERTY]) {
        statement.properties[statement.properties.size() - 1].value = slice;
        previous_token = TOKEN_MAP[TOKEN::END_PROPERTY];
        slice = "";
        continue;
      }

      if (statement.command == COMMAND_MAP[COMMAND::SELECT]) {
        statement.properties.push_back({ slice, "true" });
        previous_token = TOKEN_MAP[TOKEN::END_PROPERTY];
        slice = "";
        continue;
      }

      throw runtime_error("Unexpected ','");
    }

    if (token == TOKEN_MAP[TOKEN::END_DEFINITION] && in_string == false) {
      if (stage != STAGE_MAP[STAGE::BODY_DEFINITION]) {
        throw runtime_error("Unexpected '}'");
      }

      if (slice.empty()) {
        throw runtime_error("Missing Property Value: " + get_last(statement.properties).name);
      }

      if (previous_token == TOKEN_MAP[TOKEN::START_PROPERTY]) {
        if (slice.empty()) {
          throw runtime_error("Missing Property Value: " + get_last(statement.properties).name);
        }
        
        statement.properties[statement.properties.size() - 1].value = slice;
        previous_token = TOKEN_MAP[TOKEN::END_DEFINITION];
        slice = "";
        continue;
      }

      // SELECT <ENTITY> { <PPOPERTY> }... 
      if (statement.command == COMMAND_MAP[COMMAND::SELECT]) {
        statement.properties.push_back({ slice, "true" });
        stage = STAGE_MAP[STAGE::MODIFIER_DEFINITION];
        previous_token = TOKEN_MAP[TOKEN::END_DEFINITION];
        slice = "";
        continue;
      }

      throw runtime_error("Unexpected '}'");
    }

    if (token == TOKEN_MAP[TOKEN::PROPERTY_REFERENCE] && in_string == false) {
      if (stage != STAGE_MAP[STAGE::MODIFIER_DEFINITION]) {
        if (statement.command != COMMAND_MAP[COMMAND::SELECT]) {
          throw runtime_error("Unexpected '.'");
        }

          if (slice.empty()) {
            throw runtime_error("Missing Modifier Directity: " + slice);
          }

          if (is_modifier_directive(slice) == false) {
            throw runtime_error("Unknown Modifier Directive: " + slice);
          }

        // SELECT <ENTITY> FILTER .<PROPERTY>...
          current_modifier_directive = slice;
          statement.modifiers.push_back({ slice, "", "", "" });
          stage = STAGE_MAP[STAGE::MODIFIER_DEFINITION];          
      }

      if (current_modifier_directive == MODIFIER_DIRECTIVE_MAP[MODIFIER_DIRECTIVE::LIMIT]) {
        throw runtime_error("Modifier Directive 'limit' does not accept a property reference");
      }

      if (previous_token == TOKEN_MAP[TOKEN::PROPERTY_REFERENCE]) {
        throw runtime_error("Duplicate Property Reference");
      }

      previous_token = TOKEN_MAP[TOKEN::PROPERTY_REFERENCE];
      slice = "";
      continue;
    }

    if (token == TOKEN_MAP[TOKEN::END_STATEMENT] && in_string == false) {
      if (stage == STAGE_MAP[STAGE::PENDING]) {
        if (statement.command != COMMAND_MAP[COMMAND::SELECT]) {
          throw runtime_error("Missing Body Definition for Command: " + statement.command);
        }

        if (statement.entity.empty() == false) return statement;

        if (slice.empty()) {
          throw runtime_error("Invalid Entity: " + slice);
        }

        if (is_entity(slice) == false) {
          throw runtime_error("Invalid Entity: " + slice);
        }

        statement.entity = slice;
        return statement;
      }

      if (stage == STAGE_MAP[STAGE::MODIFIER_DEFINITION]) {
        if (current_modifier_directive == MODIFIER_DIRECTIVE_MAP[MODIFIER_DIRECTIVE::LIMIT]) {
          if (slice.empty()) {
            throw runtime_error("Missing Modifier Value for: " + current_modifier_directive);
          }

          statement.modifiers[statement.modifiers.size() - 1].value = slice;
          return statement;
        }

        if (current_modifier_directive.empty()) {
          return statement;
        }

        if (slice.empty()) {
          throw runtime_error("Invalid Property Reference Value" + slice);
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
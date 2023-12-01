#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <string>
#include <map>
#include <iostream>

using namespace std;

enum class Entity {
  TOWN,
};

map<Entity, string> ENTITY_MAP = {
  {Entity::TOWN, "Town"},
};

bool is_entity(string str) {
  for (auto &pair : ENTITY_MAP) {
    if (pair.second == str) return true;
  }
  return false;
}

enum class COMMAND {
  DELETE,
  UPDATE,
  SELECT,
  INSERT,
};

map<COMMAND, string> COMMAND_MAP = {
  {COMMAND::DELETE, "delete"},
  {COMMAND::UPDATE, "update"},
  {COMMAND::SELECT, "select"},
  {COMMAND::INSERT, "insert"},
}; 

bool is_command(string str) {
  for (auto &pair : COMMAND_MAP) {
    if (pair.second == str) return true;
  }
  return false;
}

enum MODIFIER_DIRECTIVE {
  FILTER,
  FILTER_SINGLE,
  ORDER_BY,
  LIMIT,
};

map<MODIFIER_DIRECTIVE, string> MODIFIER_DIRECTIVE_MAP = {
  {MODIFIER_DIRECTIVE::FILTER, "filter"},
  {MODIFIER_DIRECTIVE::FILTER_SINGLE, "filter_single"},
  {MODIFIER_DIRECTIVE::ORDER_BY, "order_by"},
  {MODIFIER_DIRECTIVE::LIMIT, "limit"},
};

bool is_modifier_directive(string str) {
  if (str.length() < 5) return false;
  for (auto &pair : MODIFIER_DIRECTIVE_MAP) {
    if (pair.second == str) return true;
  }
  return false;
}

enum class OPERATOR {
  EQUAL,
  NOT_EQUAL,
  GREATER_THAN,
  LESS_THAN,
  GREATER_THAN_OR_EQUAL,
  LESS_THAN_OR_EQUAL,
};

map<OPERATOR, string> OPERATOR_MAP = {
  {OPERATOR::EQUAL, "="},
  {OPERATOR::NOT_EQUAL, "!="},
  {OPERATOR::GREATER_THAN, ">"},
  {OPERATOR::LESS_THAN, "<"},
  {OPERATOR::GREATER_THAN_OR_EQUAL, ">="},
  {OPERATOR::LESS_THAN_OR_EQUAL, "<="},
};

map<OPERATOR, char>OPERATOR_CHAR_MAP = {
  {OPERATOR::EQUAL, '='},
  {OPERATOR::NOT_EQUAL, '!'},
  {OPERATOR::GREATER_THAN, '>'},
  {OPERATOR::LESS_THAN, '<'},
};

bool is_operator_char(char c) {
  for (auto &pair : OPERATOR_CHAR_MAP) {
    if (pair.second == c) return true;
  }
  return false;
}

bool is_operator(string str) {
  cout << "Operator:" << str << endl;
  for (auto &pair : OPERATOR_MAP) {
    if (pair.second == str) return true;
  }
  return false;
}

enum STATUS {
  PENDING,
  BODY_DEFINITION,
  PROPERTY_DEFINITION,
  PROPERTY_VALUE,
  MODIFIER_DEFINITION,
  MODIFIER_PROPERTY_REFERENCE,
  MODIFIER_OPERATOR,
  MODIFIER_VALUE,
};

map<STATUS, string> STATUS_MAP = {
  {STATUS::PENDING, "pending"},
  {STATUS::BODY_DEFINITION, "instruction-definition"},
  {STATUS::PROPERTY_DEFINITION, "property-definition"},
  {STATUS::PROPERTY_VALUE, "property-value"},
  {STATUS::MODIFIER_DEFINITION, "modifier-definition"},
  {STATUS::MODIFIER_PROPERTY_REFERENCE, "modifier-property-reference"},
  {STATUS::MODIFIER_OPERATOR, "modifier-operator"},
  {STATUS::MODIFIER_VALUE, "modifier-value"},
};

enum TOKEN {
  START_DEFINITION,
  END_DEFINITION,
  START_PROPERTY,
  END_PROPERTY,
  PROPERTY_REFERENCE,
  END_STATEMENT,
  STRING_MARKER,
  SPLAT,
};

map<TOKEN, char> TOKEN_MAP = {
  {TOKEN::START_DEFINITION, '{'},
  {TOKEN::END_DEFINITION, '}'},
  {TOKEN::END_PROPERTY, ','},
  {TOKEN::START_PROPERTY, ':'},
  {TOKEN::PROPERTY_REFERENCE, '.'},
  {TOKEN::END_STATEMENT, ';'},
  {TOKEN::STRING_MARKER, '"'},
  {TOKEN::SPLAT, '*'},
};

bool is_token(char c) {
  for (auto &pair : TOKEN_MAP) {
    if (pair.second == c) return true;
  }
  return false;
}

struct Modifier {
  string directive;
  string operator_str;
  string property;
  string value;
};

struct Property {
  string name;
  string value;
};

struct Statement {
  string command;
  string entity;
  vector<Property> properties;  
  vector<Modifier> modifiers;
};

struct Town {
  string id;
  string name;
  string state;
  int year_up;
  int year_down;
  bool is_magic_town;
  float federal_budget;
};

#endif
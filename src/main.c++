#include <iostream>
#include "parser.h"

using namespace std;

int main() {
  auto statements = parse("db.ssdl");

  for (auto &statement : statements) {
    println(statement.command);
    println(statement.entity);

    for (auto &property : statement.properties) {
      println(property.name + ": " + property.value);
    }

    for (auto &modifier : statement.modifiers) {
      if (modifier.directive == MODIFIER_DIRECTIVE_MAP[MODIFIER_DIRECTIVE::LIMIT]) {
        println(modifier.directive + " " + modifier.value);
        continue;
      }

      println(modifier.directive + " " + modifier.property + " " + modifier.operator_str + " " + modifier.value);
    }
  }

  return 0;
}
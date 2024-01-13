#ifndef DB_H
#define DB_H

#include <string>
#include "utils.h"

using namespace std;

struct Town {
  short id;
  std::string name;
  std::string state;
  float budget;
  int year_promotion;
  int year_demotion;
  bool is_magic_town;
};

enum Option {
  CREATE,
  INVALID,
  EXIT,
};

namespace db {
  Town records[50];

  short count = 0;

  Option get_option() {
    println("What would you like to do?");
    println("1. Create a town");
    println("2. Exit");

    int option = get_input<int>("Enter an option");

    switch (option) {
      case 1: 
        return CREATE;
      case 2: 
        return EXIT;
      default: 
        return INVALID;
    }
  }

  short insert(Town &town) {
    town.id = count;
    records[count++] = town;
    return town.id;
  }

  // Option 1: Create a town
  void create() {
    Town town;
    town.name = get_string("What is the name of the town?");
    town.state = get_string("What state is the town in?");
    town.year_promotion = get_input<int>("What year was the town promoted?");
    town.is_magic_town = get_bool("Is the town a magic town?");

    if (town.is_magic_town) {
      town.budget = get_input<float>("What is the town's budget? (millions)");
    } else {
      town.year_demotion = get_input<int>("What year was the town demoted?");
      town.budget = get_input<float>("What was the town's budget? (millions)");
    }

    short id = insert(town);
    println("Town created with id " + to_string(id));
  }
}

#endif
#include "db.h"
#include "utils.h"

using namespace std;

int main() {
  println("Welcome to the Magic Towns Database");
  
  do {
    Option option = db::get_option();
    switch (option) {
      case CREATE: {
        db::create(); 
        break;
      }
      case EXIT: {
        println("Goodbye!");
        return 0;
      }
      default: {
        println("Invalid option");
      }
    }
  } while (true);

  return 0;
}
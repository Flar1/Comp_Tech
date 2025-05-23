#include "cool-parse.h"
#include "cool-tree.h"
#include "utilities.h"
#include <cstdio>
#include <unistd.h>

#include <unordered_set>

std::FILE *token_file = stdin;
extern Classes parse_results;
extern Program ast_root;

extern int curr_lineno;
const char *curr_filename = "<stdin>";
extern int parse_errors;

// Debug flags
extern int yy_flex_debug;
extern int cool_yydebug;
int lex_verbose = 0;

extern int cool_yyparse();

namespace semantic {

  

void check_builtin_type(std::string type, Expression expr) {
  std::string expr_type = expr->get_expr_type();
  if (expr_type == "no_expr_class") {
    return;
  }
  if (type == "Int" && expr_type != "int_const_class") {
    std::cerr
        << "Semantic Error! initialization of Int with non-integer value\n";
  } else if (type == "Bool" && expr_type != "bool_const_class") {
    std::cerr
        << "Semantic Error! initialization of Bool with non-boolean value\n";
  } else if (type == "String" && expr_type != "string_const_class") {
    std::cerr
        << "Semantic Error! initialization of String with non-string value\n";
  }
}


class__class *find_class(std::string name, Classes classes) {
  for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
    class__class *cur_class = dynamic_cast<class__class *>(classes->nth(i));
    if (name == cur_class->get_name()->get_string()) {
      return cur_class;
    }
  }
  return nullptr;
}

bool check_signatures(method_class *m1, method_class *m2) {
  if (m1->get_type() != m2->get_type()) {
    return false;
  }

  Formals m1_formals = m1->get_formals();
  Formals m2_formals = m2->get_formals();
  if (m1_formals->len() != m2_formals->len()) {
    return false;
  }

  for (int i = m1_formals->first(); m1_formals->more(i);
       i = m1_formals->next(i)) {
    formal_class *m1_formal = dynamic_cast<formal_class *>(m1_formals->nth(i));
    formal_class *m2_formal = dynamic_cast<formal_class *>(m2_formals->nth(i));

    std::string name1 = m1_formal->get_name()->get_string();
    std::string name2 = m2_formal->get_name()->get_string();
    if (name1 != name2) {
      return false;
    }

    std::string type1 = m1_formal->get_type()->get_string();
    std::string type2 = m1_formal->get_type()->get_string();
    if (type1 != type2) {
      return false;
    }
  }
  return true;
}

bool dfs_visit(
    const std::string &class_name,
    const std::unordered_map<std::string, std::string> &classes_hierarchy,
    std::unordered_set<std::string> &visited,
    std::unordered_set<std::string> &rec_stack) {

  visited.insert(class_name);
  rec_stack.insert(class_name);

  auto it = classes_hierarchy.find(class_name);
  if (it != classes_hierarchy.end()) {
    const std::string &parent = it->second;

    if (parent != "Object") {
      if (visited.find(parent) == visited.end()) {
        if (dfs_visit(parent, classes_hierarchy, visited, rec_stack)) {
          return true;
        }
      }
      else if (rec_stack.find(parent) != rec_stack.end()) {
        return true;
      }
    }
  }

  rec_stack.erase(class_name);
  return false;
}

bool detect_cycle(const std::unordered_map<std::string, std::string> &classes_hierarchy) {
  std::unordered_set<std::string> visited;
  std::unordered_set<std::string> rec_stack;

  for (const auto &[class_name, _] : classes_hierarchy) {
    if (visited.find(class_name) == visited.end()) {
      if (dfs_visit(class_name, classes_hierarchy, visited, rec_stack)) {
        return true;
      }
    }
  }
  return false;
}

bool class_has_method(const std::string& class_name, const std::string& method_name, Classes all_classes) {
    for (int i = all_classes->first(); all_classes->more(i); i = all_classes->next(i)) {
        Class_ cls = all_classes->nth(i);
        if (cls->get_name()->get_string() == class_name) {
            Features feats = cls->get_features();
            for (int j = feats->first(); feats->more(j); j = feats->next(j)) {
                Feature f = feats->nth(j);
                if (f->get_feature_type() == "method_class") {
                    if (f->get_name()->get_string() == method_name) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

std::string get_method_return_type(const std::string& class_name, const std::string& method_name, Classes all_classes) {
    for (int i = all_classes->first(); all_classes->more(i); i = all_classes->next(i)) {
        Class_ cls = all_classes->nth(i);
        if (cls->get_name()->get_string() == class_name) {
            Features feats = cls->get_features();
            for (int j = feats->first(); feats->more(j); j = feats->next(j)) {
                Feature f = feats->nth(j);
                if (f->get_feature_type() == "method_class") {
                    if (f->get_name()->get_string() == method_name) {
                        return f->get_type()->get_string();
                    }
                }
            }
        }
    }
    return "error";
}


std::string infer_expr_type(Expression expr, const std::unordered_set<std::string> &scope_vars) {
    if (!expr) return "no_expr_class";

    std::string expr_type = expr->get_expr_type();
  
    if (expr_type == "object_class") {
        std::string var_name = expr->get_name();
        if (scope_vars.find(var_name) == scope_vars.end()) {
            std::cerr << "Semantic Error! Undefined variable '" << var_name << "' used.\n";
            return "error";
        }
        return "Object";
    } else if (expr_type == "int_const_class") {
        return "Int";
    } else if (expr_type == "bool_const_class") {
        return "Bool";
    } else if (expr_type == "string_const_class") {
        return "String";
    } else if (expr_type == "plus_class") {
        plus_class* plus_expr = dynamic_cast<plus_class*>(expr);
        std::string lhs_type = infer_expr_type(plus_expr->get_e1(), scope_vars);
        std::string rhs_type = infer_expr_type(plus_expr->get_e2(), scope_vars);
        if (lhs_type != "Int" || rhs_type != "Int") {
            std::cerr << "Semantic Error! '+' operator applied to non-integer types.\n";
        }
        return "Int";
    } else if (expr_type == "block_class") {
        block_class* block_expr = dynamic_cast<block_class*>(expr);
        Expressions exprs = block_expr->get_expressions();
        std::string last_type = "Object";
        for (int i = exprs->first(); exprs->more(i); i = exprs->next(i)) {
            last_type = infer_expr_type(exprs->nth(i), scope_vars);
        }
        return last_type;
    } 
    else if (expr_type == "let_class") {
    let_class* let_expr = dynamic_cast<let_class*>(expr);
    if (!let_expr) return "error";

    std::string init_type = "Object";
    if (let_expr->get_init()) {
        init_type = infer_expr_type(let_expr->get_init(), scope_vars);
    }

    auto new_scope = scope_vars;
    std::string var_name = let_expr->get_name();
    new_scope.insert(var_name);

        if (scope_vars.find(var_name) != scope_vars.end()) {
        std::cerr << "Semantic Error! Variable '" << var_name << "' is already defined in the current scope.\n";
    }

    return infer_expr_type(let_expr->get_body(), new_scope);

  } else if (expr_type == "dispatch_class" || expr_type == "static_dispatch_class") {
    dispatch_class* disp_expr = dynamic_cast<dispatch_class*>(expr);
    Expression caller = disp_expr->get_expr();
    Symbol method_name = disp_expr->get_name_symbol();
    Expressions actuals = disp_expr->get_actual();

    std::string caller_type = infer_expr_type(caller, scope_vars);

    if (caller_type == "error") return "error";

    if (!class_has_method(caller_type, method_name->get_string(), parse_results)) {
        std::cerr << "Semantic Error! Class '" << caller_type << "' has no method '"
                  << method_name->get_string() << "'.\n";
        return "error";
    }

    std::string return_type = get_method_return_type(caller_type, method_name->get_string(), parse_results);

    return return_type;
}

    return "Object";
}


}

int main(int argc, char **argv) {
  yy_flex_debug = 0;
  cool_yydebug = 0;
  lex_verbose = 0;

  for (int i = 1; i < argc; i++) {
    token_file = std::fopen(argv[i], "r");
    if (token_file == NULL) {
      std::cerr << "Error: can not open file " << argv[i] << std::endl;
      std::exit(1);
    }
    curr_lineno = 1;

    cool_yyparse();
    if (parse_errors != 0) {
      std::cerr << "Error: parse errors\n";
      std::exit(1);
    }

    // Добавление в AST класса (узла) для встроенных типов
    Symbol filename = stringtable.add_string("<builtin-classes>");
    Symbol Object = idtable.add_string("Object");
    Symbol Bool = idtable.add_string("Bool");
    Symbol Int = idtable.add_string("Int");
    Symbol String = idtable.add_string("String");
    Symbol SELF_TYPE = idtable.add_string("SELF_TYPE");
    Symbol Main = idtable.add_string("Main");

    std::unordered_set<Symbol> not_inherited{Bool, Int, String, SELF_TYPE};
    std::unordered_set<Symbol> classes_names{
        Object, Bool, Int, String, SELF_TYPE};
    std::unordered_map<std::string, std::string> classes_hierarchy;

    std::vector<std::string> classes;
    std::vector<std::string> parents;
    bool has_main_method = false;
    for (int i = parse_results->first(); parse_results->more(i);
         i = parse_results->next(i)) {
      Symbol class_name = parse_results->nth(i)->get_name();
      // проверка корректности имён классов
      auto result = classes_names.insert(class_name);
      if (!result.second) {
        std::cerr << "Semantic Error! class '" << class_name->get_string()
                  << "' redeclared.\n";
      }
      classes.push_back(class_name->get_string());

      Symbol parent_name = parse_results->nth(i)->get_parent();
      // проверка корректности имён родительских классов
      if (not_inherited.find(parent_name) != not_inherited.end()) {
        std::cerr << "Semantic Error! class '" << class_name->get_string()
                  << "': can't use parent class '" << parent_name->get_string()
                  << "' (builtin)\n";
      }
      // формируем таблицу class <-> parent
      classes_hierarchy[class_name->get_string()] = parent_name->get_string();
      if (std::string(parent_name->get_string()) != "Object") {
        if (!classes_hierarchy.contains(parent_name->get_string())) {
          std::cerr << "Semantic Error! Unknown parent of class '"
                    << class_name->get_string() << "' - '"
                    << parent_name->get_string() << "'\n";
        }
      }

      // проверяем методы класса
      Features features = parse_results->nth(i)->get_features();
      std::unordered_set<std::string> features_names;
      for (int j = features->first(); features->more(j);
           j = features->next(j)) {
        Feature feature = features->nth(j);
        std::string feature_name = feature->get_name()->get_string();

        if (feature_name == "self") {
          std::cerr << "Semantic Error! can't use 'self' as feature name\n";
        }

        auto res = features_names.insert(feature_name);
        if (!res.second) {
          std::cerr << "Semantic Error! feature '" << feature_name << "' in '"
                    << class_name << "' already exists!" << '\n';
        }

        Symbol type = feature->get_type();
        if (classes_names.find(type) == classes_names.end()) {
          std::cerr << "Semantic Error! Unknown type '" << type << "' in "
                    << feature_name << '\n';
        }

        if (feature->get_feature_type() == "method_class") {
          // Check method overrides - must have same signature
          if (std::string(parent_name->get_string()) != "Object") {
            class__class *parent =
                semantic::find_class(parent_name->get_string(), parse_results);

            if (parent) {
              Features parent_features = parent->get_features();

              // Loop through parent features
              for (int a = parent_features->first(); parent_features->more(a);
                   a = parent_features->next(a)) {
                Feature parent_feature = parent_features->nth(a);

                // Get feature name
                std::string parent_feature_name =
                    parent_feature->get_name()->get_string();

                // If there is parent feature with same name
                if (parent_feature_name == feature_name) {
                  // Check method signatures
                  method_class *cur_method =
                      dynamic_cast<method_class *>(feature);
                  method_class *parent_method =
                      dynamic_cast<method_class *>(parent_feature);
                  if (!semantic::check_signatures(cur_method, parent_method)) {
                    std::cerr
                        << "Semantic Error! '" << feature_name
                        << "' method from class '" << parent_name->get_string()
                        << "' doesn't match override version of it in class '"
                        << class_name << "'";
                  }
                }
              }
            } else {
              std::cerr << "Semantic Error! Unknown parent of class '"
                        << class_name->get_string() << "' - '"
                        << parent_name->get_string() << "'\n";
            }
          }
          Formals formals = feature->get_formals();
          // Local formals names
          std::unordered_set<std::string> formals_names;
          std::unordered_set<std::string> scope_vars;
            for (int k = formals->first(); formals->more(k); k = formals->next(k)) {
                scope_vars.insert(formals->nth(k)->get_name()->get_string());
            }

            // Check body expression
            Expression expr = feature->get_expr();
            std::string expr_type = semantic::infer_expr_type(expr, scope_vars);

            std::string declared_type = feature->get_type()->get_string();
            if (expr_type != "error" && expr_type != "no_expr_class" && expr_type != declared_type) {
                std::cerr << "Semantic Error! Method '" << feature_name << "' body type '" 
                          << expr_type << "' does not match declared return type '" 
                          << declared_type << "'.\n";
}
          
          // Loop through formals
          for (int k = formals->first(); formals->more(k);
               k = formals->next(k)) {

            std::string formal_name = formals->nth(k)->get_name()->get_string();

            // 'self' name check
            if (formal_name == "self") {
              std::cerr << "Semantic Error! can't use 'self' as formal name\n";
            }

            // Unique name check
            auto f_result = formals_names.insert(formal_name);
            if (!f_result.second) {
              std::cerr << "Semantic Error! formal '" << formal_name
                        << "' in method '" << feature_name
                        << "' already exists!\n";
            }

            type = formals->nth(k)->get_type();

            // Check formal type
            if (classes_names.find(type) == classes_names.end()) {
              std::cerr << "Semantic Error! Unknown type '"
                        << type->get_string() << "' in " << formal_name << '\n';
            }

            // Get method expression
            Expression expr = features->nth(j)->get_expr();

            // block_class check
            if (expr->get_expr_type() == "block_class") {

              // Get expressions from block
              Expressions exprs = expr->get_expressions();

              // Block expressions check
              for (int l = exprs->first(); exprs->more(l); l = exprs->next(l)) {
                Expression current = exprs->nth(l);

                // let
                if (current->get_expr_type() == "let_class") {

                  // Get let-expr variable name
                  formal_name = current->get_name();

                  // 'self' name check
                  if (formal_name == "self") {
                    std::cerr
                        << "Semantic Error! can't use 'self' as formal name\n";
                  }

                  // Check unique of nested formal
                  f_result = formals_names.insert(formal_name);
                  if (!f_result.second) {
                    std::cerr << "Semantic Error! formal '" << formal_name
                              << "' in method '" << feature_name << "' from '"
                              << class_name->get_string()
                              << "' already exists!\n";
                  }

                  // Let-expr formal type check
                  type = current->get_type_decl();
                  if (classes_names.find(type) == classes_names.end()) {
                    std::cerr << "Semantic Error! Unknown type '"
                              << type->get_string() << "' in " << formal_name
                              << '\n';
                  }
                }
              }
            }
          }
        } else { 
          attr_class *attr = dynamic_cast<attr_class *>(feature);
          semantic::check_builtin_type(
              attr->get_type()->get_string(), attr->get_expr());
        }
      }
      // Check existence of method main in class Main

      for (int i = parse_results->first(); parse_results->more(i); i = parse_results->next(i)) {
        Features features = parse_results->nth(i)->get_features();

        for (int j = features->first(); features->more(j); j = features->next(j)) {
          Feature f = features->nth(j);

          if (f->get_feature_type() == "method_class" &&
              std::string(f->get_name()->get_string()) == "main") {
            has_main_method = true;
            break;
          }
        }

        if (has_main_method) break;
      }
    }
          if (!has_main_method) {
        std::cerr << "Semantic Error! No class contains method 'main'\n";
      }

    if (semantic::detect_cycle(classes_hierarchy)) {
      std::cerr
          << "Semantic Error! loop detected in classes inheritance hierarchy\n";
      std::cerr << "\\ program classes' hierarchy (child : parent)\n";
      for (auto p : classes_hierarchy) {
        std::cerr << '\t' << p.first << " : " << p.second << "\n";
      }
    }

    std::fclose(token_file);
  }
}
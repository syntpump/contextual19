# Ctx19 C Interpreter: Draft

This path contains some files that might be useful in developing Ctx19 C Interpreter. This project will require some format to have sentence inputed. For now you can just have a list of positions of rules in `.ctx19` file, using following code:
```c
FILE *inpfile;
fopen("...", "r");
struct RulesAdjacentList* rules = findRules(inpfile);
```

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Forward declarations
typedef struct ASTNode ASTNode;
typedef struct Expression Expression;
typedef struct Statement Statement;

// Enums for node types
typedef enum {
    NODE_PROGRAM,
    NODE_IDENTIFIER,
    NODE_LITERAL,
    NODE_BINARY_EXPRESSION,
    NODE_UNARY_EXPRESSION,
    NODE_ASSIGNMENT_EXPRESSION,
    NODE_CALL_EXPRESSION,
    NODE_MEMBER_EXPRESSION,
    NODE_ARRAY_EXPRESSION,
    NODE_OBJECT_EXPRESSION,
    NODE_PROPERTY,
    NODE_CONDITIONAL_EXPRESSION,
    NODE_EXPRESSION_STATEMENT,
    NODE_VARIABLE_DECLARATION,
    NODE_VARIABLE_DECLARATOR,
    NODE_FUNCTION_DECLARATION,
    NODE_PARAMETER,
    NODE_BLOCK_STATEMENT,
    NODE_RETURN_STATEMENT,
    NODE_IF_STATEMENT,
    NODE_WHILE_STATEMENT,
    NODE_FOR_STATEMENT,
    NODE_BREAK_STATEMENT,
    NODE_CONTINUE_STATEMENT,
    NODE_THROW_STATEMENT,
    NODE_TRY_STATEMENT,
    NODE_CATCH_CLAUSE,
    NODE_SWITCH_STATEMENT,
    NODE_SWITCH_CASE
} NodeType;

typedef enum {
    VAR_KIND_VAR,
    VAR_KIND_LET,
    VAR_KIND_CONST
} VariableKind;

typedef enum {
    SOURCE_SCRIPT,
    SOURCE_MODULE
} SourceType;

// Value types for literals
typedef enum {
    LITERAL_STRING,
    LITERAL_NUMBER,
    LITERAL_BOOLEAN,
    LITERAL_NULL
} LiteralType;

typedef union {
    char* string_value;
    double number_value;
    bool boolean_value;
} LiteralValue;

// Dynamic array structure
typedef struct {
    void** items;
    size_t count;
    size_t capacity;
} Array;

// Base AST Node structure
struct ASTNode {
    NodeType type;
    int line;
    int column;
    struct ASTNode* parent;
};

// Expression structures
typedef struct {
    ASTNode base;
    char* name;
} Identifier;

typedef struct {
    ASTNode base;
    LiteralType literal_type;
    LiteralValue value;
    char* raw;
} Literal;

typedef struct {
    ASTNode base;
    char* operator;
    Expression* left;
    Expression* right;
} BinaryExpression;

typedef struct {
    ASTNode base;
    char* operator;
    Expression* argument;
} UnaryExpression;

typedef struct {
    ASTNode base;
    char* operator;
    Expression* left;
    Expression* right;
} AssignmentExpression;

typedef struct {
    ASTNode base;
    Expression* callee;
    Array arguments; // Array of Expression*
} CallExpression;

typedef struct {
    ASTNode base;
    Expression* object;
    Expression* property;
    bool computed;
} MemberExpression;

typedef struct {
    ASTNode base;
    Array elements; // Array of Expression*
} ArrayExpression;

typedef struct Property {
    ASTNode base;
    Expression* key;
    Expression* value;
} Property;

typedef struct {
    ASTNode base;
    Array properties; // Array of Property*
} ObjectExpression;

typedef struct {
    ASTNode base;
    Expression* test;
    Expression* consequent;
    Expression* alternate;
} ConditionalExpression;

// Statement structures
typedef struct {
    ASTNode base;
    Expression* expression;
} ExpressionStatement;

typedef struct {
    ASTNode base;
    Identifier* id;
    Expression* init;
} VariableDeclarator;

typedef struct {
    ASTNode base;
    Array declarations; // Array of VariableDeclarator*
    VariableKind kind;
} VariableDeclaration;

typedef struct {
    ASTNode base;
    Identifier* name;
    char* param_type;
    Expression* default_value;
} Parameter;

typedef struct {
    ASTNode base;
    Array body; // Array of Statement*
} BlockStatement;

typedef struct {
    ASTNode base;
    Identifier* id;
    Array params; // Array of Parameter*
    BlockStatement* body;
    char* return_type;
} FunctionDeclaration;

typedef struct {
    ASTNode base;
    Expression* argument;
} ReturnStatement;

typedef struct {
    ASTNode base;
    Expression* test;
    Statement* consequent;
    Statement* alternate;
} IfStatement;

typedef struct {
    ASTNode base;
    Expression* test;
    Statement* body;
} WhileStatement;

typedef struct {
    ASTNode base;
    ASTNode* init; // Can be VariableDeclaration or Expression
    Expression* test;
    Expression* update;
    Statement* body;
} ForStatement;

typedef struct {
    ASTNode base;
    Identifier* label;
} BreakStatement;

typedef struct {
    ASTNode base;
    Identifier* label;
} ContinueStatement;

typedef struct {
    ASTNode base;
    Expression* argument;
} ThrowStatement;

typedef struct {
    ASTNode base;
    Identifier* param;
    BlockStatement* body;
} CatchClause;

typedef struct {
    ASTNode base;
    BlockStatement* block;
    CatchClause* handler;
    BlockStatement* finalizer;
} TryStatement;

typedef struct {
    ASTNode base;
    Expression* test; // NULL for default case
    Array consequent; // Array of Statement*
} SwitchCase;

typedef struct {
    ASTNode base;
    Expression* discriminant;
    Array cases; // Array of SwitchCase*
} SwitchStatement;

typedef struct {
    ASTNode base;
    Array body; // Array of Statement*
    SourceType source_type;
} Program;

// Union-like structures using void pointers and type checking
struct Expression {
    ASTNode base;
    // Actual type determined by base.type
};

struct Statement {
    ASTNode base;
    // Actual type determined by base.type
};

// Dynamic array functions
Array* array_create(size_t initial_capacity) {
    Array* arr = malloc(sizeof(Array));
    arr->items = malloc(sizeof(void*) * initial_capacity);
    arr->count = 0;
    arr->capacity = initial_capacity;
    return arr;
}

void array_push(Array* arr, void* item) {
    if (arr->count >= arr->capacity) {
        arr->capacity *= 2;
        arr->items = realloc(arr->items, sizeof(void*) * arr->capacity);
    }
    arr->items[arr->count++] = item;
}

void array_free(Array* arr) {
    free(arr->items);
    free(arr);
}

// AST Builder functions
Identifier* create_identifier(const char* name, int line, int column) {
    Identifier* node = malloc(sizeof(Identifier));
    node->base.type = NODE_IDENTIFIER;
    node->base.line = line;
    node->base.column = column;
    node->base.parent = NULL;
    node->name = strdup(name);
    return node;
}

Literal* create_literal_string(const char* value, const char* raw, int line, int column) {
    Literal* node = malloc(sizeof(Literal));
    node->base.type = NODE_LITERAL;
    node->base.line = line;
    node->base.column = column;
    node->base.parent = NULL;
    node->literal_type = LITERAL_STRING;
    node->value.string_value = strdup(value);
    node->raw = raw ? strdup(raw) : NULL;
    return node;
}

Literal* create_literal_number(double value, const char* raw, int line, int column) {
    Literal* node = malloc(sizeof(Literal));
    node->base.type = NODE_LITERAL;
    node->base.line = line;
    node->base.column = column;
    node->base.parent = NULL;
    node->literal_type = LITERAL_NUMBER;
    node->value.number_value = value;
    node->raw = raw ? strdup(raw) : NULL;
    return node;
}

Literal* create_literal_boolean(bool value, const char* raw, int line, int column) {
    Literal* node = malloc(sizeof(Literal));
    node->base.type = NODE_LITERAL;
    node->base.line = line;
    node->base.column = column;
    node->base.parent = NULL;
    node->literal_type = LITERAL_BOOLEAN;
    node->value.boolean_value = value;
    node->raw = raw ? strdup(raw) : NULL;
    return node;
}

Literal* create_literal_null(const char* raw, int line, int column) {
    Literal* node = malloc(sizeof(Literal));
    node->base.type = NODE_LITERAL;
    node->base.line = line;
    node->base.column = column;
    node->base.parent = NULL;
    node->literal_type = LITERAL_NULL;
    node->raw = raw ? strdup(raw) : NULL;
    return node;
}

BinaryExpression* create_binary_expression(const char* operator, Expression* left, 
                                         Expression* right, int line, int column) {
    BinaryExpression* node = malloc(sizeof(BinaryExpression));
    node->base.type = NODE_BINARY_EXPRESSION;
    node->base.line = line;
    node->base.column = column;
    node->base.parent = NULL;
    node->operator = strdup(operator);
    node->left = left;
    node->right = right;
    return node;
}

UnaryExpression* create_unary_expression(const char* operator, Expression* argument,
                                       int line, int column) {
    UnaryExpression* node = malloc(sizeof(UnaryExpression));
    node->base.type = NODE_UNARY_EXPRESSION;
    node->base.line = line;
    node->base.column = column;
    node->base.parent = NULL;
    node->operator = strdup(operator);
    node->argument = argument;
    return node;
}

CallExpression* create_call_expression(Expression* callee, Array* arguments,
                                     int line, int column) {
    CallExpression* node = malloc(sizeof(CallExpression));
    node->base.type = NODE_CALL_EXPRESSION;
    node->base.line = line;
    node->base.column = column;
    node->base.parent = NULL;
    node->callee = callee;
    node->arguments = *arguments;
    return node;
}

VariableDeclarator* create_variable_declarator(Identifier* id, Expression* init,
                                             int line, int column) {
    VariableDeclarator* node = malloc(sizeof(VariableDeclarator));
    node->base.type = NODE_VARIABLE_DECLARATOR;
    node->base.line = line;
    node->base.column = column;
    node->base.parent = NULL;
    node->id = id;
    node->init = init;
    return node;
}

VariableDeclaration* create_variable_declaration(Array* declarations, VariableKind kind,
                                               int line, int column) {
    VariableDeclaration* node = malloc(sizeof(VariableDeclaration));
    node->base.type = NODE_VARIABLE_DECLARATION;
    node->base.line = line;
    node->base.column = column;
    node->base.parent = NULL;
    node->declarations = *declarations;
    node->kind = kind;
    return node;
}

Parameter* create_parameter(Identifier* name, const char* param_type, 
                          Expression* default_value, int line, int column) {
    Parameter* node = malloc(sizeof(Parameter));
    node->base.type = NODE_PARAMETER;
    node->base.line = line;
    node->base.column = column;
    node->base.parent = NULL;
    node->name = name;
    node->param_type = param_type ? strdup(param_type) : NULL;
    node->default_value = default_value;
    return node;
}

BlockStatement* create_block_statement(Array* body, int line, int column) {
    BlockStatement* node = malloc(sizeof(BlockStatement));
    node->base.type = NODE_BLOCK_STATEMENT;
    node->base.line = line;
    node->base.column = column;
    node->base.parent = NULL;
    node->body = *body;
    return node;
}

FunctionDeclaration* create_function_declaration(Identifier* id, Array* params,
                                               BlockStatement* body, const char* return_type,
                                               int line, int column) {
    FunctionDeclaration* node = malloc(sizeof(FunctionDeclaration));
    node->base.type = NODE_FUNCTION_DECLARATION;
    node->base.line = line;
    node->base.column = column;
    node->base.parent = NULL;
    node->id = id;
    node->params = *params;
    node->body = body;
    node->return_type = return_type ? strdup(return_type) : NULL;
    return node;
}

ReturnStatement* create_return_statement(Expression* argument, int line, int column) {
    ReturnStatement* node = malloc(sizeof(ReturnStatement));
    node->base.type = NODE_RETURN_STATEMENT;
    node->base.line = line;
    node->base.column = column;
    node->base.parent = NULL;
    node->argument = argument;
    return node;
}

Program* create_program(Array* body, SourceType source_type, int line, int column) {
    Program* node = malloc(sizeof(Program));
    node->base.type = NODE_PROGRAM;
    node->base.line = line;
    node->base.column = column;
    node->base.parent = NULL;
    node->body = *body;
    node->source_type = source_type;
    return node;
}

// Visitor function type
typedef void (*VisitorFunc)(ASTNode* node, ASTNode* parent, void* data);

// AST traversal function
void traverse_ast(ASTNode* node, VisitorFunc enter, VisitorFunc exit, void* data) {
    traverse_ast_with_parent(node, NULL, enter, exit, data);
}

void traverse_ast_with_parent(ASTNode* node, ASTNode* parent, 
                             VisitorFunc enter, VisitorFunc exit, void* data) {
    if (!node) return;
    
    if (enter) {
        enter(node, parent, data);
    }
    
    // Visit children based on node type
    switch (node->type) {
        case NODE_BINARY_EXPRESSION: {
            BinaryExpression* bin = (BinaryExpression*)node;
            traverse_ast_with_parent((ASTNode*)bin->left, node, enter, exit, data);
            traverse_ast_with_parent((ASTNode*)bin->right, node, enter, exit, data);
            break;
        }
        case NODE_UNARY_EXPRESSION: {
            UnaryExpression* unary = (UnaryExpression*)node;
            traverse_ast_with_parent((ASTNode*)unary->argument, node, enter, exit, data);
            break;
        }
        case NODE_CALL_EXPRESSION: {
            CallExpression* call = (CallExpression*)node;
            traverse_ast_with_parent((ASTNode*)call->callee, node, enter, exit, data);
            for (size_t i = 0; i < call->arguments.count; i++) {
                traverse_ast_with_parent((ASTNode*)call->arguments.items[i], node, enter, exit, data);
            }
            break;
        }
        case NODE_VARIABLE_DECLARATION: {
            VariableDeclaration* var_decl = (VariableDeclaration*)node;
            for (size_t i = 0; i < var_decl->declarations.count; i++) {
                traverse_ast_with_parent((ASTNode*)var_decl->declarations.items[i], node, enter, exit, data);
            }
            break;
        }
        case NODE_VARIABLE_DECLARATOR: {
            VariableDeclarator* var_declarator = (VariableDeclarator*)node;
            traverse_ast_with_parent((ASTNode*)var_declarator->id, node, enter, exit, data);
            if (var_declarator->init) {
                traverse_ast_with_parent((ASTNode*)var_declarator->init, node, enter, exit, data);
            }
            break;
        }
        case NODE_FUNCTION_DECLARATION: {
            FunctionDeclaration* func = (FunctionDeclaration*)node;
            traverse_ast_with_parent((ASTNode*)func->id, node, enter, exit, data);
            for (size_t i = 0; i < func->params.count; i++) {
                traverse_ast_with_parent((ASTNode*)func->params.items[i], node, enter, exit, data);
            }
            traverse_ast_with_parent((ASTNode*)func->body, node, enter, exit, data);
            break;
        }
        case NODE_PARAMETER: {
            Parameter* param = (Parameter*)node;
            traverse_ast_with_parent((ASTNode*)param->name, node, enter, exit, data);
            if (param->default_value) {
                traverse_ast_with_parent((ASTNode*)param->default_value, node, enter, exit, data);
            }
            break;
        }
        case NODE_BLOCK_STATEMENT: {
            BlockStatement* block = (BlockStatement*)node;
            for (size_t i = 0; i < block->body.count; i++) {
                traverse_ast_with_parent((ASTNode*)block->body.items[i], node, enter, exit, data);
            }
            break;
        }
        case NODE_RETURN_STATEMENT: {
            ReturnStatement* ret = (ReturnStatement*)node;
            if (ret->argument) {
                traverse_ast_with_parent((ASTNode*)ret->argument, node, enter, exit, data);
            }
            break;
        }
        case NODE_PROGRAM: {
            Program* program = (Program*)node;
            for (size_t i = 0; i < program->body.count; i++) {
                traverse_ast_with_parent((ASTNode*)program->body.items[i], node, enter, exit, data);
            }
            break;
        }
        case NODE_IF_STATEMENT: {
            IfStatement* if_stmt = (IfStatement*)node;
            traverse_ast_with_parent((ASTNode*)if_stmt->test, node, enter, exit, data);
            traverse_ast_with_parent((ASTNode*)if_stmt->consequent, node, enter, exit, data);
            if (if_stmt->alternate) {
                traverse_ast_with_parent((ASTNode*)if_stmt->alternate, node, enter, exit, data);
            }
            break;
        }
        case NODE_WHILE_STATEMENT: {
            WhileStatement* while_stmt = (WhileStatement*)node;
            traverse_ast_with_parent((ASTNode*)while_stmt->test, node, enter, exit, data);
            traverse_ast_with_parent((ASTNode*)while_stmt->body, node, enter, exit, data);
            break;
        }
        case NODE_ARRAY_EXPRESSION: {
            ArrayExpression* arr = (ArrayExpression*)node;
            for (size_t i = 0; i < arr->elements.count; i++) {
                traverse_ast_with_parent((ASTNode*)arr->elements.items[i], node, enter, exit, data);
            }
            break;
        }
        case NODE_OBJECT_EXPRESSION: {
            ObjectExpression* obj = (ObjectExpression*)node;
            for (size_t i = 0; i < obj->properties.count; i++) {
                traverse_ast_with_parent((ASTNode*)obj->properties.items[i], node, enter, exit, data);
            }
            break;
        }
        case NODE_PROPERTY: {
            Property* prop = (Property*)node;
            traverse_ast_with_parent((ASTNode*)prop->key, node, enter, exit, data);
            traverse_ast_with_parent((ASTNode*)prop->value, node, enter, exit, data);
            break;
        }
        // Add other cases as needed
        default:
            break;
    }
    
    if (exit) {
        exit(node, parent, data);
    }
}

// Utility functions for finding nodes
typedef struct {
    NodeType target_type;
    Array* results;
} FindNodesData;

void find_nodes_visitor(ASTNode* node, ASTNode* parent, void* data) {
    FindNodesData* find_data = (FindNodesData*)data;
    if (node->type == find_data->target_type) {
        array_push(find_data->results, node);
    }
}

Array* find_nodes_by_type(ASTNode* root, NodeType type) {
    Array* results = array_create(10);
    FindNodesData data = { type, results };
    traverse_ast(root, find_nodes_visitor, NULL, &data);
    return results;
}

// Pretty print function
const char* node_type_to_string(NodeType type) {
    switch (type) {
        case NODE_PROGRAM: return "Program";
        case NODE_IDENTIFIER: return "Identifier";
        case NODE_LITERAL: return "Literal";
        case NODE_BINARY_EXPRESSION: return "BinaryExpression";
        case NODE_UNARY_EXPRESSION: return "UnaryExpression";
        case NODE_ASSIGNMENT_EXPRESSION: return "AssignmentExpression";
        case NODE_CALL_EXPRESSION: return "CallExpression";
        case NODE_MEMBER_EXPRESSION: return "MemberExpression";
        case NODE_ARRAY_EXPRESSION: return "ArrayExpression";
        case NODE_OBJECT_EXPRESSION: return "ObjectExpression";
        case NODE_PROPERTY: return "Property";
        case NODE_CONDITIONAL_EXPRESSION: return "ConditionalExpression";
        case NODE_EXPRESSION_STATEMENT: return "ExpressionStatement";
        case NODE_VARIABLE_DECLARATION: return "VariableDeclaration";
        case NODE_VARIABLE_DECLARATOR: return "VariableDeclarator";
        case NODE_FUNCTION_DECLARATION: return "FunctionDeclaration";
        case NODE_PARAMETER: return "Parameter";
        case NODE_BLOCK_STATEMENT: return "BlockStatement";
        case NODE_RETURN_STATEMENT: return "ReturnStatement";
        case NODE_IF_STATEMENT: return "IfStatement";
        case NODE_WHILE_STATEMENT: return "WhileStatement";
        case NODE_FOR_STATEMENT: return "ForStatement";
        case NODE_BREAK_STATEMENT: return "BreakStatement";
        case NODE_CONTINUE_STATEMENT: return "ContinueStatement";
        case NODE_THROW_STATEMENT: return "ThrowStatement";
        case NODE_TRY_STATEMENT: return "TryStatement";
        case NODE_CATCH_CLAUSE: return "CatchClause";
        case NODE_SWITCH_STATEMENT: return "SwitchStatement";
        case NODE_SWITCH_CASE: return "SwitchCase";
        default: return "Unknown";
    }
}

void pretty_print_ast(ASTNode* node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
    
    printf("%s", node_type_to_string(node->type));
    
    // Print node-specific information
    switch (node->type) {
        case NODE_IDENTIFIER: {
            Identifier* id = (Identifier*)node;
            printf(" \"%s\"", id->name);
            break;
        }
        case NODE_LITERAL: {
            Literal* lit = (Literal*)node;
            switch (lit->literal_type) {
                case LITERAL_STRING:
                    printf(" = \"%s\"", lit->value.string_value);
                    break;
                case LITERAL_NUMBER:
                    printf(" = %g", lit->value.number_value);
                    break;
                case LITERAL_BOOLEAN:
                    printf(" = %s", lit->value.boolean_value ? "true" : "false");
                    break;
                case LITERAL_NULL:
                    printf(" = null");
                    break;
            }
            break;
        }
        case NODE_BINARY_EXPRESSION: {
            BinaryExpression* bin = (BinaryExpression*)node;
            printf(" (%s)", bin->operator);
            break;
        }
        case NODE_UNARY_EXPRESSION: {
            UnaryExpression* unary = (UnaryExpression*)node;
            printf(" (%s)", unary->operator);
            break;
        }
    }
    
    printf("\n");
    
    // Print children
    switch (node->type) {
        case NODE_BINARY_EXPRESSION: {
            BinaryExpression* bin = (BinaryExpression*)node;
            pretty_print_ast((ASTNode*)bin->left, indent + 1);
            pretty_print_ast((ASTNode*)bin->right, indent + 1);
            break;
        }
        case NODE_UNARY_EXPRESSION: {
            UnaryExpression* unary = (UnaryExpression*)node;
            pretty_print_ast((ASTNode*)unary->argument, indent + 1);
            break;
        }
        case NODE_CALL_EXPRESSION: {
            CallExpression* call = (CallExpression*)node;
            pretty_print_ast((ASTNode*)call->callee, indent + 1);
            for (size_t i = 0; i < call->arguments.count; i++) {
                pretty_print_ast((ASTNode*)call->arguments.items[i], indent + 1);
            }
            break;
        }
        case NODE_VARIABLE_DECLARATION: {
            VariableDeclaration* var_decl = (VariableDeclaration*)node;
            for (size_t i = 0; i < var_decl->declarations.count; i++) {
                pretty_print_ast((ASTNode*)var_decl->declarations.items[i], indent + 1);
            }
            break;
        }
        case NODE_VARIABLE_DECLARATOR: {
            VariableDeclarator* var_declarator = (VariableDeclarator*)node;
            pretty_print_ast((ASTNode*)var_declarator->id, indent + 1);
            if (var_declarator->init) {
                pretty_print_ast((ASTNode*)var_declarator->init, indent + 1);
            }
            break;
        }
        case NODE_FUNCTION_DECLARATION: {
            FunctionDeclaration* func = (FunctionDeclaration*)node;
            pretty_print_ast((ASTNode*)func->id, indent + 1);
            for (size_t i = 0; i < func->params.count; i++) {
                pretty_print_ast((ASTNode*)func->params.items[i], indent + 1);
            }
            pretty_print_ast((ASTNode*)func->body, indent + 1);
            break;
        }
        case NODE_PARAMETER: {
            Parameter* param = (Parameter*)node;
            pretty_print_ast((ASTNode*)param->name, indent + 1);
            if (param->default_value) {
                pretty_print_ast((ASTNode*)param->default_value, indent + 1);
            }
            break;
        }
        case NODE_BLOCK_STATEMENT: {
            BlockStatement* block = (BlockStatement*)node;
            for (size_t i = 0; i < block->body.count; i++) {
                pretty_print_ast((ASTNode*)block->body.items[i], indent + 1);
            }
            break;
        }
        case NODE_RETURN_STATEMENT: {
            ReturnStatement* ret = (ReturnStatement*)node;
            if (ret->argument) {
                pretty_print_ast((ASTNode*)ret->argument, indent + 1);
            }
            break;
        }
        case NODE_PROGRAM: {
            Program* program = (Program*)node;
            for (size_t i = 0; i < program->body.count; i++) {
                pretty_print_ast((ASTNode*)program->body.items[i], indent + 1);
            }
            break;
        }
        // Add other cases as needed
        default:
            break;
    }
}

// Memory cleanup functions
void free_ast_node(ASTNode* node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_IDENTIFIER: {
            Identifier* id = (Identifier*)node;
            free(id->name);
            break;
        }
        case NODE_LITERAL: {
            Literal* lit = (Literal*)node;
            if (lit->literal_type == LITERAL_STRING) {
                free(lit->value.string_value);
            }
            if (lit->raw) {
                free(lit->raw);
            }
            break;
        }
        case NODE_BINARY_EXPRESSION: {
            BinaryExpression* bin = (BinaryExpression*)node;
            free(bin->operator);
            free_ast_node((ASTNode*)bin->left);
            free_ast_node((ASTNode*)bin->right);
            break;
        }
        case NODE_UNARY_EXPRESSION: {
            UnaryExpression* unary = (UnaryExpression*)node;
            free(unary->operator);
            free_ast_node((ASTNode*)unary->argument);
            break;
        }
        case NODE_VARIABLE_DECLARATION: {
            VariableDeclaration* var_decl = (VariableDeclaration*)node;
            for (size_t i = 0; i < var_decl->declarations.count; i++) {
                free_ast_node((ASTNode*)var_decl->declarations.items[i]);
            }
            array_free(&var_decl->declarations);
            break;
        }
        case NODE_FUNCTION_DECLARATION: {
            FunctionDeclaration* func = (FunctionDeclaration*)node;
            free_ast_node((ASTNode*)func->id);
            for (size_t i = 0; i < func->params.count; i++) {
                free_ast_node((ASTNode*)func->params.items[i]);
            }
            array_free(&func->params);
            free_ast_node((ASTNode*)func->body);
            if (func->return_type) {
                free(func->return_type);
            }
            break;
        }
        // Add cleanup for other node types
        default:
            break;
    }
    
    free(node);
}

// Example usage function
void demonstrate_ast() {
    printf("=== Enhanced AST Demo ===\n\n");
    
    // Create identifiers
    Identifier* x_id = create_identifier("x", 1, 1);
    Identifier* a_id = create_identifier("a", 3, 15);
    Identifier* b_id = create_identifier("b", 3, 25);
    Identifier* add_id = create_identifier("add", 3, 10);
    
    // Create literals
    Literal* num_42 = create_literal_number(42.0, "42", 1, 7);
    
    // Create variable declarator and declaration
    VariableDeclarator* x_declarator = create_variable_declarator(x_id, (Expression*)num_42, 1, 5);
    Array* var_declarations = array_create(1);
    array_push(var_declarations, x_declarator);
    VariableDeclaration* var_decl = create_variable_declaration(var_declarations, VAR_KIND_LET, 1, 1);
    
    // Create function parameters
    Parameter* param_a = create_parameter(a_id, "number", NULL, 3, 15);
    Parameter* param_b = create_parameter(b_id, "number", NULL, 3, 25);
    Array* func_params = array_create(2);
    array_push(func_params, param_a);
    array_push(func_params, param_b);
    
    // Create binary expression for a + b
    Identifier* a_ref = create_identifier("a", 4, 12);
    Identifier* b_ref = create_identifier("b", 4, 16);
    BinaryExpression* add_expr = create_binary_expression("+", (Expression*)a_ref, (Expression*)b_ref, 4, 14);
    
    // Create return statement
    ReturnStatement* return_stmt = create_return_statement((Expression*)add_expr, 4, 5);
    
    // Create block statement for function body
    Array* func_body = array_create(1);
    array_push(func_body, return_stmt);
    BlockStatement* block = create_block_statement(func_body, 3, 35);
    
    // Create function declaration
    FunctionDeclaration* func_decl = create_function_declaration(add_id, func_params, block, "number", 3, 1);
    
    // Create program
    Array* program_body = array_create(2);
    array_push(program_body, var_decl);
    array_push(program_body, func_decl);
    Program* program = create_program(program_body, SOURCE_SCRIPT, 0, 0);
    
    // Demonstrate functionality
    printf("1. AST Structure:\n");
    pretty_print_ast((ASTNode*)program, 0);
    printf("\n");
    
    printf("2. All Identifiers:\n");
    Array* identifiers = find_nodes_by_type((ASTNode*)program, NODE_IDENTIFIER);
    for (size_t i = 0; i < identifiers->count; i++) {
        Identifier* id = (Identifier*)identifiers->items[i];
        printf("   %s\n", id->name);
    }
    printf("\n");
    
    printf("3. Function Declarations:\n");
    Array* functions = find_nodes_by_type((ASTNode*)program, NODE_FUNCTION_DECLARATION);
    for (size_t i = 0; i < functions->count; i++) {
        FunctionDeclaration* func = (FunctionDeclaration*)functions->items[i];
        printf("   %s\n", func->id->name);
    }
    printf("\n");
    
    printf("4. Variable Declarations:\n");
    Array* variables = find_nodes_by_type((ASTNode*)program, NODE_VARIABLE_DECLARATION);
    for (size_t i = 0; i < variables->count; i++) {
        VariableDeclaration* var = (VariableDeclaration*)variables->items[i];
        for (size_t j = 0; j < var->declarations.count; j++) {
            VariableDeclarator* declarator = (VariableDeclarator*)var->declarations.items[j];
            printf("   %s\n", declarator->id->name);
        }
    }
    printf("\n");
    
    // Cleanup
    array_free(identifiers);
    array_free(functions);
    array_free(variables);
    free_ast_node((ASTNode*)program);
}

// JSON serialization functions
void print_json_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
}

void ast_to_json(ASTNode* node, int indent) {
    if (!node) {
        printf("null");
        return;
    }
    
    printf("{\n");
    print_json_indent(indent + 1);
    printf("\"type\": \"%s\"", node_type_to_string(node->type));
    
    if (node->line > 0) {
        printf(",\n");
        print_json_indent(indent + 1);
        printf("\"line\": %d", node->line);
    }
    
    if (node->column > 0) {
        printf(",\n");
        print_json_indent(indent + 1);
        printf("\"column\": %d", node->column);
    }
    
    // Add node-specific fields
    switch (node->type) {
        case NODE_IDENTIFIER: {
            Identifier* id = (Identifier*)node;
            printf(",\n");
            print_json_indent(indent + 1);
            printf("\"name\": \"%s\"", id->name);
            break;
        }
        case NODE_LITERAL: {
            Literal* lit = (Literal*)node;
            printf(",\n");
            print_json_indent(indent + 1);
            printf("\"value\": ");
            switch (lit->literal_type) {
                case LITERAL_STRING:
                    printf("\"%s\"", lit->value.string_value);
                    break;
                case LITERAL_NUMBER:
                    printf("%g", lit->value.number_value);
                    break;
                case LITERAL_BOOLEAN:
                    printf("%s", lit->value.boolean_value ? "true" : "false");
                    break;
                case LITERAL_NULL:
                    printf("null");
                    break;
            }
            if (lit->raw) {
                printf(",\n");
                print_json_indent(indent + 1);
                printf("\"raw\": \"%s\"", lit->raw);
            }
            break;
        }
        case NODE_BINARY_EXPRESSION: {
            BinaryExpression* bin = (BinaryExpression*)node;
            printf(",\n");
            print_json_indent(indent + 1);
            printf("\"operator\": \"%s\",\n", bin->operator);
            print_json_indent(indent + 1);
            printf("\"left\": ");
            ast_to_json((ASTNode*)bin->left, indent + 1);
            printf(",\n");
            print_json_indent(indent + 1);
            printf("\"right\": ");
            ast_to_json((ASTNode*)bin->right, indent + 1);
            break;
        }
        case NODE_UNARY_EXPRESSION: {
            UnaryExpression* unary = (UnaryExpression*)node;
            printf(",\n");
            print_json_indent(indent + 1);
            printf("\"operator\": \"%s\",\n", unary->operator);
            print_json_indent(indent + 1);
            printf("\"argument\": ");
            ast_to_json((ASTNode*)unary->argument, indent + 1);
            break;
        }
        case NODE_VARIABLE_DECLARATION: {
            VariableDeclaration* var_decl = (VariableDeclaration*)node;
            printf(",\n");
            print_json_indent(indent + 1);
            printf("\"kind\": \"%s\",\n", 
                   var_decl->kind == VAR_KIND_VAR ? "var" :
                   var_decl->kind == VAR_KIND_LET ? "let" : "const");
            print_json_indent(indent + 1);
            printf("\"declarations\": [\n");
            for (size_t i = 0; i < var_decl->declarations.count; i++) {
                if (i > 0) printf(",\n");
                print_json_indent(indent + 2);
                ast_to_json((ASTNode*)var_decl->declarations.items[i], indent + 2);
            }
            printf("\n");
            print_json_indent(indent + 1);
            printf("]");
            break;
        }
        case NODE_VARIABLE_DECLARATOR: {
            VariableDeclarator* var_declarator = (VariableDeclarator*)node;
            printf(",\n");
            print_json_indent(indent + 1);
            printf("\"id\": ");
            ast_to_json((ASTNode*)var_declarator->id, indent + 1);
            if (var_declarator->init) {
                printf(",\n");
                print_json_indent(indent + 1);
                printf("\"init\": ");
                ast_to_json((ASTNode*)var_declarator->init, indent + 1);
            }
            break;
        }
        case NODE_FUNCTION_DECLARATION: {
            FunctionDeclaration* func = (FunctionDeclaration*)node;
            printf(",\n");
            print_json_indent(indent + 1);
            printf("\"id\": ");
            ast_to_json((ASTNode*)func->id, indent + 1);
            printf(",\n");
            print_json_indent(indent + 1);
            printf("\"params\": [\n");
            for (size_t i = 0; i < func->params.count; i++) {
                if (i > 0) printf(",\n");
                print_json_indent(indent + 2);
                ast_to_json((ASTNode*)func->params.items[i], indent + 2);
            }
            printf("\n");
            print_json_indent(indent + 1);
            printf("],\n");
            print_json_indent(indent + 1);
            printf("\"body\": ");
            ast_to_json((ASTNode*)func->body, indent + 1);
            if (func->return_type) {
                printf(",\n");
                print_json_indent(indent + 1);
                printf("\"returnType\": \"%s\"", func->return_type);
            }
            break;
        }
        case NODE_PARAMETER: {
            Parameter* param = (Parameter*)node;
            printf(",\n");
            print_json_indent(indent + 1);
            printf("\"name\": ");
            ast_to_json((ASTNode*)param->name, indent + 1);
            if (param->param_type) {
                printf(",\n");
                print_json_indent(indent + 1);
                printf("\"paramType\": \"%s\"", param->param_type);
            }
            if (param->default_value) {
                printf(",\n");
                print_json_indent(indent + 1);
                printf("\"defaultValue\": ");
                ast_to_json((ASTNode*)param->default_value, indent + 1);
            }
            break;
        }
        case NODE_BLOCK_STATEMENT: {
            BlockStatement* block = (BlockStatement*)node;
            printf(",\n");
            print_json_indent(indent + 1);
            printf("\"body\": [\n");
            for (size_t i = 0; i < block->body.count; i++) {
                if (i > 0) printf(",\n");
                print_json_indent(indent + 2);
                ast_to_json((ASTNode*)block->body.items[i], indent + 2);
            }
            printf("\n");
            print_json_indent(indent + 1);
            printf("]");
            break;
        }
        case NODE_RETURN_STATEMENT: {
            ReturnStatement* ret = (ReturnStatement*)node;
            if (ret->argument) {
                printf(",\n");
                print_json_indent(indent + 1);
                printf("\"argument\": ");
                ast_to_json((ASTNode*)ret->argument, indent + 1);
            }
            break;
        }
        case NODE_PROGRAM: {
            Program* program = (Program*)node;
            printf(",\n");
            print_json_indent(indent + 1);
            printf("\"sourceType\": \"%s\",\n", 
                   program->source_type == SOURCE_SCRIPT ? "script" : "module");
            print_json_indent(indent + 1);
            printf("\"body\": [\n");
            for (size_t i = 0; i < program->body.count; i++) {
                if (i > 0) printf(",\n");
                print_json_indent(indent + 2);
                ast_to_json((ASTNode*)program->body.items[i], indent + 2);
            }
            printf("\n");
            print_json_indent(indent + 1);
            printf("]");
            break;
        }
        default:
            break;
    }
    
    printf("\n");
    print_json_indent(indent);
    printf("}");
}

// AST cloning function
ASTNode* clone_ast_node(ASTNode* node) {
    if (!node) return NULL;
    
    switch (node->type) {
        case NODE_IDENTIFIER: {
            Identifier* orig = (Identifier*)node;
            return (ASTNode*)create_identifier(orig->name, orig->base.line, orig->base.column);
        }
        case NODE_LITERAL: {
            Literal* orig = (Literal*)node;
            switch (orig->literal_type) {
                case LITERAL_STRING:
                    return (ASTNode*)create_literal_string(orig->value.string_value, orig->raw,
                                                          orig->base.line, orig->base.column);
                case LITERAL_NUMBER:
                    return (ASTNode*)create_literal_number(orig->value.number_value, orig->raw,
                                                          orig->base.line, orig->base.column);
                case LITERAL_BOOLEAN:
                    return (ASTNode*)create_literal_boolean(orig->value.boolean_value, orig->raw,
                                                           orig->base.line, orig->base.column);
                case LITERAL_NULL:
                    return (ASTNode*)create_literal_null(orig->raw, orig->base.line, orig->base.column);
            }
            break;
        }
        case NODE_BINARY_EXPRESSION: {
            BinaryExpression* orig = (BinaryExpression*)node;
            Expression* left = (Expression*)clone_ast_node((ASTNode*)orig->left);
            Expression* right = (Expression*)clone_ast_node((ASTNode*)orig->right);
            return (ASTNode*)create_binary_expression(orig->operator, left, right,
                                                     orig->base.line, orig->base.column);
        }
        // Add other cloning cases as needed
        default:
            return NULL;
    }
    
    return NULL;
}

// Main function
int main() {
    demonstrate_ast();
    return 0;
}



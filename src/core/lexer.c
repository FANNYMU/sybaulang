#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Token type enumeration
typedef enum {
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_OPERATOR,
    TOKEN_DELIMITER,
    TOKEN_COMMENT,
    TOKEN_WHITESPACE,
    TOKEN_EOF
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char* value;
    int line;
    int column;
} Token;

// Tokenizer options structure
typedef struct {
    bool include_whitespace;
    bool include_comments;
    char** keywords;
    int keywords_count;
    char** operators;
    int operators_count;
    char** delimiters;
    int delimiters_count;
    bool skip_unknown;
    bool case_sensitive;
} TokenizerOptions;

// Tokenizer structure
typedef struct {
    TokenizerOptions options;
    char** keywords;
    int keywords_count;
    char** operators;
    int operators_count;
    char** delimiters;
    int delimiters_count;
} Tokenizer;

// Token array structure for returning multiple tokens
typedef struct {
    Token* tokens;
    int count;
    int capacity;
} TokenArray;

// Utility functions
char* string_duplicate(const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str);
    char* copy = malloc(len + 1);
    if (copy) {
        strcpy(copy, str);
    }
    return copy;
}

char* string_to_lower(const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str);
    char* lower = malloc(len + 1);
    if (lower) {
        for (size_t i = 0; i <= len; i++) {
            lower[i] = tolower(str[i]);
        }
    }
    return lower;
}

bool is_keyword(Tokenizer* tokenizer, const char* word) {
    for (int i = 0; i < tokenizer->keywords_count; i++) {
        if (tokenizer->options.case_sensitive) {
            if (strcmp(tokenizer->keywords[i], word) == 0) {
                return true;
            }
        } else {
            char* lower_word = string_to_lower(word);
            char* lower_keyword = string_to_lower(tokenizer->keywords[i]);
            bool match = (strcmp(lower_keyword, lower_word) == 0);
            free(lower_word);
            free(lower_keyword);
            if (match) return true;
        }
    }
    return false;
}

// Token creation
Token create_token(TokenType type, const char* value, int line, int column) {
    Token token;
    token.type = type;
    token.value = string_duplicate(value);
    token.line = line;
    token.column = column;
    return token;
}

void free_token(Token* token) {
    if (token && token->value) {
        free(token->value);
        token->value = NULL;
    }
}

// TokenArray functions
TokenArray* create_token_array() {
    TokenArray* arr = malloc(sizeof(TokenArray));
    if (arr) {
        arr->tokens = malloc(sizeof(Token) * 100);
        arr->count = 0;
        arr->capacity = 100;
    }
    return arr;
}

void add_token(TokenArray* arr, Token token) {
    if (!arr) return;
    
    if (arr->count >= arr->capacity) {
        arr->capacity *= 2;
        arr->tokens = realloc(arr->tokens, sizeof(Token) * arr->capacity);
    }
    
    arr->tokens[arr->count++] = token;
}

void free_token_array(TokenArray* arr) {
    if (!arr) return;
    
    for (int i = 0; i < arr->count; i++) {
        free_token(&arr->tokens[i]);
    }
    free(arr->tokens);
    free(arr);
}

// Default arrays
static const char* default_keywords[] = {
    "fn", "int", "float", "bool", "char", "string", "if", "else", "while",
    "for", "return", "print", "true", "false", "null", "undefined",
    "let", "const", "var"
};

static const char* default_operators[] = {
    "==", "!=", "<=", ">=", "&&", "||", "++", "--", "+=", "-=", "*=", "/=",
    "+", "-", "*", "/", "%", "=", "<", ">", "!", "&", "|", "^", "~", "?", ":"
};

static const char* default_delimiters[] = {
    "(", ")", "{", "}", "[", "]", ",", ";", ".", "->"
};

// Comparison function for sorting operators by length (descending)
int compare_operators(const void* a, const void* b) {
    const char* str_a = *(const char**)a;
    const char* str_b = *(const char**)b;
    return strlen(str_b) - strlen(str_a);
}

// Initialize tokenizer
Tokenizer* create_tokenizer(TokenizerOptions* options) {
    Tokenizer* tokenizer = malloc(sizeof(Tokenizer));
    if (!tokenizer) return NULL;
    
    // Set default options
    if (options) {
        tokenizer->options = *options;
    } else {
        tokenizer->options.include_whitespace = false;
        tokenizer->options.include_comments = false;
        tokenizer->options.skip_unknown = true;
        tokenizer->options.case_sensitive = true;
        tokenizer->options.keywords = NULL;
        tokenizer->options.operators = NULL;
        tokenizer->options.delimiters = NULL;
    }
    
    // Setup keywords
    if (tokenizer->options.keywords && tokenizer->options.keywords_count > 0) {
        tokenizer->keywords_count = tokenizer->options.keywords_count;
        tokenizer->keywords = malloc(sizeof(char*) * tokenizer->keywords_count);
        for (int i = 0; i < tokenizer->keywords_count; i++) {
            tokenizer->keywords[i] = string_duplicate(tokenizer->options.keywords[i]);
        }
    } else {
        tokenizer->keywords_count = sizeof(default_keywords) / sizeof(default_keywords[0]);
        tokenizer->keywords = malloc(sizeof(char*) * tokenizer->keywords_count);
        for (int i = 0; i < tokenizer->keywords_count; i++) {
            tokenizer->keywords[i] = string_duplicate(default_keywords[i]);
        }
    }
    
    // Setup operators
    if (tokenizer->options.operators && tokenizer->options.operators_count > 0) {
        tokenizer->operators_count = tokenizer->options.operators_count;
        tokenizer->operators = malloc(sizeof(char*) * tokenizer->operators_count);
        for (int i = 0; i < tokenizer->operators_count; i++) {
            tokenizer->operators[i] = string_duplicate(tokenizer->options.operators[i]);
        }
    } else {
        tokenizer->operators_count = sizeof(default_operators) / sizeof(default_operators[0]);
        tokenizer->operators = malloc(sizeof(char*) * tokenizer->operators_count);
        for (int i = 0; i < tokenizer->operators_count; i++) {
            tokenizer->operators[i] = string_duplicate(default_operators[i]);
        }
    }
    
    // Sort operators by length (longest first)
    qsort(tokenizer->operators, tokenizer->operators_count, sizeof(char*), compare_operators);
    
    // Setup delimiters
    if (tokenizer->options.delimiters && tokenizer->options.delimiters_count > 0) {
        tokenizer->delimiters_count = tokenizer->options.delimiters_count;
        tokenizer->delimiters = malloc(sizeof(char*) * tokenizer->delimiters_count);
        for (int i = 0; i < tokenizer->delimiters_count; i++) {
            tokenizer->delimiters[i] = string_duplicate(tokenizer->options.delimiters[i]);
        }
    } else {
        tokenizer->delimiters_count = sizeof(default_delimiters) / sizeof(default_delimiters[0]);
        tokenizer->delimiters = malloc(sizeof(char*) * tokenizer->delimiters_count);
        for (int i = 0; i < tokenizer->delimiters_count; i++) {
            tokenizer->delimiters[i] = string_duplicate(default_delimiters[i]);
        }
    }
    
    return tokenizer;
}

void free_tokenizer(Tokenizer* tokenizer) {
    if (!tokenizer) return;
    
    for (int i = 0; i < tokenizer->keywords_count; i++) {
        free(tokenizer->keywords[i]);
    }
    free(tokenizer->keywords);
    
    for (int i = 0; i < tokenizer->operators_count; i++) {
        free(tokenizer->operators[i]);
    }
    free(tokenizer->operators);
    
    for (int i = 0; i < tokenizer->delimiters_count; i++) {
        free(tokenizer->delimiters[i]);
    }
    free(tokenizer->delimiters);
    
    free(tokenizer);
}

bool is_delimiter(Tokenizer* tokenizer, char ch) {
    char str[2] = {ch, '\0'};
    for (int i = 0; i < tokenizer->delimiters_count; i++) {
        if (strcmp(tokenizer->delimiters[i], str) == 0) {
            return true;
        }
    }
    return false;
}

// Main tokenization function
TokenArray* tokenize(Tokenizer* tokenizer, const char* input) {
    if (!tokenizer || !input) return NULL;
    
    TokenArray* tokens = create_token_array();
    if (!tokens) return NULL;
    
    int len = strlen(input);
    int i = 0;
    int line = 1;
    int column = 1;
    
    while (i < len) {
        char ch = input[i];
        int start_line = line;
        int start_column = column;
        
        // Track line and column
        if (ch == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        
        // Whitespace
        if (isspace(ch)) {
            if (tokenizer->options.include_whitespace) {
                char whitespace[1000] = {0};
                int ws_idx = 0;
                while (i < len && isspace(input[i]) && ws_idx < 999) {
                    whitespace[ws_idx++] = input[i];
                    if (input[i] == '\n') {
                        line++;
                        column = 1;
                    } else {
                        column++;
                    }
                    i++;
                }
                whitespace[ws_idx] = '\0';
                Token token = create_token(TOKEN_WHITESPACE, whitespace, start_line, start_column);
                add_token(tokens, token);
            } else {
                i++;
            }
            continue;
        }
        
        // Single line comments
        if (ch == '/' && i + 1 < len && input[i + 1] == '/') {
            char comment[1000] = {0};
            int comment_idx = 0;
            while (i < len && input[i] != '\n' && comment_idx < 999) {
                comment[comment_idx++] = input[i++];
                column++;
            }
            comment[comment_idx] = '\0';
            if (tokenizer->options.include_comments) {
                Token token = create_token(TOKEN_COMMENT, comment, start_line, start_column);
                add_token(tokens, token);
            }
            continue;
        }
        
        // Multi-line comments
        if (ch == '/' && i + 1 < len && input[i + 1] == '*') {
            char comment[2000] = {'/', '*'};
            int comment_idx = 2;
            i += 2;
            column += 2;
            
            while (i < len - 1 && comment_idx < 1998) {
                if (input[i] == '*' && input[i + 1] == '/') {
                    comment[comment_idx++] = '*';
                    comment[comment_idx++] = '/';
                    i += 2;
                    column += 2;
                    break;
                }
                comment[comment_idx++] = input[i];
                if (input[i] == '\n') {
                    line++;
                    column = 1;
                } else {
                    column++;
                }
                i++;
            }
            comment[comment_idx] = '\0';
            
            if (tokenizer->options.include_comments) {
                Token token = create_token(TOKEN_COMMENT, comment, start_line, start_column);
                add_token(tokens, token);
            }
            continue;
        }
        
        // Numbers
        if (isdigit(ch)) {
            char num[100] = {0};
            int num_idx = 0;
            while (i < len && (isdigit(input[i]) || input[i] == '.') && num_idx < 99) {
                num[num_idx++] = input[i++];
                column++;
            }
            num[num_idx] = '\0';
            Token token = create_token(TOKEN_NUMBER, num, start_line, start_column);
            add_token(tokens, token);
            continue;
        }
        
        // Identifiers and keywords
        if (isalpha(ch) || ch == '_') {
            char ident[256] = {0};
            int ident_idx = 0;
            while (i < len && (isalnum(input[i]) || input[i] == '_') && ident_idx < 255) {
                ident[ident_idx++] = input[i++];
                column++;
            }
            ident[ident_idx] = '\0';
            
            TokenType type = is_keyword(tokenizer, ident) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
            Token token = create_token(type, ident, start_line, start_column);
            add_token(tokens, token);
            continue;
        }
        
        // Strings
        if (ch == '"' || ch == '\'') {
            char quote = ch;
            i++; // skip opening quote
            column++;
            
            char str[1000] = {0};
            int str_idx = 0;
            
            while (i < len && input[i] != quote && str_idx < 999) {
                if (input[i] == '\\' && i + 1 < len) {
                    str[str_idx++] = input[i++]; // backslash
                    if (str_idx < 999) {
                        str[str_idx++] = input[i++]; // escaped character
                    }
                    column += 2;
                } else {
                    str[str_idx++] = input[i++];
                    column++;
                }
            }
            
            if (i < len) {
                i++; // skip closing quote
                column++;
            }
            str[str_idx] = '\0';
            Token token = create_token(TOKEN_STRING, str, start_line, start_column);
            add_token(tokens, token);
            continue;
        }
        
        // Operators (check longer ones first)
        bool found = false;
        for (int op_idx = 0; op_idx < tokenizer->operators_count; op_idx++) {
            char* op = tokenizer->operators[op_idx];
            int op_len = strlen(op);
            
            if (i + op_len <= len && strncmp(input + i, op, op_len) == 0) {
                Token token = create_token(TOKEN_OPERATOR, op, start_line, start_column);
                add_token(tokens, token);
                i += op_len;
                column += op_len;
                found = true;
                break;
            }
        }
        if (found) continue;
        
        // Delimiters
        if (is_delimiter(tokenizer, ch)) {
            char delim[2] = {ch, '\0'};
            Token token = create_token(TOKEN_DELIMITER, delim, start_line, start_column);
            add_token(tokens, token);
            i++;
            continue;
        }
        
        // Unknown character
        if (!tokenizer->options.skip_unknown) {
            char unknown[2] = {ch, '\0'};
            Token token = create_token(TOKEN_IDENTIFIER, unknown, start_line, start_column);
            add_token(tokens, token);
        }
        i++;
    }
    
    // Add EOF token
    Token eof_token = create_token(TOKEN_EOF, "", line, column);
    add_token(tokens, eof_token);
    
    return tokens;
}

// Utility functions
const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_KEYWORD: return "keyword";
        case TOKEN_IDENTIFIER: return "identifier";
        case TOKEN_NUMBER: return "number";
        case TOKEN_STRING: return "string";
        case TOKEN_OPERATOR: return "operator";
        case TOKEN_DELIMITER: return "delimiter";
        case TOKEN_COMMENT: return "comment";
        case TOKEN_WHITESPACE: return "whitespace";
        case TOKEN_EOF: return "eof";
        default: return "unknown";
    }
}

// Pretty print tokens
void pretty_print_tokens(TokenArray* tokens) {
    if (!tokens) return;
    
    for (int i = 0; i < tokens->count; i++) {
        Token* token = &tokens->tokens[i];
        if (token->type == TOKEN_EOF) continue;
        
        printf("%-12s [%d:%d]   \"%s\"\n", 
               token_type_to_string(token->type),
               token->line,
               token->column,
               token->value);
    }
}

// Find tokens of specific type
TokenArray* find_tokens_by_type(TokenArray* tokens, TokenType type) {
    if (!tokens) return NULL;
    
    TokenArray* result = create_token_array();
    if (!result) return NULL;
    
    for (int i = 0; i < tokens->count; i++) {
        if (tokens->tokens[i].type == type) {
            Token token = create_token(tokens->tokens[i].type, 
                                     tokens->tokens[i].value,
                                     tokens->tokens[i].line,
                                     tokens->tokens[i].column);
            add_token(result, token);
        }
    }
    
    return result;
}

// Get token at specific position
Token* get_token_at_position(TokenArray* tokens, int line, int column) {
    if (!tokens) return NULL;
    
    for (int i = 0; i < tokens->count; i++) {
        Token* token = &tokens->tokens[i];
        if (token->line == line && 
            token->column <= column && 
            column < token->column + (int)strlen(token->value)) {
            return token;
        }
    }
    return NULL;
}

// Convert tokens to string array
char** tokenize_to_strings(TokenArray* tokens, int* count) {
    if (!tokens || !count) return NULL;
    
    *count = 0;
    for (int i = 0; i < tokens->count; i++) {
        if (tokens->tokens[i].type != TOKEN_EOF) {
            (*count)++;
        }
    }
    
    char** strings = malloc(sizeof(char*) * (*count));
    if (!strings) {
        *count = 0;
        return NULL;
    }
    
    int idx = 0;
    for (int i = 0; i < tokens->count; i++) {
        if (tokens->tokens[i].type != TOKEN_EOF) {
            strings[idx++] = string_duplicate(tokens->tokens[i].value);
        }
    }
    
    return strings;
}

// Free string array
void free_string_array(char** strings, int count) {
    if (!strings) return;
    
    for (int i = 0; i < count; i++) {
        free(strings[i]);
    }
    free(strings);
}

// Convenience function for quick tokenization
TokenArray* quick_tokenize(const char* input) {
    Tokenizer* tokenizer = create_tokenizer(NULL);
    TokenArray* tokens = tokenize(tokenizer, input);
    free_tokenizer(tokenizer);
    return tokens;
}

// Demonstration function
void demonstrate_tokenizer() {
    printf("=== Tokenizer Demo ===\n\n");
    
    const char* code = 
        "fn main() {\n"
        "  let x = 42;\n"
        "  let name = \"Hello World\";\n"
        "  if (x > 0) {\n"
        "    print(\"Positive number\");\n"
        "  }\n"
        "  // This is a comment\n"
        "  return x * 2;\n"
        "}\n";
    
    // Basic usage
    Tokenizer* tokenizer = create_tokenizer(NULL);
    TokenArray* tokens = tokenize(tokenizer, code);
    
    printf("1. Basic tokenization:\n");
    pretty_print_tokens(tokens);
    printf("\n");
    
    // Get only identifiers
    printf("2. Only identifiers:\n");
    TokenArray* identifiers = find_tokens_by_type(tokens, TOKEN_IDENTIFIER);
    for (int i = 0; i < identifiers->count; i++) {
        printf("%s ", identifiers->tokens[i].value);
    }
    printf("\n\n");
    
    // Get tokens as strings
    printf("3. Tokens as strings:\n");
    int string_count;
    char** token_strings = tokenize_to_strings(tokens, &string_count);
    for (int i = 0; i < string_count; i++) {
        printf("\"%s\" ", token_strings[i]);
    }
    printf("\n\n");
    
    // Custom configuration with comments
    printf("4. With comments included:\n");
    TokenizerOptions custom_options = {0};
    custom_options.include_comments = true;
    custom_options.case_sensitive = true;
    custom_options.skip_unknown = true;
    
    Tokenizer* custom_tokenizer = create_tokenizer(&custom_options);
    TokenArray* custom_tokens = tokenize(custom_tokenizer, code);
    
    TokenArray* comments = find_tokens_by_type(custom_tokens, TOKEN_COMMENT);
    printf("Found %d comments:\n", comments->count);
    for (int i = 0; i < comments->count; i++) {
        printf("  \"%s\"\n", comments->tokens[i].value);
    }
    
    // Cleanup
    free_string_array(token_strings, string_count);
    free_token_array(tokens);
    free_token_array(identifiers);
    free_token_array(custom_tokens);
    free_token_array(comments);
    free_tokenizer(tokenizer);
    free_tokenizer(custom_tokenizer);
}

// Example main function
int main() {
    demonstrate_tokenizer();
    
    // Simple example
    printf("\n=== Simple Example ===\n");
    const char* simple_code = "int x = 10 + 20;";
    TokenArray* simple_tokens = quick_tokenize(simple_code);
    
    printf("Input: %s\n", simple_code);
    printf("Tokens:\n");
    pretty_print_tokens(simple_tokens);
    
    free_token_array(simple_tokens);
    
    return 0;
}


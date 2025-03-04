#pragma once

/* Token types */
struct token
{
    enum
    {
        key_class = 1,
        key_else,
        key_false,
        key_fi,
        key_if,
        key_in,
        key_inherits,
        key_isvoid,
        key_let,
        key_loop,
        key_pool,
        key_then,
        key_while,
        key_case,
        key_esac,
        key_new,
        key_of,
        key_not,
        key_true,

        class_Object,
        class_Int,
        class_String,
        class_Bool,

        id,

        expr_assign,

        op_plus,
        op_minus,
        op_mul,
        op_div,
        op_lt,
        op_le,
        op_eq,

        brace_l,
        brace_r,
        paren_l,
        paren_r,

        str_lit,

        tok_unknown,
    };
};

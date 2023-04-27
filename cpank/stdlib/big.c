#include <gmp.h>
#include <mpfr.h>
#include <stdbool.h>
#include <stdint.h>

#include "../include/stdlib.h"
#include "../include/vm.h"

#define DO_ADD  20
#define DO_SUB  21
#define DO_MUL  22
#define DO_DIV  23
#define DO_POW  24

#define DO_SQRT 30
#define DO_LOG  31
#define DO_SIN  32
#define DO_COS  33
#define DO_TAN  34

#define DO_GT   40
#define DO_LT   41
#define DO_EQ   42
#define DO_NEQ  43

#define GT      130
#define LT      131
#define EQ      132

void int_pow(mpz_t result, mpz_t left, mpz_t right) {
    mpfr_t l_float, r_float, result_float;
    mpfr_init2(result_float, BIGFLOAT_MINPREC);
    mpfr_init2(l_float, BIGFLOAT_MINPREC);
    mpfr_set_z(l_float, left, BIGFLOAT_ROUND);

    mpfr_init2(r_float, BIGFLOAT_MINPREC);
    mpfr_set_z(r_float, right, BIGFLOAT_ROUND);

    mpfr_pow(result_float, l_float, r_float, BIGFLOAT_ROUND);
    mpfr_clear(l_float);
    mpfr_clear(r_float);
    mpfr_get_z(result, result_float, BIGFLOAT_ROUND);
    mpfr_clear(result_float);
}

void do_float_bin_calc(mpfr_t result, mpfr_t left, mpfr_t right, uint8_t op) {
    switch (op) {
        case DO_ADD:
            mpfr_add(result, left, right, BIGFLOAT_ROUND);
            break;
        case DO_SUB:
            mpfr_sub(result, left, right, BIGFLOAT_ROUND);
            break;
        case DO_MUL:
            mpfr_mul(result, left, right, BIGFLOAT_ROUND);
            break;
        case DO_DIV:
            mpfr_div(result, left, right, BIGFLOAT_ROUND);
            // mpf_set_prec(result , 100);
            break;
        case DO_POW:
            mpfr_pow(result, left, right, BIGFLOAT_ROUND);
            break;
    }
}

void do_int_bin_calc(mpz_t result, mpz_t left, mpz_t right, uint8_t op) {
    switch (op) {
        case DO_ADD:
            mpz_add(result, left, right);
            break;
        case DO_SUB:
            mpz_sub(result, left, right);
            break;
        case DO_MUL:
            mpz_mul(result, left, right);
            break;
        case DO_DIV:
            mpz_div(result, left, right);
            break;
        case DO_POW:
            int_pow(result, left, right);
            break;
    }
}

uint8_t do_int_comp(mpz_t left, mpz_t right) {
    int result = mpz_cmp(left, right);
    if (result > 0) {
        return GT;
    } else if (result == 0) {
        return EQ;
    } else if (result < 0) {
        return LT;
    }
    return 0;
}

uint8_t do_float_comp(mpfr_t left, mpfr_t right) {
    int result = mpfr_cmp(left, right);
    if (result > 0) {
        return GT;
    } else if (result == 0) {
        return EQ;
    } else if (result < 0) {
        return LT;
    }
    return 0;
}

void do_float_single_calc(mpfr_t result, mpfr_t x, uint8_t op) {
    switch (op) {
        case DO_SQRT:
            mpfr_sqrt(result, x, BIGFLOAT_ROUND);
            break;
        case DO_LOG:
            mpfr_log(result, x, BIGFLOAT_ROUND);
            break;
        case DO_SIN:
            mpfr_sin(result, x, BIGFLOAT_ROUND);
            break;
        case DO_COS:
            mpfr_cos(result, x, BIGFLOAT_ROUND);
        case DO_TAN:
            mpfr_tan(result, x, BIGFLOAT_ROUND);
            break;
    }
}

Value do_single_big_calc(PankVm *vm, ObjBigNum *x, uint8_t op) {
    mpfr_t result;
    mpfr_init2(result, BIGFLOAT_MINPREC);
    if (x->isfloat) {
        do_float_single_calc(result, x->as.fval, op);
        Value v = make_obj_val(new_bignum_with_mpf(vm, result));
        push(vm, v);
        mpfr_clear(result);
        return pop(vm);
    } else {
        mpfr_t input;
        mpfr_init2(input, BIGFLOAT_MINPREC);
        mpfr_set_z(input, x->as.ival, BIGFLOAT_ROUND);
        do_float_single_calc(result, input, op);
        Value v = make_obj_val(new_bignum_with_mpf(vm, result));
        push(vm, v);
        mpfr_clear(result);
        mpfr_clear(input);
        return pop(vm);
    }

    return make_bool(false);
}

Value do_big_calc(PankVm *vm, ObjBigNum *left, ObjBigNum *right, uint8_t op) {
    if (left->isfloat || right->isfloat) {
        mpfr_t f_left;
        mpfr_t f_right;

        mpfr_init2(f_left, BIGFLOAT_MINPREC);
        mpfr_init2(f_right, BIGFLOAT_MINPREC);

        if (left->isfloat) {
            mpfr_set(f_left, left->as.fval, BIGFLOAT_ROUND);
        } else {
            mpfr_set_z(f_right, left->as.ival, BIGFLOAT_MINPREC);
        }

        if (right->isfloat) {
            mpfr_set(f_right, right->as.fval, BIGFLOAT_ROUND);
        } else {
            mpfr_set_z(f_right, right->as.ival, BIGFLOAT_ROUND);
        }
        if (op >= DO_ADD && op <= DO_POW) {
            mpfr_t result;
            mpfr_init2(result, BIGFLOAT_MINPREC);
            // mpf_set_prec(result , 200);
            // mpf_init2(result , 20);
            do_float_bin_calc(result, f_left, f_right, op);
            Value v = make_obj_val(new_bignum_with_mpf(vm, result));
            mpfr_clear(f_left);
            mpfr_clear(f_right);
            mpfr_clear(result);
            return v;
        } else if (op >= DO_GT && op <= DO_NEQ) {
            uint8_t res = do_float_comp(f_left, f_right);
            if (res == GT && op == DO_GT) {
                return make_bool(true);
            } else if (res == LT && op == DO_LT) {
                return make_bool(true);
            } else if (res == EQ && op == DO_EQ) {
                return make_bool(true);
            } else if (res != EQ && op == DO_NEQ) {
                return make_bool(true);
            } else {
                return make_bool(false);
            }
        }

    } else {
        if (op >= DO_ADD && op <= DO_POW) {
            mpz_t result;
            mpz_init(result);
            do_int_bin_calc(result, left->as.ival, right->as.ival, op);
            Value v = make_obj_val(new_bignum_with_mpz(vm, result));
            mpz_clear(result);
            return v;
        } else if (op >= DO_GT && op <= DO_NEQ) {
            uint8_t res = do_int_comp(left->as.ival, right->as.ival);
            if (res == GT && op == DO_GT) {
                return make_bool(true);
            } else if (res == LT && op == DO_LT) {
                return make_bool(true);
            } else if (res == EQ && op == DO_EQ) {
                return make_bool(true);
            } else if (res != EQ && op == DO_NEQ) {
                return make_bool(true);
            } else {
                return make_bool(false);
            }
        }
    }

    return make_bool(false);
}

Value _big_add(PankVm *vm, int argc, Value *args) {
    check_argc_count("add(a,b )", 2, argc);
    Value raw_a = args[0];
    Value raw_b = args[1];
    if (!is_bignum_obj(raw_a) || !is_bignum_obj(raw_b)) {
        return make_error(
            vm, U"both arguments to big.add(a,b) must be big numbers");
    }

    ObjBigNum *left = get_as_bignum(raw_a);
    ObjBigNum *right = get_as_bignum(raw_b);
    return do_big_calc(vm, left, right, DO_ADD);
}

Value _big_sub(PankVm *vm, int argc, Value *args) {
    check_argc_count("sub(a, b)", 2, argc);
    Value raw_a = args[0];
    Value raw_b = args[1];
    if (!is_bignum_obj(raw_a) || !is_bignum_obj(raw_b)) {
        return make_error(
            vm, U"both arguments to big.sub(a,b) must be big numbers");
    }

    ObjBigNum *left = get_as_bignum(raw_a);
    ObjBigNum *right = get_as_bignum(raw_b);
    return do_big_calc(vm, left, right, DO_SUB);
}

Value _big_mul(PankVm *vm, int argc, Value *args) {
    check_argc_count("mul(a, b)", 2, argc);
    Value raw_a = args[0];
    Value raw_b = args[1];
    if (!is_bignum_obj(raw_a) || !is_bignum_obj(raw_b)) {
        return make_error(
            vm, U"both arguments to big.mul(a,b) must be big numbers");
    }

    ObjBigNum *left = get_as_bignum(raw_a);
    ObjBigNum *right = get_as_bignum(raw_b);
    return do_big_calc(vm, left, right, DO_MUL);
}

Value _big_div(PankVm *vm, int argc, Value *args) {
    check_argc_count("div(a, b)", 2, argc);
    Value raw_a = args[0];
    Value raw_b = args[1];
    if (!is_bignum_obj(raw_a) || !is_bignum_obj(raw_b)) {
        return make_error(
            vm, U"both arguments to big.div(a,b) must be big numbers");
    }

    ObjBigNum *left = get_as_bignum(raw_a);
    ObjBigNum *right = get_as_bignum(raw_b);
    return do_big_calc(vm, left, right, DO_DIV);
}

Value _big_pow(PankVm *vm, int argc, Value *args) {
    check_argc_count("pow(a, b)", 2, argc);
    Value raw_a = args[0];
    Value raw_b = args[1];
    if (!is_bignum_obj(raw_a) || !is_bignum_obj(raw_b)) {
        return make_error(
            vm, U"both arguments to big.pow(a,b) must be big numbers");
    }

    ObjBigNum *left = get_as_bignum(raw_a);
    ObjBigNum *right = get_as_bignum(raw_b);
    return do_big_calc(vm, left, right, DO_POW);
}

Value _big_comp_gt(PankVm *vm, int argc, Value *args) {
    check_argc_count("gt(a, b)", 2, argc);
    Value raw_a = args[0];
    Value raw_b = args[1];
    if (!is_bignum_obj(raw_a) || !is_bignum_obj(raw_b)) {
        return make_error(vm,
                          U"both arguments to big.gt(a,b) must be big numbers");
    }

    ObjBigNum *left = get_as_bignum(raw_a);
    ObjBigNum *right = get_as_bignum(raw_b);
    return do_big_calc(vm, left, right, DO_GT);
}

Value _big_comp_lt(PankVm *vm, int argc, Value *args) {
    check_argc_count("lt(a, b)", 2, argc);
    Value raw_a = args[0];
    Value raw_b = args[1];
    if (!is_bignum_obj(raw_a) || !is_bignum_obj(raw_b)) {
        return make_error(vm,
                          U"both arguments to big.lt(a,b) must be big numbers");
    }

    ObjBigNum *left = get_as_bignum(raw_a);
    ObjBigNum *right = get_as_bignum(raw_b);
    return do_big_calc(vm, left, right, DO_LT);
}

Value _big_comp_eq(PankVm *vm, int argc, Value *args) {
    check_argc_count("eq(a, b)", 2, argc);
    Value raw_a = args[0];
    Value raw_b = args[1];
    if (!is_bignum_obj(raw_a) || !is_bignum_obj(raw_b)) {
        return make_error(vm,
                          U"both arguments to big.eq(a,b) must be big numbers");
    }

    ObjBigNum *left = get_as_bignum(raw_a);
    ObjBigNum *right = get_as_bignum(raw_b);
    return do_big_calc(vm, left, right, DO_EQ);
}

Value _big_comp_noteq(PankVm *vm, int argc, Value *args) {
    check_argc_count("noteq(a, b)", 2, argc);
    Value raw_a = args[0];
    Value raw_b = args[1];
    if (!is_bignum_obj(raw_a) || !is_bignum_obj(raw_b)) {
        return make_error(
            vm, U"both arguments to big.noteq(a,b) must be big numbers");
    }

    ObjBigNum *left = get_as_bignum(raw_a);
    ObjBigNum *right = get_as_bignum(raw_b);
    return do_big_calc(vm, left, right, DO_NEQ);
}

Value _big_sqrt(PankVm *vm, int argc, Value *args) {
    check_argc_count("sqrt(a)", 1, argc);
    Value raw_a = args[0];
    if (!is_bignum_obj(raw_a)) {
        return make_error(vm,
                          U"both arguments to big.sqrt(a) must be big numbers");
    }

    ObjBigNum *left = get_as_bignum(raw_a);
    return do_single_big_calc(vm, left, DO_SQRT);
}

Value _big_log(PankVm *vm, int argc, Value *args) {
    check_argc_count("log(a)", 1, argc);
    Value raw_a = args[0];
    if (!is_bignum_obj(raw_a)) {
        return make_error(vm,
                          U"both arguments to big.log(a) must be big numbers");
    }

    ObjBigNum *left = get_as_bignum(raw_a);
    return do_single_big_calc(vm, left, DO_LOG);
}

Value _big_sin(PankVm *vm, int argc, Value *args) {
    check_argc_count("sin(a)", 1, argc);
    Value raw_a = args[0];
    if (!is_bignum_obj(raw_a)) {
        return make_error(vm,
                          U"both arguments to big.sin(a) must be big numbers");
    }

    ObjBigNum *left = get_as_bignum(raw_a);
    return do_single_big_calc(vm, left, DO_SIN);
}

Value _big_cos(PankVm *vm, int argc, Value *args) {
    check_argc_count("cos(a)", 1, argc);
    Value raw_a = args[0];
    if (!is_bignum_obj(raw_a)) {
        return make_error(vm,
                          U"both arguments to big.cos(a) must be big numbers");
    }

    ObjBigNum *left = get_as_bignum(raw_a);
    return do_single_big_calc(vm, left, DO_COS);
}

Value _big_tan(PankVm *vm, int argc, Value *args) {
    check_argc_count("tan(a)", 1, argc);
    Value raw_a = args[0];
    if (!is_bignum_obj(raw_a)) {
        return make_error(vm,
                          U"both arguments to big.tan(a) must be big numbers");
    }

    ObjBigNum *left = get_as_bignum(raw_a);
    return do_single_big_calc(vm, left, DO_TAN);
}

Value _big_const_pi(PankVm *vm, int argc, Value *args) {
    if (argc != 0) {
        return make_error(vm, U"pi() doesn't take any arguments");
    }

    mpfr_t r;
    mpfr_init2(r, BIGFLOAT_MINPREC);
    mpfr_const_pi(r, BIGFLOAT_ROUND);
    Value v = make_obj_val(new_bignum_with_mpf(vm, r));
    push(vm, v);
    mpfr_clear(r);
    return pop(vm);
}

Value _big_const_e(PankVm *vm, int argc, Value *args) {
    if (argc != 0) {
        return make_error(vm, U"e() doesn't take any arguments");
    }

    mpfr_t r;
    mpfr_init2(r, BIGFLOAT_MINPREC);
    mpfr_const_euler(r, BIGFLOAT_ROUND);
    Value v = make_obj_val(new_bignum_with_mpf(vm, r));
    push(vm, v);
    mpfr_clear(r);
    return pop(vm);
}

void push_stdlib_big(PankVm *vm) {
    SL sls[] = {
        msl(U"add", _big_add),          msl(U"sub", _big_sub),
        msl(U"mul", _big_mul),          msl(U"div", _big_div),
        msl(U"gt", _big_comp_gt),       msl(U"lt", _big_comp_lt),
        msl(U"noteq", _big_comp_noteq), msl(U"eq", _big_comp_eq),
        msl(U"sqrt", _big_sqrt),        msl(U"sin", _big_sin),
        msl(U"cos", _big_cos),          msl(U"tan", _big_tan),
        msl(U"log", _big_log),          msl(U"pi", _big_const_pi),
        msl(U"e", _big_const_e),        msl(U"pow", _big_pow),
    };

    _push_stdlib(vm, U"big", sls, 16);
}

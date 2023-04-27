#include <gmp.h>
#include <mpfr.h>
#include <stdbool.h>
#include <stdint.h>

#include "../include/stdlib.h"
#include "../include/vm.h"

#define DO_ADD 20
#define DO_SUB 21
#define DO_MUL 22
#define DO_DIV 23

void do_float_calc(mpfr_t result, mpfr_t left, mpfr_t right, uint8_t op) {
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
    }
}

void do_int_calc(mpz_t result, mpz_t left, mpz_t right, uint8_t op) {
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
    }
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
        mpfr_t result;
        mpfr_init2(result, 200);
        // mpf_set_prec(result , 200);
        // mpf_init2(result , 20);
        do_float_calc(result, f_left, f_right, op);
        Value v = make_obj_val(new_bignum_with_mpf(vm, result));
        mpfr_clear(f_left);
        mpfr_clear(f_right);
        mpfr_clear(result);
        return v;
    } else {
        mpz_t result;
        mpz_init(result);
        do_int_calc(result, left->as.ival, right->as.ival, op);
        Value v = make_obj_val(new_bignum_with_mpz(vm, result));
        mpz_clear(result);
        return v;
    }
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

void push_stdlib_big(PankVm *vm) {
    SL sls[] = {
        msl(U"add", _big_add),
        msl(U"sub", _big_sub),
        msl(U"mul", _big_mul),
        msl(U"div", _big_div),
    };

    _push_stdlib(vm, U"big", sls, 4);
}

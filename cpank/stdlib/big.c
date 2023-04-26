#include <gmp.h>
#include <stdbool.h>
#include <stdint.h>

#include "../include/stdlib.h"
#include "../include/vm.h"

#define DO_ADD 20
#define DO_SUB 21
#define DO_MUL 22
#define DO_DIV 23

void do_float_calc(mpf_t result, mpf_t left, mpf_t right, uint8_t op) {
    switch (op) {
        case DO_ADD:
            mpf_add(result, left, right);
            break;
        case DO_SUB:
            mpf_sub(result, left, right);
            break;
        case DO_MUL:
            mpf_mul(result, left, right);
            break;
        case DO_DIV:
            mpf_div(result, left, right);
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
        mpf_t f_left;
        mpf_t f_right;

        mpf_init(f_left);
        mpf_init(f_right);

        if (left->isfloat) {
            mpf_set(f_left, left->as.fval);
        } else {
            mpf_set_z(f_right, left->as.ival);
        }

        if (right->isfloat) {
            mpf_set(f_right, right->as.fval);
        } else {
            mpf_set_z(f_right, right->as.ival);
        }
        mpf_t result;
        mpf_init(result);
        do_float_calc(result, f_left, f_right, op);
        Value v = make_obj_val(new_bignum_with_mpf(vm, result));
        mpf_clear(result);
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
    check_argc_count("add", 2, argc);
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

void push_stdlib_big(PankVm *vm) {
    SL sls[] = {
        msl(U"add", _big_add),
    };

    _push_stdlib(vm, U"big", sls, 1);
}

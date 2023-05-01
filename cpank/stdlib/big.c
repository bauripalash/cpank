#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include "../include/stdlib.h"
#include "../include/utils.h"
#include "../include/vm.h"

#define DO_ADD  20
#define DO_SUB  21
#define DO_MUL  22
#define DO_DIV  23
#define DO_POW  24

#define DO_SQRT 30
#define DO_LOG  31

#define DO_GT   40
#define DO_LT   41
#define DO_EQ   42
#define DO_NEQ  43

#define GT      130
#define LT      131
#define EQ      132

uint8_t do_bigint_bin_calc(mp_int *result, mp_int *left, mp_int *right,
                           uint8_t op) {
    switch (op) {
        case DO_ADD:
            if (mp_add(left, right, result) != MP_OKAY) {
                return op;
            }
            break;
            //            cp_println(L"<|>%s|%s<|>" , big_int_to_str(result) ,
            //            mp_error_to_string(r));

        case DO_SUB:
            if (mp_sub(left, right, result) != MP_OKAY) {
                return op;
            }
            break;
        case DO_MUL:
            if (mp_mul(left, right, result) != MP_OKAY) {
                return op;
            }
            break;
        case DO_DIV:
            if (mp_div(left, right, result, NULL) != MP_OKAY) {
                return op;
            }
            // mpf_set_prec(result , 100);
            break;
    }
    return 0;
}

long double do_bigfloat_bin_calc(long double left, long double right,
                                 uint8_t op) {
    switch (op) {
        case DO_ADD:
            return left + right;
        case DO_SUB:
            return left - right;
        case DO_DIV:
            return left / right;
        case DO_MUL:
            return left * right;
        default:
            return 0;
    }
}

uint8_t do_int_comp(mp_int *left, mp_int *right) {
    int result = mp_cmp(left, right);
    if (result == MP_GT) {
        return GT;
    } else if (result == MP_EQ) {
        return EQ;
    } else if (result == MP_LT) {
        return LT;
    }
    return 0;
}

uint8_t do_float_comp(long double left, long double right) {
    if (left > right) {
        return GT;
    } else if (left == right) {
        return EQ;
    } else if (left < right) {
        return LT;
    }
    return 0;
}

uint8_t do_bigint_single_calc(mp_int *result, mp_int *x, uint8_t op) {
    switch (op) {
        case DO_SQRT:
            if (mp_sqrt(x, result) != MP_OKAY) {
                return op;
            }
            break;
    }
    return 0;
}

uint8_t do_bigfloat_single_calc(long double *result, long double x,
                                uint8_t op) {
    switch (op) {
        case DO_SQRT:
            *result = sqrtl(x);
    }
    return 0;
}

Value do_single_big_calc(PankVm *vm, ObjBigNum *x, uint8_t op) {
    if (!x->isfloat) {
        mp_int result;
        if (mp_init(&result)) {
        }
        do_bigint_single_calc(&result, &x->as.ival, op);
        Value v = make_obj_val(new_bignum_with_mpint(vm, &result));
        push(vm, v);
        mp_clear(&result);
        return pop(vm);
    } else {
        double long result;
        do_bigfloat_single_calc(&result, x->as.fval, op);
        Value v = make_obj_val(new_bignum_with_ld(vm, result));
        push(vm, v);
        return pop(vm);
    }

    return make_bool(false);
}

Value do_big_calc(PankVm *vm, ObjBigNum *left, ObjBigNum *right, uint8_t op) {
    if (left->isfloat || right->isfloat) {
        long double l_d;
        long double r_d;

        if (left->isfloat) {
            l_d = left->as.fval;
            r_d = (long double)mp_get_double(&right->as.ival);
        } else {
            r_d = right->as.fval;
            l_d = (long double)mp_get_double(&left->as.ival);
        }

        // cp_println(L"->ld %Lg | -> %Lg", l_d, r_d);

        if (op >= DO_ADD && op <= DO_POW) {
            Value v = make_obj_val(
                new_bignum_with_ld(vm, do_bigfloat_bin_calc(l_d, r_d, op)));
            return v;
        } else if (op >= DO_GT && op <= DO_NEQ) {
            uint8_t res = do_float_comp(l_d, r_d);
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
        // cp_print(L"->");
        // print_val(make_obj_val(right));
        // cp_println(L"<-");
        // cp_print(L"->");
        // print_val(make_obj_val(left));
        // cp_println(L"<-");
        if (op >= DO_ADD && op <= DO_POW) {
            mp_int result;
            if (mp_init(&result) != MP_OKAY) {
                return make_nil;
            }
            do_bigint_bin_calc(&result, &left->as.ival, &right->as.ival, op);
            // mp_add(&left->as.ival, &right->as.ival, &result);

            // cp_println(L"OPOP->%s", big_int_to_str(&result));
            Value v = make_obj_val(new_bignum_with_mpint(vm, &result));

            // cp_print(L"RES->");
            // cp_println(L"%s" , big_int_to_str(&result));
            // cp_println(L"<-");
            mp_clear(&result);
            return v;
        } else if (op >= DO_GT && op <= DO_NEQ) {
            uint8_t res = do_int_comp(&left->as.ival, &right->as.ival);
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

Value _big_const_pi(PankVm *vm, int argc, Value *args) {
    if (argc != 0) {
        return make_error(vm, U"pi() doesn't take any arguments");
    }

    Value v = make_obj_val(new_bignum_with_ld(vm, (long double)CONST_PI));
    push(vm, v);
    return pop(vm);
}

Value _big_const_e(PankVm *vm, int argc, Value *args) {
    if (argc != 0) {
        return make_error(vm, U"e() doesn't take any arguments");
    }

    Value v = make_obj_val(new_bignum_with_ld(vm, (long double)CONST_E));
    push(vm, v);
    return pop(vm);
}

void push_stdlib_big(PankVm *vm) {
    SL sls[] = {
        msl(U"add", _big_add),          msl(U"sub", _big_sub),
        msl(U"mul", _big_mul),          msl(U"div", _big_div),
        msl(U"gt", _big_comp_gt),       msl(U"lt", _big_comp_lt),
        msl(U"noteq", _big_comp_noteq), msl(U"eq", _big_comp_eq),
        msl(U"sqrt", _big_sqrt),        msl(U"log", _big_log),
        msl(U"pi", _big_const_pi),      msl(U"e", _big_const_e),
        msl(U"pow", _big_pow),
    };

    _push_stdlib(vm, U"big", sls, 13);
}

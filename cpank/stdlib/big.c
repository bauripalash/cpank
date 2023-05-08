#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/stdlib.h"
#include "../include/utils.h"
#include "../include/vm.h"
#include "../ext/baurinum/baurinum.h"

#define DO_ADD  20
#define DO_SUB  21

#define DO_SQRT 30
#define DO_LOG  31

#define DO_GT   40
#define DO_LT   41
#define DO_EQ   42
#define DO_NEQ  43

#define GT      130
#define LT      131
#define EQ      132

uint8_t do_bigint_bin_calc(bnum *result, bnum *left, bnum *right,
                           uint8_t op) {
    switch (op) {
        case DO_ADD:
            if (bn_add(result , left, right) != BN_OK) {
                return op;
            }
            break;
            //            cp_println(L"<|>%s|%s<|>" , big_int_to_str(result) ,
            //            mp_error_to_string(r));

        case DO_SUB:
            if (bn_sub(result , left, right) != BN_OK) {
                return op;
            }
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
        default:
            return 0;
    }
}

uint8_t do_int_comp(bnum *left, bnum *right) {
    int result = bn_cmp(left, right);
    if (result == BN_GT) {
        return GT;
    } else if (result == BN_EQ) {
        return EQ;
    } else if (result == BN_LT) {
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






Value do_big_calc(PankVm *vm, ObjBigNum *left, ObjBigNum *right, uint8_t op) {
    if (left->isfloat || right->isfloat) {
        long double l_d;
        long double r_d;

        if (left->isfloat) {
            l_d = left->as.fval;
            char * rval_str = bn_as_str(&right->as.ival, false);
            r_d = strtold(rval_str, NULL);
            free(rval_str);
        
        } else {
            r_d = right->as.fval;

            char * lval_str = bn_as_str(&left->as.ival, false);
            l_d = strtold(lval_str, NULL);
            free(lval_str);
        }


        if (op == DO_ADD || op == DO_SUB) {
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
       /* cp_print(L"->");
         print_val(make_obj_val(right));
         cp_println(L"<-");
         cp_print(L"->");
         print_val(make_obj_val(left));
         cp_println(L"<-");*/

        if (op == DO_ADD || op == DO_SUB) {
            bnum result;
            if (bn_boot(&result) != BN_OK) {
                return make_nil;
            }
            do_bigint_bin_calc(&result, &left->as.ival, &right->as.ival, op);
            // mp_add(&left->as.ival, &right->as.ival, &result);

            //cp_println(L"OPOP->%s", big_int_to_str(&result));
            Value v = make_obj_val(new_bignum_with_mpint(vm, &result));
            //print_val(v);

            // cp_print(L"RES->");
            // cp_println(L"%s" , big_int_to_str(&result));
            // cp_println(L"<-");
            bn_clear(&result);
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
        msl(U"gt", _big_comp_gt),       msl(U"lt", _big_comp_lt),
        msl(U"noteq", _big_comp_noteq), msl(U"eq", _big_comp_eq),
        msl(U"pi", _big_const_pi),      msl(U"e", _big_const_e),
    };

    _push_stdlib(vm, U"big", sls, 8);
}

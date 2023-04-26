#include <gmp.h>
#include <stdbool.h>

#include "../include/stdlib.h"
#include "../include/vm.h"

Value _big_add(PankVm *vm, int argc, Value *args) {
    check_argc_count("add", 2, argc);
    Value raw_a = args[0];
    Value raw_b = args[1];
    if (!is_bignum_obj(raw_a) || !is_bignum_obj(raw_b)) {
        return make_error(
            vm, U"both arguments to big.add(a,b) must be big numbers");
    }

    ObjBigNum *big_a = get_as_bignum(raw_a);
    ObjBigNum *big_b = get_as_bignum(raw_b);
    if (big_a->isfloat || big_b->isfloat) {
        mpf_t f_a;
        mpf_t f_b;

        mpf_init(f_a);
        mpf_init(f_b);

        if (big_a->isfloat) {
            mpf_set(f_a, big_a->as.fval);
        } else {
            mpf_set_z(f_a, big_a->as.ival);
        }

        if (big_b->isfloat) {
            mpf_set(f_b, big_b->as.fval);
        } else {
            mpf_set_z(f_b, big_b->as.ival);
        }
        mpf_t result;
        mpf_init(result);
        mpf_add(result, f_a, f_b);
        Value v = make_obj_val(new_bignum_with_mpf(vm, result));
        mpf_clear(result);
        return v;
    } else {
        mpz_t result;
        mpz_init(result);
        mpz_add(result, big_a->as.ival, big_b->as.ival);
        Value v = make_obj_val(new_bignum_with_mpz(vm, result));
        mpz_clear(result);
        return v;
    }
}

void push_stdlib_big(PankVm *vm) {
    SL sls[] = {
        msl(U"add", _big_add),
    };

    _push_stdlib(vm, U"big", sls, 1);
}

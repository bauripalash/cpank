#include <math.h>
#include <wchar.h>

#include "../include/stdlib.h"
#include "../include/value.h"

Value _math_pow(int argc, Value* args) {
    if (argc != 2) {
        return make_nil();
    }

    if (args[0].type != args[1].type && args[0].type != V_NUM) {
        return make_nil();
    }

    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);
    double raw_result = pow(a, b);
    return make_num(raw_result);
}

Value _math_add(int argc, Value* args) {
    if (argc != 2) {
        return make_nil();
    }

    if (args[0].type != args[1].type && args[0].type != V_NUM) {
        return make_nil();
    }

    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);
    double raw_result = a + b;
    return make_num(raw_result);
}

double _get_gcd(double a, double b) {
    double x = (a > 0) ? a : -a;
    double y = (b > 0) ? b : -b;

    while (x != y) {
        if (x > y) {
            x -= y;
        } else {
            y -= x;
        }
    }

    return x;
}

Value _math_gcd(int argc, Value* args) {
    if (argc != 2) {
        return make_nil();
    }

    if (args[0].type != args[1].type && args[0].type != V_NUM) {
        return make_nil();
    }

    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);

    return make_num(_get_gcd(a, b));
}

Value _math_lcm(int argc, Value* args) {
    if (argc != 2) {
        return make_nil();
    }

    if (args[0].type != args[1].type && args[0].type != V_NUM) {
        return make_nil();
    }
    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);

    return make_num(a * b / _get_gcd(a, b));
}

void push_stdlib_math() {
    SL sls[] = {
        msl(L"pow", _math_pow),
        msl(L"add", _math_add),
        msl(L"gcd", _math_gcd),
        msl(L"lcm", _math_lcm),
    };

    _push_stdlib(L"math", sls, 4);
}

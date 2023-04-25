/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/stdlib.h"
#include "../include/utils.h"
#include "../include/value.h"
#include "../include/vm.h"

#define _USE_MATH_DEFINES

Value _bn_math_pow(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u0998\u09be\u09a4(\u0995, \u0996)", 2, argc);
    // function name -> ঘাত(ক, খ)

    if (!is_num(args[0]) && !is_num(args[1])) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 \u0998\u09be\u09a4(\u0995 , "
            U"\u0996) \u0995\u09be\u099c\u099f\u09bf \u09b6\u09c1\u09a7\u09c1 "
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be \u09ae\u09be\u09a8 "
            U"\u0997\u09cd\u09b0\u09b9\u09a3 \u0995\u09b0\u09c7");

        // MSG -> গণিতের ঘাত(ক , খ) কাজটি শুধু সংখ্যা মান গ্রহণ করে
    }

    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);
    double raw_result = pow(a, b);
    return make_num(raw_result);
}

Value _bn_math_add(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u09af\u09cb\u0997(\u0995 , \u0996)", 2, argc);
    // function name -> যোগ(ক, খ)
    //     if (args[0].type != args[1].type && args[0].type != V_NUM) {
    if (!is_num(args[0]) && !is_num(args[1])) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 \u09af\u09cb\u0997(\u0995, "
            U"\u0996) \u0995\u09be\u099c\u099f\u09bf \u09b6\u09c1\u09a7\u09c1 "
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be \u09ae\u09be\u09a8 "
            U"\u0997\u09cd\u09b0\u09b9\u09a3 \u0995\u09b0\u09c7");
        // MSG -> গণিতের যোগ(ক , খ) কাজটি শুধু সংখ্যা মান গ্রহণ করে
    }

    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);
    double raw_result = a + b;
    return make_num(raw_result);
}

static double _get_gcd_bn(double a, double b) {
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

Value _bn_math_gcd(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u0997\u09b8\u09be\u0997\u09c1(\u0995 , \u0996)", 2,
                        argc);
    // গসাগু(ক, খ)

    //    if (args[0].type != args[1].type && args[0].type != V_NUM) {
    if (!is_num(args[0]) && !is_num(args[1])) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 "
            U"\u0997\u09b8\u09be\u0997\u09c1(\u0995 , \u0996) "
            U"\u0995\u09be\u099c\u099f\u09bf \u09b6\u09c1\u09a7\u09c1 "
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be \u09ae\u09be\u09a8");
        // MSG -> গণিতের গসাগু(ক , খ) কাজটি শুধু সংখ্যা মান
    }

    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);

    return make_num(_get_gcd_bn(a, b));
}

Value _bn_math_lcm(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u09b2\u09b8\u09be\u0997\u09c1(\u0995 , \u0996)", 2,
                        argc);
    // লসাগু(ক, খ)

    if (!is_num(args[0]) && !is_num(args[1])) {
        //    if (args[0].type != args[1].type && args[0].type != V_NUM) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 "
            U"\u09b2\u09b8\u09be\u0997\u09c1(\u0995 , \u0996) "
            U"\u0995\u09be\u099c\u099f\u09bf \u09b6\u09c1\u09a7\u09c1 "
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be \u09ae\u09be\u09a8 "
            U"\u0997\u09cd\u09b0\u09b9\u09a3 \u0995\u09b0\u09c7");
        // MSG -> গণিতের লসাগু(ক , খ) কাজটি শুধু সংখ্যা মান গ্রহণ করে
    }
    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);

    return make_num(a * b / _get_gcd_bn(a, b));
}

Value _bn_math_sqrt(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u09ac\u09b0\u09cd\u0997\u09ae\u09c2\u09b2(\u0995)", 1,
                        argc);
    // বর্গমূল(ক)
    if (!is_num(args[0])) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 "
            U"\u09ac\u09b0\u09cd\u0997\u09ae\u09c2\u09b2(\u0995) "
            U"\u0995\u09be\u099c\u099f\u09bf \u09b6\u09c1\u09a7\u09c1 "
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be \u09ae\u09be\u09a8 "
            U"\u0997\u09cd\u09b0\u09b9\u09a3 \u0995\u09b0\u09c7");
        // MSG -> গণিতের বর্গমূল(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে
    }

    double a = get_as_number(args[0]);
    return make_num(sqrt(a));
}

Value _bn_math_log10(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u09b2\u0997\u09a6\u09b6(\u0995)", 1, argc);
    // লগদশ(ক)
    //  if (args[0].type != V_NUM) {

    if (!is_num(args[0])) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 "
            U"\u09b2\u0997\u09a6\u09b6(\u0995) \u0995\u09be\u099c\u099f\u09bf "
            U"\u09b6\u09c1\u09a7\u09c1 \u09b8\u0982\u0996\u09cd\u09af\u09be "
            U"\u09ae\u09be\u09a8 \u0997\u09cd\u09b0\u09b9\u09a3 "
            U"\u0995\u09b0\u09c7");
        // MSG -> গণিতের লগদশ(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে
    }
    double a = get_as_number(args[0]);

    return make_num(log10(a));
}
// Log base e
Value _bn_math_log_e(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u09b2\u0997(\u0995)", 1, argc);
    //

    if (!is_num(args[0])) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 \u09b2\u0997(\u0995) "
            U"\u0995\u09be\u099c\u099f\u09bf \u09b6\u09c1\u09a7\u09c1 "
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be \u09ae\u09be\u09a8 "
            U"\u0997\u09cd\u09b0\u09b9\u09a3 \u0995\u09b0\u09c7");
        // MSG -> গণিতের লগ(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে
    }
    double a = get_as_number(args[0]);

    return make_num(log(a));
}

// Log base X
Value _bn_math_logx(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u09b2\u0997\u09ac\u09c7\u09b8(\u0995 , \u0996)", 2,
                        argc);
    // লগবেস(ক, খ)

    if (!is_num(args[0]) && !is_num(args[1])) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 "
            U"\u09b2\u0997\u09ac\u09c7\u09b8(\u0995 , \u0996) "
            U"\u0995\u09be\u099c\u099f\u09bf \u09b6\u09c1\u09a7\u09c1 "
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be \u09ae\u09be\u09a8 "
            U"\u0997\u09cd\u09b0\u09b9\u09a3 \u0995\u09b0\u09c7");
        // MSG -> গণিতের লগবেস(ক , খ) কাজটি শুধু সংখ্যা মান গ্রহণ করে
    }

    double base = get_as_number(args[0]);
    double num = get_as_number(args[1]);

    return make_num(log(num) / log(base));
}

Value _bn_math_sine(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u09b8\u09be\u0987\u09a8(\u0995)", 1, argc);
    // সাইন(ক)
    if (!is_num(args[0])) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 "
            U"\u09b8\u09be\u0987\u09a8(\u0995) \u0995\u09be\u099c\u099f\u09bf "
            U"\u09b6\u09c1\u09a7\u09c1 \u09b8\u0982\u0996\u09cd\u09af\u09be "
            U"\u09ae\u09be\u09a8 \u0997\u09cd\u09b0\u09b9\u09a3 "
            U"\u0995\u09b0\u09c7");
        // MSG -> গণিতের সাইন(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে
    }

    double n = get_as_number(args[0]);

    return make_num(sin(n));
}

Value _bn_math_cosine(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u0995\u09b8(\u0995)", 1, argc);
    // কস(ক)

    if (!is_num(args[0])) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 \u0995\u09b8(\u0995) "
            U"\u0995\u09be\u099c\u099f\u09bf \u09b6\u09c1\u09a7\u09c1 "
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be \u09ae\u09be\u09a8 "
            U"\u0997\u09cd\u09b0\u09b9\u09a3 \u0995\u09b0\u09c7");
        // MSG -> গণিতের কস(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে
    }

    double n = get_as_number(args[0]);

    return make_num(cos(n));
}

Value _bn_math_tangent(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u099f\u09cd\u09af\u09be\u09a8(\u0995)", 1, argc);
    // ট্যান(ক)

    if (!is_num(args[0])) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 "
            U"\u099f\u09cd\u09af\u09be\u09a8(\u0995) "
            U"\u0995\u09be\u099c\u099f\u09bf \u09b6\u09c1\u09a7\u09c1 "
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be \u09ae\u09be\u09a8 "
            U"\u0997\u09cd\u09b0\u09b9\u09a3 \u0995\u09b0\u09c7");
        // MSG -> গণিতের ট্যান(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে
    }

    double n = get_as_number(args[0]);

    return make_num(tan(n));
}

Value _bn_math_to_degree(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u09a1\u09bf\u0997\u09cd\u09b0\u09bf(\u0995)", 1,
                        argc);
    // ডিগ্রি(ক)

    if (!is_num(args[0])) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 "
            U"\u09a1\u09bf\u0997\u09cd\u09b0\u09bf(\u0995) "
            U"\u0995\u09be\u099c\u099f\u09bf \u09b6\u09c1\u09a7\u09c1 "
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be \u09ae\u09be\u09a8 "
            U"\u0997\u09cd\u09b0\u09b9\u09a3 \u0995\u09b0\u09c7");
        // MSG -> গণিতের ডিগ্রি(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে
    }

    double n = get_as_number(args[0]);

    return make_num(n * (180 / CONST_PI));
}

Value _bn_math_to_radians(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u09b0\u09c7\u09a1\u09bf\u09df\u09be\u09a8(\u0995)", 1,
                        argc);
    // রেডিয়ান(ক)
    if (!is_num(args[0])) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 "
            U"\u09b0\u09c7\u09a1\u09bf\u09df\u09be\u09a8(\u0995) "
            U"\u0995\u09be\u099c\u099f\u09bf \u09b6\u09c1\u09a7\u09c1 "
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be \u09ae\u09be\u09a8 "
            U"\u0997\u09cd\u09b0\u09b9\u09a3 \u0995\u09b0\u09c7");
        // MSG -> গণিতের রেডিয়ান(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে
    }

    double n = get_as_number(args[0]);

    return make_num(n * (CONST_PI / 180));
}

Value _bn_math_get_pi(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u09aa\u09be\u0987()", 0, argc);
    // পাই()

    return make_num(CONST_PI);
}

Value _bn_math_get_e(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u0987()", 0, argc);
    // ই()

    return make_num(CONST_E);
}

Value _bn_math_str_to_num(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u09b8\u0982\u0996\u09cd\u09af\u09be(\u0995)", 1,
                        argc);
    // সংখ্যা(ক)

    if (!is_str_obj(args[0])) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 "
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be(\u0995) "
            U"\u0995\u09be\u099c\u099f\u09bf "
            U"\u09b6\u09c1\u09a7\u09c1\u09ae\u09be\u09a4\u09cd\u09b0 "
            U"\u09b8\u09cd\u099f\u09cd\u09b0\u09bf\u0982/\u09a8\u09be\u09ae "
            U"\u09ae\u09be\u09a8 \u0997\u09cd\u09b0\u09b9\u09a3 "
            U"\u0995\u09b0\u09c7");
        // MSG -> গণিতের সংখ্যা(ক) কাজটি শুধুমাত্র স্ট্রিং/নাম মান গ্রহণ করে
    }

    ObjString* ostr = get_as_string(args[0]);
    char* input = c_to_c(ostr->chars, ostr->len);
    double result = 0;
    scanf(input, U"%lf", &result);

    return make_num(result);
}

// cppcheck-suppress constParameter
Value _bn_math_get_random(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count(
        "\u098f\u09b2\u09cb\u09ae\u09c7\u09b2\u09cb_"
        "\u09b8\u0982\u0996\u09cd\u09af\u09be(\u0995)",
        1, argc);
    // এলোমেলো_সংখ্যা(ক)
    if (!is_num(args[0])) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 "
            U"\u098f\u09b2\u09cb\u09ae\u09c7\u09b2\u09cb_"
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be(\u0995) "
            U"\u0995\u09be\u099c\u099f\u09bf \u09b6\u09c1\u09a7\u09c1 "
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be \u09ae\u09be\u09a8 "
            U"\u0997\u09cd\u09b0\u09b9\u09a3 \u0995\u09b0\u09c7");
        // MSG-> গণিতের এলোমেলো_সংখ্যা(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে
    }
    srand((unsigned int)time(0));
    double result = ((double)rand() / (double)(RAND_MAX));

    return make_num(result);
}

Value _bn_math_get_random_from_range(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count(
        "\u098f\u09b2\u09cb\u09ae\u09c7\u09b2\u09cb("
        "\u09a8\u09bf\u09ae\u09cd\u09a8 , \u0989\u099a\u09cd\u099a)",
        2, argc);
    // এলোমেলো(নিম্ন, উচ্চ)
    if (!is_num(args[0]) && !is_num(args[1])) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 "
            U"\u098f\u09b2\u09cb\u09ae\u09c7\u09b2\u09cb("
            U"\u09a8\u09bf\u09ae\u09cd\u09a8, \u0989\u099a\u09cd\u099a) "
            U"\u0995\u09be\u099c\u099f\u09bf "
            U"\u09b6\u09c1\u09a7\u09c1\u09ae\u09be\u09a4\u09cd\u09b0 "
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be \u09ae\u09be\u09a8 "
            U"\u0997\u09cd\u09b0\u09b9\u09a3 \u0995\u09b0\u09c7");
        // MSG -> গণিতের এলোমেলো(নিম্ন, উচ্চ) কাজটি শুধুমাত্র সংখ্যা মান গ্রহণ করে
    }
    srand((unsigned int)time(0));
    double min = get_as_number(args[0]);
    double max = get_as_number(args[1]);
    double range = (max - min);
    double div = RAND_MAX / range;
    double result = min + (rand() / div);

    return make_num(result);
}

Value _bn_math_abs_num(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u09aa\u09b0\u09ae(\u0995)", 1, argc);
    // পরম(ক)

    if (!is_num(args[0])) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 \u09aa\u09b0\u09ae(\u0995) "
            U"\u0995\u09be\u099c\u099f\u09bf \u09b6\u09c1\u09a7\u09c1 "
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be \u09ae\u09be\u09a8 "
            U"\u0997\u09cd\u09b0\u09b9\u09a3 \u0995\u09b0\u09c7");
        // MSG -> গণিতের পরম(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে
    }

    double v = get_as_number(args[0]);

    return make_num(fabs(v));
}

Value _bn_math_round(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u09b0\u09be\u0989\u09a8\u09cd\u09a1(\u0995)", 1,
                        argc);
    // রাউন্ড(ক)

    if (!is_num(args[0])) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 "
            U"\u09b0\u09be\u0989\u09a8\u09cd\u09a1(\u0995) "
            U"\u0995\u09be\u099c\u099f\u09bf \u09b6\u09c1\u09a7\u09c1 "
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be \u09ae\u09be\u09a8 "
            U"\u0997\u09cd\u09b0\u09b9\u09a3 \u0995\u09b0\u09c7");
        // MSG -> গণিতের রাউন্ড(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে
    }

    double v = get_as_number(args[0]);

    return make_num(round(v));
}

Value _bn_math_get_infinity(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u0985\u09b8\u09c0\u09ae()", 0, argc);
    // অসীম()

    return make_num(INFINITY);
}

Value _bn_math_ceil(PankVm* vm, int argc, Value* args) {
    bn_check_argc_count("\u09b8\u09bf\u09b2(\u0995)", 1, argc);
    // সিল(ক)

    if (!is_num(args[0])) {
        return make_error(
            vm,
            U"\u0997\u09a3\u09bf\u09a4\u09c7\u09b0 \u09b8\u09bf\u09b2(\u0995) "
            U"\u0995\u09be\u099c\u099f\u09bf \u09b6\u09c1\u09a7\u09c1 "
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be \u09ae\u09be\u09a8 "
            U"\u0997\u09cd\u09b0\u09b9\u09a3 \u0995\u09b0\u09c7");
        // MSG -> গণিতের সিল(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে
    }

    double v = get_as_number(args[0]);
    return make_num(ceil(v));
}

void push_stdlib_math_bn(PankVm* vm) {
    SL sls[] = {
        msl(U"\u0998\u09be\u09a4", _bn_math_pow),
        msl(U"\u09af\u09cb\u0997", _bn_math_add),
        msl(U"\u0997\u09b8\u09be\u0997\u09c1", _bn_math_gcd),
        msl(U"\u09b2\u09b8\u09be\u0997\u09c1", _bn_math_lcm),
        msl(U"\u09ac\u09b0\u09cd\u0997\u09ae\u09c2\u09b2", _bn_math_sqrt),
        msl(U"\u09b2\u0997\u09a6\u09b6", _bn_math_log10),
        msl(U"\u09b2\u0997", _bn_math_log_e),
        msl(U"\u09b2\u0997\u09ac\u09c7\u09b8", _bn_math_logx),
        msl(U"\u09b8\u09be\u0987\u09a8", _bn_math_sine),
        msl(U"\u0995\u09b8", _bn_math_cosine),
        msl(U"\u099f\u09cd\u09af\u09be\u09a8", _bn_math_tangent),
        msl(U"\u09a1\u09bf\u0997\u09cd\u09b0\u09bf", _bn_math_to_degree),
        msl(U"\u09b0\u09c7\u09a1\u09bf\u09df\u09be\u09a8", _bn_math_to_radians),
        msl(U"\u09aa\u09be\u0987", _bn_math_get_pi),
        msl(U"\u0987", _bn_math_get_e),
        msl(U"\u09b8\u0982\u0996\u09cd\u09af\u09be", _bn_math_str_to_num),
        msl(U"\u098f\u09b2\u09cb\u09ae\u09c7\u09b2\u09cb_"
            U"\u09b8\u0982\u0996\u09cd\u09af\u09be",
            _bn_math_get_random),
        msl(U"\u098f\u09b2\u09cb\u09ae\u09c7\u09b2\u09cb",
            _bn_math_get_random_from_range),
        msl(U"\u09aa\u09b0\u09ae", _bn_math_abs_num),
        msl(U"\u09b0\u09be\u0989\u09a8\u09cd\u09a1", _bn_math_round),
        msl(U"\u0985\u09b8\u09c0\u09ae", _bn_math_get_infinity),
        msl(U"\u09b8\u09bf\u09b2", _bn_math_ceil),
    };

    _push_stdlib(vm, U"\u0997\u09a3\u09bf\u09a4", sls, 22);
}

/*
 * Copyright 2008 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef CMOCKA_H_
#define CMOCKA_H_

#ifdef _WIN32
#if _MSC_VER < 1500
#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */
int __stdcall IsDebuggerPresent();
#ifdef __cplusplus
} /* extern "C" */
#endif   /* __cplusplus */
#endif  /* _MSC_VER < 1500 */
#endif  /* _WIN32 */

/*
 * These headers or their equivalents should be included prior to including
 * this header file.
 *
 * #include <stdarg.h>
 * #include <stddef.h>
 * #include <setjmp.h>
 *
 * This allows test applications to use custom definitions of C standard
 * library functions and types.
 */

/* For those who are used to __func__ from gcc. */
#ifndef __func__
#define __func__ __FUNCTION__
#endif

/* GCC have printf type attribute check.  */
#ifdef __GNUC__
#define PRINTF_ATTRIBUTE(a,b) __attribute__ ((__format__ (__printf__, a, b)))
#else
#define PRINTF_ATTRIBUTE(a,b)
#endif /* __GNUC__ */

/**
 * @defgroup cmocka The CMocka API
 *
 * TODO Describe cmocka.
 *
 * @{
 */

/*
 * Largest integral type.  This type should be large enough to hold any
 * pointer or integer supported by the compiler.
 */
#ifndef LargestIntegralType
#define LargestIntegralType unsigned long long
#endif /* LargestIntegralType */

/* Printf format used to display LargestIntegralType. */
#ifndef LargestIntegralTypePrintfFormat
#ifdef _WIN32
#define LargestIntegralTypePrintfFormat "%I64x"
#else
#define LargestIntegralTypePrintfFormat "%llx"
#endif /* _WIN32 */
#endif /* LargestIntegralTypePrintfFormat */

/* Perform an unsigned cast to LargestIntegralType. */
#define cast_to_largest_integral_type(value) \
    ((LargestIntegralType)((size_t)(value)))

/* Smallest integral type capable of holding a pointer. */
#ifndef _UINTPTR_T
#define _UINTPTR_T
#ifdef _WIN32

/* WIN32 is an ILP32 platform */
typedef unsigned long uintptr_t;

/* what about 64-bit windows?
 * what's the right preprocessor symbol?
typedef unsigned long long uintptr_t */

#else /* _WIN32 */

/* ILP32 and LP64 platforms */
#ifdef __WORDSIZE /* glibc */
# if __WORDSIZE == 64
typedef unsigned long int uintptr_t;
# else
typedef unsigned int uintptr_t;
# endif /* __WORDSIZE == 64 */
#else /* __WORDSIZE */

# if defined(_LP64) || defined(_I32LPx)
typedef unsigned long int uintptr_t;
# else
typedef unsigned int uintptr_t;
# endif

#endif /* __WORDSIZE */

#endif /* _WIN32 */
#endif /* _UINTPTR_T */

/* Perform an unsigned cast to uintptr_t. */
#define cast_to_pointer_integral_type(value) \
    ((uintptr_t)(value))

/* Perform a cast of a pointer to uintmax_t */
#define cast_ptr_to_largest_integral_type(value) \
cast_to_largest_integral_type(cast_to_pointer_integral_type(value))

/**
 * @defgroup cmocka_mock Mock Objects
 * @ingroup cmocka.
 *
 * Mock objects mock objects are simulated objects that mimic the behavior of
 * real objects. Instead of calling the real objects, the tested object calls a
 * mock object that merely asserts that the correct methods were called, with
 * the expected parameters, in the correct order.
 *
 * @{
 */

#ifdef DOXYGEN
/**
 * @brief Retrieve a return value of the current function.
 *
 * @return The value which was stored to return by this function.
 *
 * @see will_return()
 */
void *mock(void);
#else
#define mock() _mock(__func__, __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a value to be returned by mock() later.
 *
 * @param[in]  #function  The function which should return the given value.
 *
 * @param[in]  value The value to be returned by mock().
 *
 * @code
 * int return_integer(void)
 * {
 *      return (int)mock();
 * }
 *
 * static void test_integer_return(void **state)
 * {
 *      will_return(return_integer, 42);
 *
 *      assert_int_equal(my_function_calling_return_integer(), 42);
 * }
 * @endcode
 *
 * @see mock()
 * @see will_return_count()
 */
void will_return(#function, void *value);
#else
#define will_return(function, value) \
    _will_return(#function, __FILE__, __LINE__, \
                 cast_to_largest_integral_type(value), 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a value to be returned by mock() later.
 *
 * @param[in]  #function  The function which should return the given value.
 *
 * @param[in]  value The value to be returned by mock().
 *
 * @param[in]  count The parameter returns the number of times the value should
 *                   be returned by mock(). If count is set to -1 the value will
 *                   always be returned.
 *
 * @see mock()
 */
void will_return_count(#function, void *value, int count);
#else
#define will_return_count(function, value, count) \
    _will_return(#function, __FILE__, __LINE__, \
                 cast_to_largest_integral_type(value), count)
#endif

/** @} */

/*
 * Add a custom parameter checking function.  If the event parameter is NULL
 * the event structure is allocated internally by this function.  If event
 * parameter is provided it must be allocated on the heap and doesn't need to
 * be deallocated by the caller.
 */
#define expect_check(function, parameter, check_function, check_data) \
    _expect_check(#function, #parameter, __FILE__, __LINE__, check_function, \
                  cast_to_largest_integral_type(check_data), NULL, 0)

/*
 * Add an event to check a parameter, using check_expected(), against a set of
 * values. See will_return() for a description of the count parameter.
 */
#define expect_in_set(function, parameter, value_array) \
    expect_in_set_count(function, parameter, value_array, 1)
#define expect_in_set_count(function, parameter, value_array, count) \
    _expect_in_set(#function, #parameter, __FILE__, __LINE__, value_array, \
                   sizeof(value_array) / sizeof((value_array)[0]), count)
#define expect_not_in_set(function, parameter, value_array) \
    expect_not_in_set_count(function, parameter, value_array, 1)
#define expect_not_in_set_count(function, parameter, value_array, count) \
    _expect_not_in_set( \
        #function, #parameter, __FILE__, __LINE__, value_array, \
        sizeof(value_array) / sizeof((value_array)[0]), count)


/*
 * Add an event to check a parameter, using check_expected(), against a
 * signed range.  Where range is minimum <= value <= maximum.
 * See will_return() for a description of the count parameter.
 */
#define expect_in_range(function, parameter, minimum, maximum) \
    expect_in_range_count(function, parameter, minimum, maximum, 1)
#define expect_in_range_count(function, parameter, minimum, maximum, count) \
    _expect_in_range(#function, #parameter, __FILE__, __LINE__, minimum, \
                     maximum, count)

/*
 * Add an event to check a parameter, using check_expected(), against a
 * signed range.  Where range is value < minimum or value > maximum.
 * See will_return() for a description of the count parameter.
 */
#define expect_not_in_range(function, parameter, minimum, maximum) \
    expect_not_in_range_count(function, parameter, minimum, maximum, 1)
#define expect_not_in_range_count(function, parameter, minimum, maximum, \
                                  count) \
    _expect_not_in_range(#function, #parameter, __FILE__, __LINE__, \
                         minimum, maximum, count)

/*
 * Add an event to check whether a parameter, using check_expected(), is or
 * isn't a value.  See will_return() for a description of the count parameter.
 */
#define expect_value(function, parameter, value) \
    expect_value_count(function, parameter, value, 1)
#define expect_value_count(function, parameter, value, count) \
    _expect_value(#function, #parameter, __FILE__, __LINE__, \
                  cast_to_largest_integral_type(value), count)
#define expect_not_value(function, parameter, value) \
    expect_not_value_count(function, parameter, value, 1)
#define expect_not_value_count(function, parameter, value, count) \
    _expect_not_value(#function, #parameter, __FILE__, __LINE__, \
                      cast_to_largest_integral_type(value), count)

/*
 * Add an event to check whether a parameter, using check_expected(),
 * is or isn't a string.  See will_return() for a description of the count
 * parameter.
 */
#define expect_string(function, parameter, string) \
    expect_string_count(function, parameter, string, 1)
#define expect_string_count(function, parameter, string, count) \
    _expect_string(#function, #parameter, __FILE__, __LINE__, \
                   (const char*)(string), count)
#define expect_not_string(function, parameter, string) \
    expect_not_string_count(function, parameter, string, 1)
#define expect_not_string_count(function, parameter, string, count) \
    _expect_not_string(#function, #parameter, __FILE__, __LINE__, \
                       (const char*)(string), count)

/*
 * Add an event to check whether a parameter, using check_expected() does or
 * doesn't match an area of memory.  See will_return() for a description of
 * the count parameter.
 */
#define expect_memory(function, parameter, memory, size) \
    expect_memory_count(function, parameter, memory, size, 1)
#define expect_memory_count(function, parameter, memory, size, count) \
    _expect_memory(#function, #parameter, __FILE__, __LINE__, \
                   (const void*)(memory), size, count)
#define expect_not_memory(function, parameter, memory, size) \
    expect_not_memory_count(function, parameter, memory, size, 1)
#define expect_not_memory_count(function, parameter, memory, size, count) \
    _expect_not_memory(#function, #parameter, __FILE__, __LINE__, \
                       (const void*)(memory), size, count)


/*
 * Add an event to allow any value for a parameter checked using
 * check_expected().  See will_return() for a description of the count
 * parameter.
 */
#define expect_any(function, parameter) \
    expect_any_count(function, parameter, 1)
#define expect_any_count(function, parameter, count) \
    _expect_any(#function, #parameter, __FILE__, __LINE__, count)

/*
 * Determine whether a function parameter is correct.  This ensures the next
 * value queued by one of the expect_*() macros matches the specified variable.
 */
#define check_expected(parameter) \
    _check_expected(__func__, #parameter, __FILE__, __LINE__, \
                    cast_to_largest_integral_type(parameter))

/* Assert that the given expression is true. */
#define assert_true(c) _assert_true(cast_to_largest_integral_type(c), #c, \
                                    __FILE__, __LINE__)
/* Assert that the given expression is false. */
#define assert_false(c) _assert_true(!(cast_to_largest_integral_type(c)), #c, \
                                     __FILE__, __LINE__)

/* Assert that the given pointer is non-NULL. */
#define assert_non_null(c) _assert_true(cast_ptr_to_largest_integral_type(c), #c, \
__FILE__, __LINE__)
/* Assert that the given pointer is NULL. */
#define assert_null(c) _assert_true(!(cast_ptr_to_largest_integral_type(c)), #c, \
__FILE__, __LINE__)

/* Assert that the two given integers are equal, otherwise fail. */
#define assert_int_equal(a, b) \
    _assert_int_equal(cast_to_largest_integral_type(a), \
                      cast_to_largest_integral_type(b), \
                      __FILE__, __LINE__)
/* Assert that the two given integers are not equal, otherwise fail. */
#define assert_int_not_equal(a, b) \
    _assert_int_not_equal(cast_to_largest_integral_type(a), \
                          cast_to_largest_integral_type(b), \
                          __FILE__, __LINE__)

/* Assert that the two given strings are equal, otherwise fail. */
#define assert_string_equal(a, b) \
    _assert_string_equal((const char*)(a), (const char*)(b), __FILE__, \
                         __LINE__)
/* Assert that the two given strings are not equal, otherwise fail. */
#define assert_string_not_equal(a, b) \
    _assert_string_not_equal((const char*)(a), (const char*)(b), __FILE__, \
                             __LINE__)

/* Assert that the two given areas of memory are equal, otherwise fail. */
#define assert_memory_equal(a, b, size) \
    _assert_memory_equal((const char*)(a), (const char*)(b), size, __FILE__, \
                         __LINE__)
/* Assert that the two given areas of memory are not equal, otherwise fail. */
#define assert_memory_not_equal(a, b, size) \
    _assert_memory_not_equal((const char*)(a), (const char*)(b), size, \
                             __FILE__, __LINE__)

/* Assert that the specified value is >= minimum and <= maximum. */
#define assert_in_range(value, minimum, maximum) \
    _assert_in_range( \
        cast_to_largest_integral_type(value), \
        cast_to_largest_integral_type(minimum), \
        cast_to_largest_integral_type(maximum), __FILE__, __LINE__)

/* Assert that the specified value is < minumum or > maximum */
#define assert_not_in_range(value, minimum, maximum) \
    _assert_not_in_range( \
        cast_to_largest_integral_type(value), \
        cast_to_largest_integral_type(minimum), \
        cast_to_largest_integral_type(maximum), __FILE__, __LINE__)

/* Assert that the specified value is within a set. */
#define assert_in_set(value, values, number_of_values) \
    _assert_in_set(value, values, number_of_values, __FILE__, __LINE__)
/* Assert that the specified value is not within a set. */
#define assert_not_in_set(value, values, number_of_values) \
    _assert_not_in_set(value, values, number_of_values, __FILE__, __LINE__)


/**
 * @defgroup cmocka_exec Running Tests
 * @ingroup cmocka
 *
 * This is the way tests are executed with CMocka.
 *
 * @{
 */

#ifdef DOXYGEN
/**
 * @brief Forces the test to fail immediately and quit.
 */
void fail(void);
#else
#define fail() _fail(__FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Generic method to run a single test.
 *
 * @param[in]  #function The function to test.
 *
 * @return 0 on success, 1 if an error occured.
 *
 * @code
 * // A test case that does nothing and succeeds.
 * void null_test_success(void **state) {
 * }
 *
 * int main(void) {
 *      return run_test(null_test_success);
 * }
 * @endcode
 */
int run_test(#function);
#else
#define run_test(f) _run_test(#f, f, NULL, UNIT_TEST_FUNCTION_TYPE_TEST, NULL)
#endif

/** Initializes a UnitTest structure. */
#define unit_test(f) { #f, f, UNIT_TEST_FUNCTION_TYPE_TEST }

/** Initializes a UnitTest structure with a setup function. */
#define unit_test_setup(test, setup) \
    { #test "_" #setup, setup, UNIT_TEST_FUNCTION_TYPE_SETUP }

/** Initializes a UnitTest structure with a teardown function. */
#define unit_test_teardown(test, teardown) \
    { #test "_" #teardown, teardown, UNIT_TEST_FUNCTION_TYPE_TEARDOWN }

/**
 * Initialize an array of UnitTest structures with a setup function for a test
 * and a teardown function.  Either setup or teardown can be NULL.
 */
#define unit_test_setup_teardown(test, setup, teardown) \
    unit_test_setup(test, setup), \
    unit_test(test), \
    unit_test_teardown(test, teardown)

/*
 * Run tests specified by an array of UnitTest structures.  The following
 * example illustrates this macro's use with the unit_test macro.
 *
 * void Test0();
 * void Test1();
 *
 * int main(int argc, char* argv[]) {
 *     const UnitTest tests[] = {
 *         unit_test(Test0);
 *         unit_test(Test1);
 *     };
 *     return run_tests(tests);
 * }
 */
#ifdef DOXYGEN
/**
 * @brief Run tests specified by an array of UnitTest structures.
 *
 * @param[in]  tests[] The array of unit tests to execute.
 *
 * @return 0 on success, 1 if an error occured.
 *
 * @code
 * static void setup(void **state) {
 *      int *answer = malloc(sizeof(int));
 *      assert_non_null(answer);
 *
 *      *answer = 42;
 *
 *      *state = answer;
 * }
 *
 * static void teardown(void **state) {
 *      free(*state);
 * }
 *
 * static void null_test_success(void **state) {
 *     (void) state;
 * }
 *
 * static void int_test_success(void **state) {
 *      int *answer = *state;
 *      assert_int_equal(*answer, 42);
 * }
 *
 * int main(void) {
 *     const UnitTest tests[] = {
 *         unit_test(null_test_success),
 *         unit_test_setup_teardown(int_test_success, setup, teardown),
 *     };
 *
 *     return run_tests(tests);
 * }
 * @endcode
 *
 * @see unit_test
 * @see unit_test_setup
 * @see unit_test_teardown
 * @see unit_test_setup_teardown
 */
int run_tests(tests[]);
#else
#define run_tests(tests) _run_tests(tests, sizeof(tests) / sizeof(tests)[0])
#endif

/** @} */

/* Dynamic allocators */
#define test_malloc(size) _test_malloc(size, __FILE__, __LINE__)
#define test_calloc(num, size) _test_calloc(num, size, __FILE__, __LINE__)
#define test_free(ptr) _test_free(ptr, __FILE__, __LINE__)

/* Redirect malloc, calloc and free to the unit test allocators. */
#if UNIT_TESTING
#define malloc test_malloc
#define calloc test_calloc
#define free test_free
#endif /* UNIT_TESTING */

/*
 * Ensure mock_assert() is called.  If mock_assert() is called the assert
 * expression string is returned.
 * For example:
 *
 * #define assert mock_assert
 *
 * void showmessage(const char *message) {
 *   assert(message);
 * }
 *
 * int main(int argc, const char* argv[]) {
 *   expect_assert_failure(show_message(NULL));
 *   printf("succeeded\n");
 *   return 0;
 * }
 */
#define expect_assert_failure(function_call) \
  { \
    const int result = setjmp(global_expect_assert_env); \
    global_expecting_assert = 1; \
    if (result) { \
      print_message("Expected assertion %s occurred\n", \
                    global_last_failed_assert); \
      global_expecting_assert = 0; \
    } else { \
      function_call ; \
      global_expecting_assert = 0; \
      print_error("Expected assert in %s\n", #function_call); \
      _fail(__FILE__, __LINE__); \
    } \
  }

/* Function prototype for setup, test and teardown functions. */
typedef void (*UnitTestFunction)(void **state);

/* Function that determines whether a function parameter value is correct. */
typedef int (*CheckParameterValue)(const LargestIntegralType value,
                                   const LargestIntegralType check_value_data);

/* Type of the unit test function. */
typedef enum UnitTestFunctionType {
    UNIT_TEST_FUNCTION_TYPE_TEST = 0,
    UNIT_TEST_FUNCTION_TYPE_SETUP,
    UNIT_TEST_FUNCTION_TYPE_TEARDOWN,
} UnitTestFunctionType;

/*
 * Stores a unit test function with its name and type.
 * NOTE: Every setup function must be paired with a teardown function.  It's
 * possible to specify NULL function pointers.
 */
typedef struct UnitTest {
    const char* name;
    UnitTestFunction function;
    UnitTestFunctionType function_type;
} UnitTest;


/* Location within some source code. */
typedef struct SourceLocation {
    const char* file;
    int line;
} SourceLocation;

/* Event that's called to check a parameter value. */
typedef struct CheckParameterEvent {
    SourceLocation location;
    const char *parameter_name;
    CheckParameterValue check_value;
    LargestIntegralType check_value_data;
} CheckParameterEvent;

/* Used by expect_assert_failure() and mock_assert(). */
extern int global_expecting_assert;
extern jmp_buf global_expect_assert_env;
extern const char * global_last_failed_assert;

/* Retrieves a value for the given function, as set by "will_return". */
LargestIntegralType _mock(const char * const function, const char* const file,
                          const int line);

void _expect_check(
    const char* const function, const char* const parameter,
    const char* const file, const int line,
    const CheckParameterValue check_function,
    const LargestIntegralType check_data, CheckParameterEvent * const event,
    const int count);

void _expect_in_set(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const LargestIntegralType values[],
    const size_t number_of_values, const int count);
void _expect_not_in_set(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const LargestIntegralType values[],
    const size_t number_of_values, const int count);

void _expect_in_range(
    const char* const function, const char* const parameter,
    const char* const file, const int line,
    const LargestIntegralType minimum,
    const LargestIntegralType maximum, const int count);
void _expect_not_in_range(
    const char* const function, const char* const parameter,
    const char* const file, const int line,
    const LargestIntegralType minimum,
    const LargestIntegralType maximum, const int count);

void _expect_value(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const LargestIntegralType value,
    const int count);
void _expect_not_value(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const LargestIntegralType value,
    const int count);

void _expect_string(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const char* string,
    const int count);
void _expect_not_string(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const char* string,
    const int count);

void _expect_memory(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const void* const memory,
    const size_t size, const int count);
void _expect_not_memory(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const void* const memory,
    const size_t size, const int count);

void _expect_any(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const int count);

void _check_expected(
    const char * const function_name, const char * const parameter_name,
    const char* file, const int line, const LargestIntegralType value);

/*
 * Can be used to replace assert in tested code so that in conjuction with
 * check_assert() it's possible to determine whether an assert condition has
 * failed without stopping a test.
 */
void mock_assert(const int result, const char* const expression,
                 const char * const file, const int line);

void _will_return(const char * const function_name, const char * const file,
                  const int line, const LargestIntegralType value,
                  const int count);
void _assert_true(const LargestIntegralType result,
                  const char* const expression,
                  const char * const file, const int line);
void _assert_int_equal(
    const LargestIntegralType a, const LargestIntegralType b,
    const char * const file, const int line);
void _assert_int_not_equal(
    const LargestIntegralType a, const LargestIntegralType b,
    const char * const file, const int line);
void _assert_string_equal(const char * const a, const char * const b,
                          const char * const file, const int line);
void _assert_string_not_equal(const char * const a, const char * const b,
                              const char *file, const int line);
void _assert_memory_equal(const void * const a, const void * const b,
                          const size_t size, const char* const file,
                          const int line);
void _assert_memory_not_equal(const void * const a, const void * const b,
                              const size_t size, const char* const file,
                              const int line);
void _assert_in_range(
    const LargestIntegralType value, const LargestIntegralType minimum,
    const LargestIntegralType maximum, const char* const file, const int line);
void _assert_not_in_range(
    const LargestIntegralType value, const LargestIntegralType minimum,
    const LargestIntegralType maximum, const char* const file, const int line);
void _assert_in_set(
    const LargestIntegralType value, const LargestIntegralType values[],
    const size_t number_of_values, const char* const file, const int line);
void _assert_not_in_set(
    const LargestIntegralType value, const LargestIntegralType values[],
    const size_t number_of_values, const char* const file, const int line);

void* _test_malloc(const size_t size, const char* file, const int line);
void* _test_calloc(const size_t number_of_elements, const size_t size,
                   const char* file, const int line);
void _test_free(void* const ptr, const char* file, const int line);

void _fail(const char * const file, const int line);
int _run_test(
    const char * const function_name, const UnitTestFunction Function,
    void ** const volatile state, const UnitTestFunctionType function_type,
    const void* const heap_check_point);
int _run_tests(const UnitTest * const tests, const size_t number_of_tests);

/* Standard output and error print methods. */
void print_message(const char* const format, ...) PRINTF_ATTRIBUTE(1, 2);
void print_error(const char* const format, ...) PRINTF_ATTRIBUTE(1, 2);
void vprint_message(const char* const format, va_list args) PRINTF_ATTRIBUTE(1, 0);
void vprint_error(const char* const format, va_list args) PRINTF_ATTRIBUTE(1, 0);

/** @} */

#endif /* CMOCKA_H_ */
// 1). This implementation of static reflection is meant to provide ease
// in iteration over *static fields of a Structure that will be initialized
// from a json config file. (For non-static members there will be a different
// static_assert on the pointer type std::false_type vs std::true_type)
//
// WANR: New variables have to declared between FIRST_LINE and LAST_LINE and
// not span on more than single line. (no comments in between the lines)
//
// Usage example:
//
// Struct SampleStruct {
//    static const int FIRST_LINE = __LINE__;
//    static bool my_bool;
//    static int my_int;
//    static const int LAST_LINE = __LINE__;
//
//    static const int PARAMETERS_COUNT = LAST_LINE - FIRST_LINE -1;
// };
//
// bool SampleStruct::my_bool = true;
// int SampleStruct::my_int = 123;
//
// 2). The PARAMETERS_COUNT field must be present for static_assert in the
// DEFINE_STRUCT_SCHEMA() macro that assures the correct number of parameters
// is reflected by that macro
//
// 3). The StructSchema has to be defined with the macros:
// DEFINE_STRUCT_SCHEMA(SampleStruct,
//                      DEFINE_STRUCT_FIELD(my_bool, "my_bool", json_type);
//                      DEFINE_STRUCT_FIELD(my_int, "my_int", json_type));
//
// 4). Once the Schema is defined it can be accessed with the for_each_field()
// and lambda expression:
// for_each_field(SampleStruct{}, [](auto&& field, auto&& name, auto json_type)
//                { std::cout << name << " = " << field << " : " << json_type
//                            << std::endl;
//                });

#pragma once

#include <tuple>
#include <type_traits>

// Enum representing consecutive elements of the StructSchema single tuple
enum StructSchemaElem {
  FIELD = 0,
  NAME = 1,
  JSON_TYPE = 2,
};

// The for_each_tuple implementation that uses compiles-time recursion and
// indices to provide iteration over array of tuple elements with enforced
// left-to-right traverse order
namespace detail {

template <typename Fn, typename Tuple, std::size_t... Idx>
inline constexpr void for_each_tuple(Tuple&& tuple, Fn&& fn,
                                     std::index_sequence<Idx...>) {
  using Expander = int[];
  (void)Expander{0,
                 ((void)fn(std::get<Idx>(std::forward<Tuple>(tuple))), 0)...};
}

template <typename Fn, typename Tuple>
inline constexpr void for_each_tuple(Tuple&& tuple, Fn&& fn) {
  for_each_tuple(
      std::forward<Tuple>(tuple), std::forward<Fn>(fn),
      std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
}

}  // namespace detail

// This function template with explicit template arguments has to be declared
// for the SructSchema() function to be used in the DEFINE_STRUCT_SCHEMA macro
// or the for_each_field() function
//
// StructureSchema() returns all Structure field information or by default
// an empty tuple
template <typename T>
inline constexpr auto StructSchema() {
  return std::make_tuple();
}

// Macro for defining all fields of desired Struct in form of array of tuples:
// (struct_field, field_name, json_type)
//
// The using _Struct here provides a method of a data relay within macro,
// so that the next macro can obtain the data of the previous macro
//
// Assertion makes sure that all of the parameters of the Struct are reflected
//
// The returned tuple is:
// ((& field1, name1, json_type1), (& field2, name2, json_type2), ...)
#define DEFINE_STRUCT_SCHEMA(Struct, ...)                                   \
  template <>                                                               \
  inline constexpr auto StructSchema<Struct>() {                            \
    using _Struct = Struct;                                                 \
                                                                            \
    static_assert(                                                          \
        Struct::PARAMETERS_COUNT ==                                         \
            std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value, \
        "The number of declared parameters in the struct "                  \
        "differs from defined reflective schema.");                         \
                                                                            \
    return std::make_tuple(__VA_ARGS__);                                    \
  }

// Macro for defining each field of the desired Struct
#define DEFINE_STRUCT_FIELD(struct_field, json_type) \
  std::make_tuple(&_Struct::struct_field, #struct_field, json_type)

// Template for a constexp for_each_field() function that will iterate over the
// fields of the type T Struct with the previously defined StructSchema
//
// This function takes the corresponding StructSchema<StructType> record of all
// the fields information and then traverses the elements of the tuple to get
// the location, name, json_type of each field and call the conversion function
// as a parameter fn
template <typename T, typename Fn>
inline constexpr void for_each_field(T&&, Fn&& fn) {
  // Computed at compile time Struct schema used for
  constexpr auto struct_schema = StructSchema<std::decay_t<T>>();

  // For each tuple use the static assertion to examine whether all the field
  // information is defined
  static_assert(
      std::tuple_size<decltype(struct_schema)>::value != 0,
      "StructSchema<T>() for type T should be specialized to return "
      "FieldSchema tuples, like: (*ptr, field_name, json_type), ...).");

  detail::for_each_tuple(struct_schema, [&fn](auto&& field_schema) {
    using FieldSchema = std::decay_t<decltype(field_schema)>;
    // Assertion makes sure that all the tuples consist of three fields
    static_assert(std::tuple_size<FieldSchema>::value == 3,
                  "FieldSchema tuple should be a tuple of three elements.");

    // Call the conversion function on each tuple
    fn(*std::get<FIELD>(std::forward<decltype(field_schema)>(field_schema)),
       std::get<NAME>(std::forward<decltype(field_schema)>(field_schema)),
       std::get<JSON_TYPE>(std::forward<decltype(field_schema)>(field_schema)));
  });
}

# String maker
![Build](https://github.com/balashovAD/stringMaker/actions/workflows/BuildLinux.yml/badge.svg)

## Links
- [Idea](#Idea)
- [Examples](#Examples)
- [Installation](#Installation)
- [Built-in primitives](#Built-in)
- [Benchmarks](#benchmarks)

## Idea
This is zero-cost abstraction library for generating `std::string_view` with `std::format` like API,
but without memory allocation and without copying compile time known strings. 

This library good for hot points in code than you need to make a `string` like object for short usage. 
Like send network message or write it in shared memory or, in not so fast cases, to hard disk.

You should know all maximum size for each placeholder in the generation pattern, or use default calculated, like for numbers format.
Because return `std::string_view` uses the common memory, you should control overuse manually.

The core idea is to calculate maximum possible size of result string and pre-allocate memory for this,
after, prepare the buffer by filling constexpr string. In active phase use only in place convert to string and `memcpy`.

### Good for usage it if
- You need really high performance
- You need only `std::string_view` or can spend time for one creation `std::string`
- Don't afraid long compile time, because of a lot of metaprogramming

For `Default`, `PreInit` or `PreInitOnly` configs:
- Output format has kind of 'neutral' symbol, like space in Json or Xml.
- A lot of static (constexpr known) parts of result, compare to runtime values. Especially for `PreInitOnlyConfig`.

For `Dynamic`:
- Want to have good formatted text, like in `format` output.
- Any others case, universal solution, but can be slightly slower in theoretical situation

## Examples

### Hello world
```cpp
std::string name = "Paul";
std::string longName = "PaulPaulPaulPaul";

using HelloName = mkr::MakeAggregator<"Hello {}!", mkr::RuntimeStr<10>>; // Name should have less than 11 symbols
mkr::Maker<HelloName> maker; // Allocate and fill buffer `Hello ~~~~~~~~~~!` where `~` is place for name
auto sv = maker.generate(name); // sv == `Hello Paul      !`
auto sv2 = maker.generate(name); // Name cannot be placed, sv2 == ``, sv is in unknown state

// Dynamic located
mkr::Maker<HelloName, LocalStorage, DynamicConfig> maker; // Generate string without neutral symbols inside
auto sv = maker.generate(name); // sv == `Hello Paul!`
```
### Json
```cpp
using namespace mkr;
using Json = MakeAggregator<R"({"small_int_field": {}, "string_field": {}, "obj_fields": {"custom": {}, "static": 5}})"_str, 
        RuntimeIntegral<int, 3>, QuotedStr<RuntimeStr<10>>, QuotedStr<CustomType<MyTime>>>;

Maker<Json> maker;
// ...
maker.generate(5, "text", MyTime{}); // {"int_field": 5  , "string_field": "text"      , "obj_fields": {"custom": "03/2024", "static": 5}}

```
## Installation
This header only library requires a **c++20** compiler, requires Clang-17, MinGW13 or GCC-13 compiler.  
> gcc-13.1.0 has internal bug: `internal compiler error: in is_base_type, at dwarf2out.cc:13417`

![Clang-17](https://github.com/balashovAD/stringMaker/actions/workflows/CompilerCheckClang17.yml/badge.svg)
![GCC-13](https://github.com/balashovAD/stringMaker/actions/workflows/CompilerCheckGCC13.yml/badge.svg)

You can include the library as a git submodule in your project and add
`target_include_directories(prj PRIVATE ${PARSECPP_INCLUDE_DIR})` to your `CMakeLists.txt` file.

Alternatively, you can copy & paste the `single_include` headers to your project's include path.
Use `all.hpp` as the base parser header, while `fwd.hpp` (1000 lines) contains only general definition without implementations.

## Built-in
### Pattern
##### `RuntimeStr`
> Param: maxSize  
> Arg s: string_view | s.size() <= maxSize 
##### `StaticStr`
> Param: ConstexprStr  
> No arg
##### `RuntimeIntegral`
> Param: Type | is_integral  
> Param: size_t maxSize = max size for Type | use Type value if 0  
> Arg n: implicit convertible to Type | std::to_string(n).size() < maxSize
##### `RuntimeFloat`
> Param: Type | is_floating_point  
> Param: size_t maxSize = max size for Type | use Type value if 0  
> Arg n: implicit convertible to Type | std::to_string(n).size() < maxSize
##### `QuotedStr`
> Param Pattern | IsPattern  
> Param char quote = "  
> Arg: same as Pattern
##### `DynamicLocated`
> Param Pattern | IsPattern  
> Arg: same as Pattern
##### `CustomType`
> Param: Type | has mkr_generate(char*, char const*, Type const&) in ADL  
> Param: size_t maxSize = 0 | use Type value if 0
##### `Apply`
> Param: Pattern | IsPattern  
> Param: auto fn | fn(Arg) is suitable for Pattern  
> Arg fnArg 
### Storage
##### `LocalStorage`
`Memory<MaxSize>`
##### `RingBufferStorage`
> Param: size_t ringSize

`std::array<Memory<maxSize>, ringSize>`
### Config
##### `DefaultConfig`
##### `DynamicConfig`
##### `PreInitConfig`
##### `PreInitOnlyConfig`

## Benchmarks
- `BM_CDR`: CSV format with a lot of integer and string fields, better use with `DynamicConfig`. Heavy for linux `timestamp` to local time call.
- `BM_OKX`: Json format, good for `Default` or `PreInitOnly`
- `BM_BNN`: Http request line format, should use with `DynamicConfig`
Lower values is better.

| *                 | BM_CDR     | BM_OKX     | BM_BNN     | Slowdown (Avg) |
|-------------------|------------|------------|------------|----------------|
| Default           | 599.888us  | 92.072us   | 130.615us  | 1x             |
| Dynamic           | 530.134us  | 94.164us   | 102.539us  | 0.98x          |
| RingPreInitOnly   | 530.134us  | 88.933us   | 108.817us  | 0.99x          |
| Format            | 5580.36us  | 669.643us  | 655.692us  | 10.43x         |
| Stream            | 8159.72us  | 2949.3us   | 3928.07us  | 20.25x         |
| Perfect           | 531.25us   | 87.193us   | 107.422us  | 0.99x          |


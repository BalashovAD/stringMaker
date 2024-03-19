# String maker
![Build](https://github.com/balashovAD/stringMaker/actions/workflows/BuildLinux.yml/badge.svg)
- [Design goals](#Design-goals)
- [Idea](#Idea)
- [Examples](#Examples)
- [Installation](#Installation)
- [Built-in primitives](#Built-in-primitives)
- [Benchmarks](#benchmarks)

## Design goals
Create a faster and more performance-focused library for generating `string`-like object with `format`-like interface,
aimed at providing top performance in hot spots.
### Highlights
- No memory allocation
- Concepts and compile time checks
- Header only for straightforward integration
- Zero abstraction code, optimized for hot path
- Simplifies adding custom storage and serialization functions (See [CustomType](#customtype))
- No exception and no runtime type information necessary (can be built with `-fno-exceptions -fno-rtti`)

## Idea
String Maker is a zero-cost abstraction library designed for generating `std::string_view` with a `std::format`-like API, 
without memory allocation and copying of compile-time known strings. 
This library is especially useful for critical points in code where you need to create a temporary `string`-like object, 
such as sending a network message, writing to shared memory, or even, in less time-sensitive scenarios, writing to a disk. 

Critical design involves knowing the maximum size for placeholders at compile time, manually specifying it, 
or leveraging default size estimations for numerical patterns.
Since `std::string_view` utilizing shared memory from `Maker`, it's essential to manage its lifecycle carefully. 
The library pre-calculates the maximum result string size, pre-allocates memory, and initializes the buffer with constexpr strings, ensuring maximum efficiency.

### Ideal Use Cases
- You prioritize high performance.
- You require `std::string_view` or can spend time for creating one `std::string`.
- You are comfortable with longer compile times due to extensive metaprogramming.

For `Default`, `PreInit` or `PreInitOnly` configs:
- The output format requires a 'neutral' symbol, like spaces in JSON or XML.
- There are many static (constexpr known) parts of the result compared to runtime values, especially for `PreInitOnlyConfig`.

For `Dynamic` configs:
- You seek well-formatted text output similar to `format`.
- You're looking for a universal solution, albeit potentially slightly slower in some cases.

## Examples

### Hello world
```cpp
std::string name = "Paul";
std::string longName = "PaulPaulPaulPaul";

using HelloName = mkr::MakeAggregator<"Hello {}!"_str, mkr::RuntimeStr<10>>; 
// Name should be less than 11 symbols (bytes)
mkr::Maker<HelloName> maker; 
// Allocate and fill buffer `Hello ~~~~~~~~~~!` where `~` is place for the name
auto sv = maker.generate(name); 
// sv == `Hello Paul      !`
auto sv2 = maker.generate(name); 
// Name cannot be placed, sv2 == ``, sv is in unknown state

// Dynamic located
mkr::Maker<HelloName, LocalStorage, DynamicConfig> maker; 
// Generate string without neutral symbols inside
auto sv = maker.generate(name); 
// sv == `Hello Paul!`
```
### Json
```cpp
using namespace mkr;
using Json = MakeAggregator<R"({"small_int_field": {}, "string_field": {}, "obj_fields": {"custom": {}, "static": 5}})"_str, 
        RuntimeIntegral<int, 3>, QuotedStr<RuntimeStr<10>>, QuotedStr<CustomType<MyTime>>>;

Maker<Json> maker;
// ...
maker.generate(5, "text", MyTime{}); 
// {"int_field": 5  , "string_field": "text"      , "obj_fields": {"custom": "03/2024", "static": 5}}

```
## Installation
This header only library requires a **c++20** compiler, requires Clang-17, MinGW13 or GCC-13 compiler.  

![Clang-17](https://github.com/balashovAD/stringMaker/actions/workflows/CompilerCheckClang17.yml/badge.svg)
![GCC-13](https://github.com/balashovAD/stringMaker/actions/workflows/CompilerCheckGCC13.yml/badge.svg)

You can include the library as a git submodule in your project and add
`target_include_directories(prj PRIVATE ${STRMAKER_INCLUDE_DIR})` to your `CMakeLists.txt` file.

Alternatively, you can copy & paste the `single_include` headers to your project's include path.
Use `all.hpp` as the base header, while `fwd.hpp` (1000 lines) contains only general definition without implementations.

## Built-in primitives
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
> Arg fn Arg 

### Storage
##### `LocalStorage`
> Uses `Memory<MaxSize>`

Local storing one buffer for `string_view`
##### `RingBufferStorage`
> Param: size_t ringSize

> `std::array<Memory<maxSize>, ringSize>`

Local storing `ringSize` buffers for `string_view`
### Config
##### `DefaultConfig`
##### `DynamicConfig`
##### `PreInitConfig`
##### `PreInitOnlyConfig`

## Benchmarks
- `BM_CDR`: CSV format, with numerous unsigned integer and string fields. 
Best result formatting observed using `DynamicConfig`.   
_Note_: Particularly resource-intensive for converting Linux `timestamp` to local time.
- `BM_OKX`: Json format, showing optimal results with either `DefaultConfig` or `PreInitOnlyConfig`
- `BM_BNN`: Http request line format, should be used with `DynamicConfig`

In these benchmarks, lower values indicate better performance, reflecting the library's effectiveness in processing and converting data with minimal resource utilization.

> Windows, MinGW

| *                 | BM_CDR     | BM_OKX     | BM_BNN     | Slowdown   |
|-------------------|------------|------------|------------|------------|
| Default           | 613.839us  | 96.257us   | 119.978us  | 1.09x      |
| Dynamic           | 562.5us    | 94.164us   | 107.422us  | 1.01x      |
| RingDynamic       | 571.987us  | 100.442us  | 111.607us  | 1.05x      |
| RingPreInitOnly   | 558.036us  | 92.425us   | 109.863us  | 1.01x      |
| Perfect           | 544.085us  | 89.979us   | 112.305us  | 1x         |
| Format            | 5580.36us  | 680.106us  | 669.643us  | 7.93x      |
| Stream            | 7986.11us  | 3012.05us  | 3997.09us  | 27.9x      |



> Linux, GCC-13.2.0

| *                 | BM_CDR     | BM_OKX     | BM_BNN     | Slowdown   |
|-------------------|------------|------------|------------|------------|
| Default           | 2217.18us  | 88.751us   | 120.848us  | 1.05x      |
| Dynamic           | 2183.88us  | 92.856us   | 122.258us  | 1.06x      |
| RingDynamic       | 2186.04us  | 90.572us   | 106.114us  | 1.01x      |
| RingPreInitOnly   | 2179.9us   | 92.251us   | 105.447us  | 1.01x      |
| Perfect           | 2170.39us  | 84.928us   | 112.056us  | 1x         |
| Format            | 3630.24us  | 402.263us  | 417.427us  | 3.38x      |
| Stream            | 3095.51us  | 830.098us  | 865.985us  | 6.31x      |


> Linux, Clang-17

| *                 | BM_CDR     | BM_OKX     | BM_BNN     | Slowdown   |
|-------------------|------------|------------|------------|------------|
| Default           | 2199.52us  | 89.157us   | 109.417us  | 1.05x      |
| Dynamic           | 2161.29us  | 92.928us   | 116.354us  | 1.08x      |
| RingDynamic       | 2152.45us  | 93.295us   | 105.601us  | 1.04x      |
| RingPreInitOnly   | 2152.79us  | 87.87us    | 103.922us  | 1.02x      |
| Perfect           | 2133.58us  | 85.143us   | 103.413us  | 1x         |
| Format            | 3577.11us  | 371.23us   | 382.121us  | 3.24x      |
| Stream            | 3059.17us  | 790.37us   | 830.772us  | 6.25x      |

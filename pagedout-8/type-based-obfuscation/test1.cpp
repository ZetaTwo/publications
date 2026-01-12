#include <iostream>
#include <string>

// #define OBFUSCATE __attribute__((annotate("metadata_secret")))

template <typename T>
constexpr std::string_view TypeName();

template <>
constexpr std::string_view TypeName<int>()
{
    return "int";
}

template <int A, int B>
[[clang::noinline]]
int add([[clang::annotate("obfuscate", A)]] int a, [[clang::annotate("obfuscate", B)]] int b)
{
    if (A > B)
    {
        constexpr int C = A / B;
        return C * (a / C + b);
    }
    else
    {
        constexpr int C = B / A;
        return C * (a + b / C);
    }
}

template <int A>
int get_input()
{
    int i;
    std::cin >> i;
    return i * A;
}

int main()
{
    const int maskA = 8, maskB = 16, maskC = 32;
    int a = get_input<maskA>();
    int b = get_input<maskB>();
    int c = get_input<maskC>();

    std::cout << add<maskA, maskB>(a, b) / maskB << std::endl;
    std::cout << add<maskB, maskC>(b, c) / maskC << std::endl;

    return 0;
}

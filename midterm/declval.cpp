#include <iostream>
using namespace std;
template <typename T, typename not_used = decltype(*declval<T>())>
false_type are_you_pointer(T) {
    return false_type{};
}
template <typename T>
true_type are_you_pointer(...) {
    return true_type{};
}

template <typename T>
struct AA : public decltype(are_you_pointer<T>(declval<T>())) {};

int main() {
    cout << AA<int>{};
}



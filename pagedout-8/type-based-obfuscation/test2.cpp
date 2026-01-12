
int f([[clang::annotate("x", 1)]] int a) {
    return a;
}

int main() {
    f(1);
}

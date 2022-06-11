export $(cat .env | xargs)
# -g -O0 for debugging with gdb
{
    clang++ -std=c++17 src/*.cpp -o bot -ldpp -Wall
} && {
    ./bot
}
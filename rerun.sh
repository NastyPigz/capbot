export $(cat .env | xargs)
cd build

# this line below is for macOS
alias nproc='sysctl -n hw.logicalcpu'

{
    cmake ..
    make -j$(nproc)
} && {
    ./capbot
}
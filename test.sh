g++ @opcje prz.cpp -o wzo
g++ -std=c++23 brute.cpp -o brute
g++ -std=c++23 gen.cpp -o gen

first=1
last=20000

dir=prz_test/big
SAVE_TESTS=true

if $SAVE_TESTS; then
    echo "Saving tests into $dir"
fi

for((i=$first;i<=$last;i++))
do
    echo "test $i"
    ./gen > test.in
    MallocNanoZone=0 ./wzo < test.in > test.wzo
    ./brute < test.in > test.brute
    diff -b test.wzo test.brute || break
    echo "OK"

    if $SAVE_TESTS; then
        cp test.in $dir/$i.in
        cp test.wzo $dir/$i.out
        echo "saved test $i"
    fi
    echo ""
done
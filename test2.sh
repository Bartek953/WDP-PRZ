g++ @opcje prz.cpp -o wzo
g++ -std=c++23 brute_log.cpp -o brute_log
g++ -std=c++23 gen.cpp -o gen

first=0
last=10000

dir=prz_test/large
SAVE_TESTS=false

echo "TEST 2 - testing with brute_log"

if $SAVE_TESTS; then
    echo "Saving tests into $dir"
fi

for((i=$first;i<=$last;i++))
do
    echo "test $i"
    ./gen > test.in
    head -n 1 test.in
    time MallocNanoZone=0 ./wzo < test.in > test.wzo
    time ./brute_log < test.in > test.brute_log
    diff -b test.wzo test.brute_log || break
    echo "OK"

    if $SAVE_TESTS; then
        cp test.in $dir/$i.in
        cp test.wzo $dir/$i.out
        echo "saved test $i"
    fi
    echo ""
done
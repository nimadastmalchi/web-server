PATH_TO_BIN="../build/bin/server"
PATH_TO_CONFIG="../src/server_config"
DELAY=1 # How long to wait for each curl request
EXPOSED_PORT=80

# Store host IP
host=$(hostname -I)

# Remove trailing spaces
host=`echo $host | sed -e 's/^[[:space:]]*//'`

exit_status=0

# Run server in background
$PATH_TO_BIN $PATH_TO_CONFIG &
server_pid=$!
echo $server_pid

sleep 5

################# Test 1 #################
actual_output=$(curl --header "test:test" $host:$EXPOSED_PORT/echo --max-time $DELAY)
expected_output=$'GET /echo HTTP/1.1\r\nHost: '"$host"$'\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\ntest:test\r\n\r'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 1 passed"
else
    echo "Integration Test 1 failed"
    exit_status=1
fi

################# Test 2 #################
actual_output=$(curl --header "Accept: test" $host:$EXPOSED_PORT/echo --max-time $DELAY)
expected_output=$'GET /echo HTTP/1.1\r\nHost: '"$host"$'\r\nUser-Agent: curl/7.81.0\r\nAccept: test\r\n\r'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 2 passed"
else
    echo "Integration Test 2 failed"
    exit_status=1
fi

################# Test 3 #################
actual_output=$(curl --header "" $host:$EXPOSED_PORT/echo --max-time $DELAY)
echo $actual_output
expected_output=$'GET /echo HTTP/1.1\r\nHost: '"$host"$'\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\n\r'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 3 passed"
else
    echo "Integration Test 3 failed"
    exit_status=1
fi

mkdir -p img
touch img/file.txt
echo "hello world" > img/file.txt

################# Test 4 #################
actual_output=$(curl --header "" $host:$EXPOSED_PORT/static/all/img/file.txt --output -)
expected_output="hello world"
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 4 passed"
else
    echo "Integration Test 4 failed"
    exit_status=1
fi

################# Test 5 #################
actual_output=$(curl --header "" $host:$EXPOSED_PORT/static/image/file.txt --output -)
expected_output="hello world"
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 5 passed"
else
    echo "Integration Test 5 failed"
    exit_status=1
fi

head -c 200000 /dev/urandom > img/200thousandbytes.txt
head -c 100 /dev/urandom > img/100bytes.txt

################# Test 6 #################
actual_output=$(curl --header "" $host:$EXPOSED_PORT/static/image/200thousandbytes.txt --output -)
expected_output="$(cat img/200thousandbytes.txt)"
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 6 passed"
else
    echo "Integration Test 6 failed"
    exit_status=1
fi

################# Test 7 #################
actual_output=$(curl --header "" $host:$EXPOSED_PORT/static/image/100bytes.txt --output -)
expected_output="$(cat img/100bytes.txt)"
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 7 passed"
else
    echo "Integration Test 7 failed"
    exit_status=1
fi

rm img/file.txt img/200thousandbytes.txt img/100bytes.txt

################# Test 8 #################
actual_output=$(curl --header "" $host:$EXPOSED_PORT/does_not_exist --output -)
expected_output=$'<html><head><title>404 Not Found</title></head><body><p>404 Not Found</p></body></html>\r'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 8 passed"
else
    echo "Integration Test 8 failed"
    exit_status=1
fi

# Kill server
kill -9 $server_pid

exit $exit_status

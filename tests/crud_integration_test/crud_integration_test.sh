#!/bin/bash

../build/bin/server ../tests/crud_integration_test/crud.conf &
mkdir test_entities
#server info
SERVER_PID=$!
SERVER_IP=localhost
SERVER_PORT=800
TIMEOUT=0.5

##### Test 1 #####
##### Creating entity
echo -e "POST /api/shoes HTTP/1.1\r\n\r\n" | timeout $TIMEOUT nc $SERVER_IP $SERVER_PORT > temp_integration_pass_test1
diff temp_integration_pass_test1 integration_pass_test1
RESULT=$?

if [ $RESULT -eq 0 ];
then
    echo "Test 1 Success"
else
    echo "Test 1 Fail"
    exit_status=1
fi

##### Test 2 #####
##### Creating entity

echo -e "POST /api/shoes HTTP/1.1\r\n\r\n" | timeout $TIMEOUT nc $SERVER_IP $SERVER_PORT > temp_integration_pass_test2
diff temp_integration_pass_test2 integration_pass_test2
RESULT=$?

if [ $RESULT -eq 0 ];
then
    echo "Test 2 Success"
else
    echo "Test 2 Fail"
    exit_status=1
fi

##### kill server and restart server
kill -9 $SERVER_PID

../build/bin/server ../tests/crud_integration_test/crud.conf &
SERVER_PID=$!

sleep 1

##### Test 3 #####
##### Retrieving entity while checking persistency

echo -e "GET /api/shoes/1 HTTP/1.1\r\n\r\n" | timeout $TIMEOUT nc $SERVER_IP $SERVER_PORT > temp_integration_pass_test3
diff temp_integration_pass_test3 integration_pass_test3
RESULT=$?

if [ $RESULT -eq 0 ];
then
    echo "Test 3 Success"
else
    echo "Test 3 Fail"
    exit_status=1
fi


##### Test 4 #####
##### Deleting entity

echo -e "DELETE /api/shoes/1 HTTP/1.1\r\n\r\n" | timeout $TIMEOUT nc $SERVER_IP $SERVER_PORT > temp_integration_pass_test4
diff temp_integration_pass_test4 integration_pass_test4
RESULT=$?

if [ $RESULT -eq 0 ];
then
    echo "Test 4 Success"
else
    echo "Test 4 Fail"
    exit_status=1
fi

##### Test 5 #####
##### Verifying entity that is no longer retrievable
echo -e "GET /api/shoes/1 HTTP/1.1\r\n\r\n" | timeout $TIMEOUT nc $SERVER_IP $SERVER_PORT > temp_integration_pass_test5
diff temp_integration_pass_test5 integration_pass_test5
RESULT=$?

if [ $RESULT -eq 0 ];
then
    echo "Test 5 Success"
else
    echo "Test 5 Fail"
    exit_status=1
fi


rm -rf test_entities
rm temp_integration_pass_test1
rm temp_integration_pass_test2
rm temp_integration_pass_test3
rm temp_integration_pass_test4
rm temp_integration_pass_test5

kill -9 $SERVER_PID
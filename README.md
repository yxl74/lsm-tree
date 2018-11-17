# lsm-tree
Simple LSM Tree

Implementation
In order to implement the LSM merge tree in a short time, we decided to use simple array as the data structure behind C0 and C1. When C0 is full, we sort C0 and merge it with existing C1 and then write the merging result into another file. After this write operation is finished, we will write to a file which stored one bit to demonstrate which file is consistent. So in the event of crash, we will only lose new operations stored in C0. In order to avoid unnecessary searching, we merge the update and insert function.

Insert: write the data pair into C0 which takes O(1) time. If a key is already present in the database, insert will simply update its value.
Update: write the data pair into which also takes O(1) time. If a key does not present in the database, it will simply insert this key-value pair into the database.
Delete: Operation always success, write key-value pair {<key>, "DELETE" } into C0. The delete operation propagate to C1 and become consistent when a merge happens. 
Get: First check LRU, then C0 and then finally C1. Returns NULL when nothing is found. 

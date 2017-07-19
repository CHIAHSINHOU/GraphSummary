# GraphSummary

##Install Guide
---

###Get our project
```
git clone git@github.com:CHIAHSINHOU/graphSummary.git
```

####Dependencies: 
* g++
* openmpi
* make
If you want to use python for clients, you need to install:
* python
* socket
* binascii
* struct

###Installation:
* Execute Makefile
    ```
    make
    ```
###Test:
* Create graph folder
    ```
    mkdir testcase/graph
    ```
* Generate partitioned graph data
    ```
    ./distribute testcase
    ```
* Run graphsummary server on 3 nodes at least
    ```
    mpirun -n 3 ./sp_server -t testcase
    ```
* Run client
    ```
    python client.py
    ```

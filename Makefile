#compiler 
CXX=mpic++
#cxxflags
CXXFLAGS=-pthread -g -O2
#include path 
H_PATH=include
#source path
S_PATH=source
#target path
T_PATH=target
#source
DIS=distributeGraphToMultipleGraphFiles.cpp
MAIN=main.cpp
SRCS=Graph.cpp Group.cpp SimpleGraph.cpp structs.cpp funcs.cpp BigGraph.cpp writeLog.cpp procInfo.cpp constraints.cpp summary.cpp sssp.cpp answerQuery.cpp receiveQuery.cpp
#object
OBJS=$(SRCS:%.cpp=$(T_PATH)/%.o)
MAINO=$(MAIN:%.cpp=$(T_PATH)/%.o)
DISO=$(DIS:%.cpp=$(T_PATH)/%.o)

all:	sp_server distribute

sp_server:	$(OBJS) $(MAINO)
	$(CXX) $(CXXFLAGS) -I$(H_PATH) $(MAINO) $(OBJS) -o $@

distribute: $(OBJS) $(DISO)
	$(CXX) $(CXXFLAGS) -I$(H_PATH) $(DISO) $(OBJS) -o $@ 

$(T_PATH)/%.o: 	$(S_PATH)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(H_PATH) -c -o $@ $<

clean: 
	-rm -f cc_server $(T_PATH)/*.o 
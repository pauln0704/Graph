/**
 * Name: Paul Nguyen
 *
 * Contains function declarartion for functions declared in Graph.h
 * Description of functions are also in Graph.h
 */
#include "Graph.h"

Graph::Graph(const string &edgelist_csv_fn) {
    ifstream my_file(edgelist_csv_fn); // open the file
    string line;  // will store current line

    edgeCount = 0; // initialize number of edges to 0

    // will read through file line by line
    while(getline(my_file, line)) {
        istringstream ss(line); // create istringstream of current line
        string vertex1, vertex2; // vertices of read edge
        string weight; // weight of read edge

        // gets edge info in each column
        getline(ss, vertex1, ','); 
        getline(ss, vertex2, ',');
        getline(ss, weight, '\n');

        // add edge and weight to matrix
        if (vertex1 != vertex2){ // check self-loop
            graphMtx[vertex1][vertex2] = stoi(weight);
            graphMtx[vertex2][vertex1] = stoi(weight);
        }

        // increment number of edges
        edgeCount++;
    }

    // close file when done    
    my_file.close();
}

unsigned int Graph::num_nodes() {
    return graphMtx.size();
}

unordered_set<string> Graph::nodes() {
    unordered_set<string> nodes; // holds nodes of graph

    // iterate down rows of ajacency matrix to get nodes
    for (auto it = graphMtx.begin(); it != graphMtx.end(); it++){
        nodes.insert(it->first);
    }

    return nodes;
}

unsigned int Graph::num_edges() {
    return edgeCount;
}

unsigned int Graph::num_neighbors(string const &node_label) {
    unsigned int count = 0; // hold number of neightbors

    // check if node label exist in matrix
    if(graphMtx.count(node_label) == 0){
        return 0;
    }

    // interates through each column in a row to get neighbors
    for(auto it = graphMtx[node_label].begin();
        it != graphMtx[node_label].end(); it++){

        count++;
    }

    return count;
}

int Graph::edge_weight(string const &u_label, string const &v_label) {
    // checks if edge exist then gets weight
    if (graphMtx.count(u_label) == 1 && graphMtx[u_label].count(v_label) == 1){
        return graphMtx[u_label][v_label];
    }
    // returns -1 if edge DNE
    else{
        return -1;
    }
}

unordered_set<string> Graph::neighbors(string const &node_label) {
    unordered_set<string> Nbr; // holds all neighbors of a node

    // interates through each column in a row to get neighbors
    for(auto it = graphMtx[node_label].begin();
        it != graphMtx[node_label].end(); it++){

        Nbr.insert(it->first);
    }

    return Nbr;
}

vector<tuple<string, string, int>>
Graph::shortest_path_weighted(string const &start_label,
                              string const &end_label) {
    
    vector<tuple<string, string, int>> rt; // the vector to be returned    

    // special case if start and end are the same 
    if (start_label == end_label){
        rt.push_back(make_tuple(start_label, end_label, 0));
        return rt;
    }

    // special case if the Node DNE
    if (graphMtx.count(start_label) == 0 || graphMtx.count(end_label) == 0){
        return rt;
    }
    
    unordered_map<string, Graph::Node*> nodes; // holds all created nodes

    // calls helper method to get shortest path of all nodes
    nodes = dijkstraAlg(start_label);

    
    string currNode = end_label; // keep track of current node

    // iterate through shortest weighted path
    while (nodes[currNode]->previous != ""){
        string prev = nodes[currNode]->previous; // previous node
        int weight = edge_weight(currNode, prev); // weight of edge

        // insert edge into vector as tuple
        rt.insert(rt.begin(), make_tuple(prev, currNode, weight));

        currNode = prev;
    }

    // delete all created nodes
    freeNodes(nodes);

    return rt;
    
}

int Graph::smallest_connecting_threshold(string const &start_label,
                                         string const &end_label) {
    // special case if start and end are the same
    if (start_label == end_label){
        return 0;
    }

    // build minimum spanning tree if one is not created yet
    if (minSpanTree.empty()){
        minSpanTree = minSpanning();
    }

    // find the threshold in the minimum spanning tree
    int threshold = -1;

    thresholdPath(start_label, "", end_label, threshold);

    return threshold;
    

}

unordered_map<string, Graph::Node*> Graph::dijkstraAlg(
        string const &start_label){

    // create a container of all nodes in graph with distance infinity
    unordered_map<string, Node*> cn;
    cn = createNodes();

    // set start node distance to 0
    cn[start_label]->distance = 0;

    // creates priority queue containing tuples
    priority_queue<tuple<int, string>, 
                   vector<tuple<int, string>>, compare> pq;

    // tuple format is <0>distance <1>curr_node

    // push first edge onto stack
    pq.push(make_tuple(0, start_label));

    while(!pq.empty()){

        // pop for priority queue
        tuple<int, string> poped = pq.top();
        pq.pop();

        // if current node not done
        if (!cn[get<1>(poped)]->done){

            // mark node as done and update node's fields
            cn[get<1>(poped)]->done = true;

            // goes through all the neighbor edges
            for(auto it = graphMtx[get<1>(poped)].begin();
                it != graphMtx[get<1>(poped)].end(); it++){
                
                int totalDist = get<0>(poped) + it->second; // total distance

                // if totalDist < w's current distance
                // (d currNode distance, e edge weight, w neighbor node)
                if (totalDist < cn[it->first]->distance){
                    // w current distance = totalDist
                    cn[it->first]->distance = totalDist;
                    // w previous node = curr Node
                    cn[it->first]->previous = get<1>(poped);
                    // add (totalDist, w) to priority queue
                    pq.push(make_tuple(totalDist, it->first));
                }
            }
        }
    }

    return cn;
}

unordered_map<string, Graph::Node*> Graph::createNodes(void){
    unordered_map<string, Node*> cn; // container of nodes to be returned

    // iterate down rows of ajacency matrix to get all nodes
    for (auto it = graphMtx.begin(); it != graphMtx.end(); it++){
        Node* newNode = new Node(it->first);
        cn.insert({it->first, newNode});
    }

    return cn;
}

void Graph::freeNodes(unordered_map<string, Graph::Node*> nodes){
    for (auto it = nodes.begin(); it != nodes.end(); it++){
        delete it->second;
    }
}
unordered_map<string, unordered_map<string, int>> Graph::minSpanning(void){
    
    // min heap for edge. lesser edge weights go in front
    priority_queue<tuple<int, string, string>, 
                   vector<tuple<int, string, string>>, compare2> pq;

    unordered_map<string, pair<string, int>> upTree; // represent up-tree

    // iterate matrix for every edge to fill queue and upTree
    for(auto it = graphMtx.begin(); it != graphMtx.end(); it++){   
        // add node to upTree as pair object
        // pair is (string of parent, negative size of set if sentinal node)
        upTree[it->first] = make_pair("", -1);
        for (auto jt = it->second.begin(); jt != it->second.end(); jt++){
            // add edge to queue
            pq.push(make_tuple(jt->second, it->first, jt->first));
        }
    }
    
    // minimum spanning tree to be returned
    unordered_map<string, unordered_map<string, int>> minTree;

    // build spanning tree using Kruskal's 
    while(!pq.empty()){
        // remeber edge (u,v) and (v, u)
        tuple<int, string, string> edge = pq.top(); // get edge from queue
        pq.pop();

        // check if edge is already in tree
        bool duplicate = (minTree[get<1>(edge)].count(get<2>(edge)) == 1) || 
                         (minTree[get<2>(edge)].count(get<1>(edge)) == 1);

        // cycle is found or edge is already in tree so dont add to min tree
        if (setFind(get<1>(edge), upTree) == setFind(get<2>(edge), upTree) || 
            duplicate){

            continue;
        }
        // add edge to tree and union the sets
        else{
            minTree[get<1>(edge)][get<2>(edge)] = get<0>(edge);
            minTree[get<2>(edge)][get<1>(edge)] = get<0>(edge);
            setUnion(get<1>(edge), get<2>(edge), upTree);
        }
    }

    return minTree;
}

string Graph::setFind(string node, 
                      unordered_map<string, pair<string, int>> &upTree){

    // sentinal node found
    if(upTree[node].first == "" && upTree[node].second < 0){
        return node;
    }

    // keep traversing up tree to get sentinal node
    else{
        string sentinal = setFind(upTree[node].first, upTree); // hold sentinal
        upTree[node].first = sentinal; // path compression
        return sentinal;
    }
}

void Graph::setUnion(string u, string w, 
           unordered_map<string, pair<string, int>> &upTree){
    
    //get sentinals of each node
    string uSentinal = setFind(u, upTree);
    string wSentinal = setFind(w, upTree);

    // u set has more nodes than w set
    if (upTree[uSentinal].second < upTree[wSentinal].second){
        // make u the sentinal of w
        upTree[wSentinal].first = uSentinal;
        // update u sentinal size to incluse new node (note: negative value)
        upTree[uSentinal].second += upTree[wSentinal].second;
        // set w size to 0 to signify it is not sentinal
        upTree[wSentinal].second = 0;
    } 

    // if u set has less or the same nodes as v set
    else{
        // make w the sentinal of u
        upTree[uSentinal].first = wSentinal;
        // update w sentinal size to incluse new node (note: negative value)
        upTree[wSentinal].second += upTree[uSentinal].second;
        // set u size to 0 to signify it is not sentinal
        upTree[uSentinal].second = 0;
    }
}

bool Graph::thresholdPath(string currNode, string prevNode, 
                             string endNode, int& t){
    // end node is found
    if (currNode == endNode){
        return true;
    }

    // go through all neighbors of currNode
    for(auto it = minSpanTree[currNode].begin();
        it != minSpanTree[currNode].end(); it++){

        // do not go back down edge that was just traversed
        if(it->first == prevNode) continue;
    
        // goes to neighbor nodes to look for ending node
        if(thresholdPath(it->first, currNode, endNode, t)){
            // path to end is found
            // update threshold if greater weight found
            if (minSpanTree[currNode][it->first] > t){
                t = minSpanTree[currNode][it->first];
            }
            return true;
        }
    }

    // endNode was not found for this recurssion
    return false;
}

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;


class EdgeG {
public:
    int u, v;
    string name;
};

class EdgeH {
public:
    int u, v;
    string name;
};

class Vertex {
public:
    string name;
    int id = 0;
    vector<int> successors;
};

int FindSet(vector<int>& reps, int i) {
    if (reps[i] == i)
        return i;
    return reps[i] = FindSet(reps, reps[i]); 
}

void CompressReps(vector<int>& reps, int u_end, int v_start) {
    int root_u = FindSet(reps, u_end); 
    int root_v = FindSet(reps, v_start); 
    if (root_u != root_v) {
        reps[root_v] = root_u;
    }
}

bool ReadGraph(string fileName, vector<Vertex>& vertices, vector<EdgeG>& edgeG) {
    ifstream file(fileName);
    if(!file) {
        cout<<"Nie mozna otworzyc pliku: "<<fileName<<"\n";
        return false;
    }

    string line;
    if(!getline(file, line)) return false;

    stringstream ss(line); 
    string vName;
    int idCounter=0; 

    while(ss>>vName) {
        Vertex v;
        v.name=vName;
        v.id=idCounter++;
        vertices.push_back(v);
    }

    while(getline(file, line)) {
        stringstream in(line);
        string a, b, label;
        in>>a>>b>>label;

        int ia=-1, ib=-1;
        for(int i=0; i<vertices.size(); i++) {
            if(vertices[i].name == a) {
                ia = vertices[i].id; 
            }
            if(vertices[i].name == b) {
                ib = vertices[i].id; 
            }
        }
        edgeG.push_back({ia, ib, label});
        vertices[ia].successors.push_back(ib);
    }

    for(int i=0; i<vertices.size(); i++) {
        sort(vertices[i].successors.begin(), vertices[i].successors.end());
    }

    return true;
}

bool AdjointGraph(vector<Vertex> vertices) {
    for(int i=0; i<vertices.size(); i++) {
        for(int k=0; k<vertices[i].successors.size(); k++) {
            if(k>0 && vertices[i].successors[k] == vertices[i].successors[k - 1]) return false;
        }
    }

    for(int i=0; i<vertices.size(); i++) {
        for(int j=i+1; j<vertices.size(); j++) {

            bool common=false;
            
            for (int k=0; k<vertices[i].successors.size(); k++) {
                int x=vertices[i].successors[k];
                for (int l=0; l<vertices[j].successors.size(); l++) {
                    int y=vertices[j].successors[l];
                    if (x == y) { 
                        common=true; 
                        break; 
                    }
                }
                if (common) break;
            }
            if (common && vertices[i].successors != vertices[j].successors) return false;
        }
    }
    return true;
}

void ReconstructGraph(vector<Vertex> vertices, vector<EdgeG> edgeG,
    vector<EdgeH>& edgeH)
{
    vector<int> reps(2*vertices.size());
    for(int i=0; i<(2*vertices.size()); i++) reps[i] = i;

    for(int i=0; i<edgeG.size(); i++) {
        int u_end = 2*edgeG[i].u + 1;
        int v_start = 2*edgeG[i].v;
        CompressReps(reps, u_end, v_start);
    }

    vector<int> mapping(2*vertices.size(), -1);
    int LabelCounter=0;

    for(int i=0; i<vertices.size(); i++) {
        int rootU = FindSet(reps, 2*i);
        int rootV = FindSet(reps, 2*i+1);
        if(mapping[rootU] == -1) {
            mapping[rootU] = LabelCounter++;
        }
        if(mapping[rootV] == -1) {
            mapping[rootV] = LabelCounter++;
        }
    }

    for(int i=0; i<vertices.size(); i++) {
        int uH = mapping[FindSet(reps, 2*i)];
        int vH = mapping[FindSet(reps, 2*i+1)];
        edgeH.push_back({uH, vH, vertices[i].name});
    }
}

bool LineGraph(vector<EdgeH> edgeH) {
    for (int i=0; i<edgeH.size(); i++) {
        for (int j=i+1; j<edgeH.size(); j++) {
            if (edgeH[i].u == edgeH[j].u && edgeH[i].v == edgeH[j].v) return false;
        }
    }
    return true;
}

bool SaveGraph(string outName, vector<EdgeH> edgeH) {
    ofstream out(outName);

    int maxLabel = -1;
    for(int i=0; i<edgeH.size(); i++) {
        if(edgeH[i].u > maxLabel) {
            maxLabel = edgeH[i].u;
        }
        if(edgeH[i].v > maxLabel) {
            maxLabel = edgeH[i].v;
        }
    }

    int count = maxLabel + 1;
    for(int i=0; i<count; i++) {
        out<<"e"<< i;
        if(i < maxLabel) {
            out<<" ";
        }
    }

    out << "\n";

    for(int i=0; i<edgeH.size(); i++) {
        out<<"e"<<edgeH[i].u<<" e"<<edgeH[i].v<<" "<< edgeH[i].name<<"\n";
    }

    return true;
}

void menu() {
    string fileName;
    cout<<"Podaj nazwe pliku z grafem: ";
    cin>>fileName;

    vector<Vertex> vertices;
    vector<EdgeG> edgeG;

    if(!ReadGraph(fileName, vertices, edgeG)) {
        cout<<"Blad wczytania.\n"; 
        return;
    }

    cout<<"Wczytano graf: "<<fileName<<"\n";

    if(AdjointGraph(vertices)) {
        cout<<"Graf jest sprzezony.\n";

        vector<EdgeH> edgeH;
        ReconstructGraph(vertices, edgeG, edgeH);

        if(LineGraph(edgeH)) {
            cout<<"Graf jest liniowy.\n";
        }
        else {
            cout<<"Graf nie jest liniowy.\n";
        }

        string outName="rekonstrukcja_"+fileName;
        if(SaveGraph(outName, edgeH)) { 
            cout<<"Zapisano wynik do: "<<outName<<"\n"; 
        }
    }
    else {
        cout<<"Graf nie jest sprzezony.\n";
    }
}

int main() {
    menu();
    return 0;
}

#include <sstream>
#include <unordered_map> 
#include "ct2.hpp"
#include "unitval.hpp"

using namespace std;

// Public constructor
CT2::CT2(Hector::unitval totC, string pool){
    track = false;
    totalCarbon = totC;
    ctmap[pool] = 1;
}

// Private constructor with explicit source pool map
CT2::CT2(Hector::unitval totC, unordered_map<string, double> pool_map, bool do_track){
    track = do_track;
    totalCarbon = totC;
    ctmap = pool_map;
}

bool CT2::isTracking() const {
    return track;
}

void CT2::setTracking(bool do_track){
    track = do_track;
}


std::vector<std::string> CT2::get_sources() const {
    H_ASSERT(track, "get_sources requires tracking to be turned on");
    std::vector<std::string> sources;
    for (auto itr = ctmap.begin(); itr != ctmap.end(); itr++) {
        sources.push_back(itr->first);
    }
    return sources;
}

double CT2::get_fraction(string source) const {
    H_ASSERT(track, "get_fraction requires tracking to be turned on");
    double val = 0.0;  // 0.0 is returned if not in our map
    auto x = ctmap.find(source);
    if(x != ctmap.end()) {
        return x->second;
    }
    return val;
}

CT2 CT2::operator+(const CT2& flux){
    Hector::unitval totC = totalCarbon + flux.totalCarbon;
    unordered_map<string, double> new_origins;
    
    if(track) {
        unordered_map<string, Hector::unitval> new_pools;

        // Look through *our* sources, and if any in other object, add
        for (auto itr = ctmap.begin(); itr != ctmap.end(); itr++) {
            new_pools[itr->first] = totalCarbon * itr->second + flux.totalCarbon * flux.get_fraction(itr->first);
        }
        
        // Look through the *other* object sources, and if any NOT in our map, add
        std::vector<std::string> sources = flux.get_sources();
        for (int i = 0; i < sources.size(); i++) {
            const string src = sources[i];
            if(ctmap.find(src) == ctmap.end()) {  // source that's not in our map
                new_pools[src] = flux.totalCarbon * flux.get_fraction(src);
            }
        }
        
        // Now that we have the new pool values, compute new fractions
        for (auto itr = new_pools.begin(); itr != new_pools.end(); itr++) {
            new_origins[itr->first] = itr->second / totC;
        }
    } // if(track)
    
    CT2 addedFlux(totC, new_origins, track);
    return addedFlux;
}

ostream& operator<<(ostream &out, CT2 &ct ){
    out << ct.totalCarbon << endl;
    std::vector<std::string> sources = ct.get_sources();
    for (int i = 0; i < sources.size(); i++) {
        out << "\t" << sources[i] << ": " << ct.get_fraction(sources[i]) << endl;
    }
    return out;
}

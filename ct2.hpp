#ifndef CT2_HPP
#define CT2_HPP
#include <sstream>
#include <unordered_map> 
#include "unitval.hpp"
#include <vector>

using namespace std;

  /**
   * \brief CarbonTracker Class: class to track origin of carbon in various carbon pools as it moves throughout the carbon cycle
   * in simple climate model Hector
   * 
   * Designed so that it can be dropped in place of a unitval in the Hector C++ code base
   */

class CT2 {

    /**
      *\brief parameterized constructor - initialize pools of carbon with pg carbon (unitvals)
      *\param totC unitval (units pg C) that expresses total amount of carbon in the pool
      *\param subPool Pool object to set as inital origin of carbon in the pool at time of creation
      */
public:
    CT2(Hector::unitval totC, string subPool);
    
    /**
      * \brief addition between two carbon tracker objects (one pool and one flux)
      *         - if tracking, then total carbon is the sum of the two total carbons and the map of the new CarbonTracker
      *           object will reflect percentages of added pools
      *         - else only the total carbon's will be added and the pool's array will be used (flux must be created using
      *           fluxFromTrackerPool and if not tracking array will be all 0s)
      * \param flux carbon tracker object that is being added to 'this', needs total carbon unitval (unit pg C)
      * \returns CarbonTracker object with updated total carbon and map
      */
    CT2 operator+(const CT2& flux);
    vector<string> get_sources() const;
    double get_fraction(std::string source) const;

    bool isTracking() const;

    void setTracking(bool do_track);

    // pretty printing
    friend ostream& operator<<(ostream &out, CT2 &ct);


private:
     // Total amount of carbon in a pool represented by a CarbonTracker object - in petagrams carbon (U_PGC)
    Hector::unitval totalCarbon;
    // Unordered map holds the fractions of `totalCarbon` (map value) corresponding to each source pool (key)
    unordered_map<string, double> ctmap;
    bool track;
    
    // internal constructor with explicit source pool map
    CT2(Hector::unitval totC, unordered_map<string, double> pool_map, bool do_track);
};

#endif

#ifndef CARBONTRACKER_HPP
#define CARBONTRACKER_HPP
#include <sstream>
#include <unordered_map> 
#include "unitval.hpp"

  using namespace std;

      // enum Pool {
      //   SOIL, ATMOSPHERE, OCEAN
      // };

  /**
   * \brief CarbonTracker Class: class to track origin of carbon in various carbon pools as it moves throughout the carbon cycle
   * in simple climate model Hector
   * 
   * Designed so that it can be dropped in place of a unitval in the Hector C++ code base
   */
  class CarbonTracker{

      // Total amount of carbon in a pool represented by a CarbonTracker object - in petagrams carbon (U-PGC)
      Hector::unitval totalCarbon;

      // map where each key is the origin/type of carbon parcel and the value is the
      // percent of the totalCarbon that comes from the origin/type noted by the key name
      unordered_map<CarbonTracker::MultiKey, double>  origin_fracs; 

      // boolean to signify if tracker should be tracking carbon movement
      bool track;
    
    private:
      /**
       * \brief MultiKey Class: custom key class for map in CarbonTracker class
       * 
       * Needs ability to hold multiple keys to further specify types of origin carbon
       * (i.e. soil and carbon 14 - ability to add isotopes and other modifiers in the future)
       */
      class MultiKey{
        public:
          String poolName;
          MutliKey(String key1);

          // std::maps require an operator<() fuction in order to function properly
          bool operator<(const MutliKey& right) const;
      };



    public:

      CarbonTracker() = delete;
      ~CarbonTracker();
      CarbonTracker (const CarbonTracker& ct);

      /**
       *\brief parameterized constructor - useful for initializing pools of carbon with only pg carbon (unitvals)
      *\param totalCarbon unitval (units pg C) that expresses total amount of carbon in the pool
      *\param key MultiKey object to set as inital origin of carbon in the pool at time of creation
      *\param track boolean that represents if the carbonTracker object should start tracking origins
      * \return CarbonTracker object with totalCarbon set and a map with one key-value pair (key, 1) as all carbon is from inital source
      */
      CarbonTracker(Hector::unitval totalCarbon, MultiKey key, bool track = false);

      /**
       *\brief parameterized constructor - can set sub-pools of carbon, probably only usful for specialized experiments 
      *\param origin_val map with specific key-value pairs representing the origin and precent of the total carbon in each sub-pool
      *\param totalCarbon unitval (units pg C) that expresses total amount of carbon in the pool
      *\param track boolean that represents if the carbonTracker object should start tracking origins
      *\return CarbonTracker object with set totalCarbon and custom map input
      */
      CarbonTracker(unordered_map<MultiKey, double> origin_val, Hector::unitval totalCarbon, bool track = true);

      /**
       * \brief addition between two carbon tracker objects - if 'this' is carbon tracking, then total carbon is the sum
       * of the two total carbons and the map of the new CarbonTracker object will reflect percentages of added pools
       * \param flux carbon tracker object that is being added to 'this', needs total carbon unitval (unit pg C) and valid map
       * \returns CarbonTracker object with updated total carbon and map
       */ 
      CarbonTracker operator+(const CarbonTracker& flux);


      /**
       * \brief subtraction between two CarbonTracker objects - if 'this' is ctracking, total carbon will be reduced and the map
       * will be updated to new proportions 
       * \param flux carbon tracker object that is being subtracted from 'this', needs total carbon unitval (unit pg C) and valid map
       * \return CarbonTracker object with decreased total carbon and upated map
       */ 
      CarbonTracker operator-(const CarbonTracker& flux);

      /**
       * \brief subtraction between CarbonTracker object and a unitval - decreases total carbon and leaves map the same -
       *  new carbon is removed evenly from current sub-pools
       * \param flux unitval with units pg C
       * \return CarbonTracker object with decreased total carbon and unchanged map from 'this'
       */ 
      CarbonTracker operator-(const Hector::unitval flux);

      /**
       * \brief multiplication between double and CarbonTracker object - usually used to get fraction of a pool
       * \param d double (usually a fractional value to get sub-section of pool)
       * \param ct CarbonTracker object with unitval (pg C) total carbon and valid map
       * \return CarbonTracker object with d*(ct totalCarbon) and unchanged map 
       */ 
      CarbonTracker operator*(const double d, CarbonTracker& ct);

      /**
       * \brief multiplication between CarbonTracker object and double - usually used to get fraction of a pool
       * opposite order of paramters from above
       * \param d double (usually a fractional value to get sub-section of pool)
       * \param ct CarbonTracker object with unitval (pg C) total carbon and valid map
       * \return CarbonTracker object with d*(ct totalCarbon) and unchanged map 
       */ 
      CarbonTracker operator*(const CarbonTracker& ct, double d);

      /**
       * \brief divison of a CarbonTracker object by a double - usually used to get fraction of a pool
       * opposite order of paramters from above - implemnted to make sure unitval operations are still allowed
       * \param d double (usually a fractional value to get sub-section of pool)
       * \param ct CarbonTracker object with unitval (pg C) total carbon and valid map
       * \return CarbonTracker object with (ct totalCarbon)/d and unchanged map 
       * UNSURE IF THIS IS USED EITHER
       */ 
      CarbonTracker operator/(CarbonTracker&, const double);

      /**
       * \brief setter for total carbon within CarbonTracker object
       * \param totalCarbon unitval with units (pg C)
       */ 
      void setTotalCarbon(Hector::unitval totalCarbon);

      /**
       * \brief setter for map object within CarbonTracker object
       * \param origin_frac map with MultiKeys key and double values
       */ 
      void setCarbonMap(unordered_map<MultiKey, double> origin_frac);

      /**
       * \brief getter for CarbonTracker total carbon
       * \return unitval with units (pg C)
       */ 
      Hector::unitval getTotalCarbon();

      /**
       * \brief getter for entire CarbonTracker map
       * \return returns map object with MultiKey keys and double values
       */ 
      unordered_map<MultiKey, double> getCarbonMap();

      /**
       * \brief getter for indiviudal key-value pairs within a CarbonTracker map object
       * \param MultiKey object
       * \return value associated with key param, if no key matches returns 0
       */ 
      Hector::unitval getOriginCarbon(MultiKey origin);

      /**
       * \brief converts a unitval to a CarbonTracker object so that it can be added to a sub-pool of a CarbonTracker object
       * \param flux unitval with units (pg C)
       * \param key sub-pool into which the carbon flux will be distributed
       * \return CarbonTracker object with total carbon set to flux and a map with one key-value pair (key, 1)
       */ 
      CarbonTracker fluxToTracker(const Hector::unitval flux, MultiKey key);

      /**
       * \brief starts tracking and makes CarbonTracker param track = true
       */ 
      void startTracking();
  };
  ostream& operator<<(ostream &out, const CarbonTracker &x );
#endif
#include <iostream>
#include <vector>

#include <segments.hpp>


using namespace ADS;
using namespace std;



SegmentImplementation::SegmentImplementation()
        :is_enable(false)
{
    ;
}


AdsManager::AdsManager()
{
    ;
}

unsigned int AdsManager::InsertSegment(SegmentImplementation<T> * in_) {
    segments.push_back(in_);
    return 0;
}





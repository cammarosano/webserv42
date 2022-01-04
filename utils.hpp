#ifndef UTILS_HPP
# define UTILS_HPP

# include "ListenEndPoint.hpp"
# include <vector>
# include <algorithm>

bool comp_max_fd(ListenEndPoint &a, ListenEndPoint &b)
{
    return (a.max_fd < b.max_fd);
}

// maximum file descriptor number among all ListenEndPoints in the vector
int max_fd(std::vector<ListenEndPoint> & leps)
{
    std::vector<ListenEndPoint>::iterator it;

    // each LEP keeps track of its maximum fd. Find which LEP has the biggest of all
    it = std::max_element(leps.begin(), leps.end(), comp_max_fd);

    return (it->max_fd);
}


#endif
#include <cmath>
#include <vector>

#define PRECISION 0.000001
#define BRUTE_FORCE_NO 10 // default is 2, special handling is required for taking nth_element two elements

unsigned *dim_conv; // for dimension convert

class Point
{
    public:
        Point( unsigned dim ) { coord.resize(dim); }
        inline double getCoord( unsigned dim ) { return coord[dim_conv[dim]]; }//{ return coord[coord.size()-dim]; }

std::vector<double> coord; // coord[0] - x, coord[1] - y, ..., etc.
unsigned index;            // avoid same coordinate, forcing to be different
};

class KDCompClass
{
    public:
        KDCompClass( unsigned d ) { dim = d; }
        KDCompClass( void ) {}

        bool operator()( const Point &p1, const Point &p2 ) 
        {
            if( fabs( p1.coord[dim_conv[dim]]-p2.coord[dim_conv[dim]] ) < PRECISION )
            {
                if( p1.index < p2.index )
                    return true;
                else
                    return false;
            }
            else
                return ( p1.coord[dim_conv[dim]] < p2.coord[dim_conv[dim]] );
        }

        unsigned dim;
};

// 1D node
class BasicNode
{
    public:
        void setPValIndex( double v, unsigned &i ) { val = v; p_index = i; }

        double   val;     // 1D value of input points
        unsigned p_index; // same with index of Point class for fractional cascading

        // for fractional cascading
        int left_id;  
        int right_id;
};

class KDNode
{
    public:
        void setPBeginEnd( std::vector<Point>::iterator pb, std::vector<Point>::iterator pe ) 
        { pBegin = pb; pEnd = pe; }

        bool     is_leaf;
        double   key;
        unsigned left_child, right_child;
        std::vector<Point> point_vec;
        std::vector<Point>::iterator pBegin, pEnd;
        std::vector<KDNode>    aux_tree;   // kD to (k-1)D, k>2
        std::vector<BasicNode> aux_vec;    // only 2D to 1D
};

typedef std::vector<Point>::iterator vPointIter;
typedef std::vector<BasicNode>::iterator vBasicNodeIter;

void check_points_dominate( vPointIter &pBegin, vPointIter &pEnd, Point &q_point, unsigned &rank );

void check_point_dominate( Point &t_point, Point &q_point, unsigned &rank );

bool cmp_BasicNode_val( const BasicNode &bn1, const BasicNode &bn2 )
{
    if( fabs( bn1.val-bn2.val ) < PRECISION )
    {
        if( bn1.p_index < bn2.p_index )
            return true;
        else
            return false;
    }
    else
        return (bn1.val < bn2.val);
}

bool cmp_BasicNode_val_woIndex( const BasicNode &bn1, const BasicNode &bn2 )
{
    return (bn1.val < bn2.val);
}

unsigned find_split( std::vector<KDNode> &kdtree, unsigned nid, double val );

void parse_inputs( unsigned &dim, unsigned &point_no, std::vector<Point> &point_vec );

void range2D_query( std::vector<KDNode> &kdtree, Point &q_point, unsigned &rank );

void rangeKD_query( std::vector<KDNode> &kdtree, Point &q_point, unsigned dim, unsigned &rank );

void tree2D_gen( std::vector<KDNode> &kdtree, std::vector<Point> &point_vec, vPointIter pBIter, vPointIter pEIter );

void treeKD_gen( std::vector<KDNode> &kdtree, unsigned dim, std::vector<Point> &point_vec, const vPointIter pBIter, const vPointIter pEIter );

// case generator & checker
void case_gen( unsigned dim, unsigned point_no, unsigned query_no );

unsigned checker( std::vector<Point> &point_vec, Point &q_point, unsigned dim );

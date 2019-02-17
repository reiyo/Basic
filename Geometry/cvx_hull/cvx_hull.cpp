/*************************************************************************************

  Input: List of points
  Output: convex polygon 
  Time complexity: O(nlogn), n is # of input points
  Method: Graham scan 

 ************************************************************************************/

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <vector>

class point
{
    public:
        point( void ) {}
        point( long long int _x, long long int _y ): x(_x), y(_y) {}
        long long int x, y; /* x, y coordinates */
};

using std::vector;

/* global variable */
point minPoint(0, 0); 

bool cmp_small_angle( const point &p1, const point &p2 );

double cross( const point &i, const point &j, const point &k );

/* comparison function - ascending sort (p1 angle < p2 angle ) */
bool cmp_small_angle( const point &p1, const point &p2 )
{
    return ( cross(minPoint, p1, p2)>0 );
}

/* cross, i.e. determinant */
double cross( const point &i, const point &j, const point &k ) 
{ 
    return ( static_cast<double>((j.x - i.x))*(k.y - i.y) - static_cast<double>((k.x - i.x))*(j.y - i.y) ); 
} 

int main( int argc, char **argv )
{
    /* read point num and do initialization */

    //freopen( "../convex_case/case2.in", "r", stdin );

    unsigned point_no, point_vec_size;
    scanf( "%u", &point_no );
    point_vec_size = point_no + 1;

    vector<point> point_vec;
    point_vec.resize(point_vec_size);

    /* tackle the boundary case */

    if( point_no <= 2 )
    {
        printf( "%u\n", point_no );

        if( point_no == 1 )
        {
            scanf( "%lld %lld", &(point_vec[0].x), &(point_vec[0].y) );
            printf( "%lld %lld\n", point_vec[0].x, point_vec[0].y );
        }
        else 
        {
            scanf( "%lld %lld", &(point_vec[0].x), &(point_vec[0].y) );
            scanf( "%lld %lld", &(point_vec[1].x), &(point_vec[1].y) );
            printf( "%lld %lld\n", point_vec[0].x, point_vec[0].y );
            printf( "%lld %lld\n", point_vec[1].x, point_vec[1].y );
        }

        return 0;
    }

    /* get input data & locate the min y */

    scanf( "%lld %lld", &(point_vec[0].x), &(point_vec[0].y) );

    long long int min_y  = point_vec[0].y;
    unsigned min_id = 0;

    for( unsigned i=1; i<point_no; ++i )
    {
        scanf( "%lld %lld", &(point_vec[i].x), &(point_vec[i].y) );

        if( point_vec[i].y < min_y )
        {
            min_y = point_vec[i].y;
            min_id = i;
        }
    }

    /* swap the min y to the beginning of point vector */

    if( min_id != 0 )
    {
        minPoint = point_vec[min_id];
        point_vec[min_id] = point_vec[0];
        point_vec[0] = minPoint;
    }
    else
        minPoint = point_vec[0];

    point_vec[point_no] = minPoint;

    /* sort angle of point_vec[1]~point_vec[point_no] in ascending order */

    sort( (point_vec.begin())+1, (point_vec.end())-1, cmp_small_angle );

    point tmpPoint;
    unsigned check = 2; // since point_vec[0], point_vec[1] must be extreme points

    for( unsigned i=3; i<point_vec_size; ++i )
    {
        while( cross(point_vec[check-1], point_vec[check], point_vec[i]) < 0 )
            check -= 1;

        check += 1;

        // swap point_vec[check] and point_vec[i]
        tmpPoint = point_vec[check];
        point_vec[check] = point_vec[i];
        point_vec[i] = tmpPoint;
    }

    /* output result */

    printf( "%u\n", check );

    for( unsigned i=0; i<check; ++i )
        printf( "%lld %lld\n", point_vec[i].x, point_vec[i].y );

    return 0;
}

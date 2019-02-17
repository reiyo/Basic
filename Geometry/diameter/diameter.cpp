/*************************************************************************************

  Input: A list of points representing a convex polygon
  Output: Two points defining the diameter, and the length of the diameter
  Time complexity: O(n), n is # of input points
  Method: Antipodal pairs checking

 ************************************************************************************/

#define NDEBUG

#include <cassert>
#include <cmath>
#include <cstdio>
#include <utility>
#include <vector>

#ifndef PI
#define PI 3.1415926535
#endif

#define PRECISION 0.000001
#define PREC_S180  179.999999 /* 180 degree (avoid floating problem) */
#define PREC_L180  180.000001 /* 180 degree (avoid floating problem) */

class point
{
    public:
        int    x, y;
        double angle;    /* included angle with vector (1, 0) */
        double angle360; /* included angle + 360 degree, since 0 degree is algo 360 degree */
};

using std::vector;

double brute_force( vector<point> &point_vec, std::pair <unsigned, unsigned> &max_pid );

double distance_sqr( const point &p1, const point &p2 );

double included_angle( const point &p1, const point &p2 );

/* brute-force search to check the solution */
double brute_force( vector<point> &point_vec, std::pair <unsigned, unsigned> &max_pid )
{
    static double   cur_dist;
    static double   max_dist;
    static unsigned tmp_size;

    assert( point_vec.size() > 1 );

    max_dist = 0.0;
    tmp_size = point_vec.size()-1;

    for( unsigned i=0; i<tmp_size; ++i )
    {
        for( unsigned j=i+1; j<point_vec.size(); ++j )
        {
            cur_dist = distance_sqr( point_vec[i], point_vec[j] );

            if( cur_dist > max_dist )
            {
                max_pid.first  = i;
                max_pid.second = j;
                max_dist = cur_dist;
            }
        }
    }

    return max_dist;
}

/* calculate square of Euclidean distance */
double distance_sqr( const point &p1, const point &p2 )
{
    static double x_diff, y_diff;
    x_diff = p1.x-p2.x;
    y_diff = p1.y-p2.y;

    return ( x_diff*x_diff + y_diff*y_diff );
}

/* calculate included angle between normal vector of two points and horizontal vector (1, 0) */
double included_angle( const point &p1, const point &p2 )
{
    static double included_angle;
    included_angle = atan2( (-1.0)*(p2.x - p1.x), static_cast<double>(p2.y - p1.y) ) * 180.0/PI;

    if( included_angle < 0 )
        return (included_angle + 360);
    else
        return included_angle;
}

int main( int argc, char **argv )
{
    /* read input points */

    //freopen( "../convex_case/case2.out", "r", stdin );

    unsigned point_no;
    scanf( "%u", &point_no );

    vector<point> point_vec;
    point_vec.resize(point_no);

    for( unsigned i=0; i<point_no; ++i )
        scanf( "%d %d", &(point_vec[i].x), &(point_vec[i].y) );

    /* tackle simple or special case */

    if( point_no <= 3 )
    {
        if( point_no <= 0 )
            printf( "0.000000\n" );
        else if( point_no == 1 )
        {
            printf( "%d %d\n", point_vec[0].x, point_vec[0].y );
            printf( "0.000000\n" );
        }
        else
        {
            std::pair <unsigned, unsigned> max_pid;
            double max_dist = brute_force( point_vec, max_pid );

            printf( "%d %d\n", point_vec[max_pid.first].x, point_vec[max_pid.first].y );
            printf( "%d %d\n", point_vec[max_pid.second].x, point_vec[max_pid.second].y );
            printf( "%lf\n", sqrt( max_dist ) );
        }

        return 0;
    }

    /* calculate included angle between normal vector of arc and vector (1, 0) */

    unsigned last_pid = point_no - 1;

    for( unsigned i=0; i<last_pid; ++i )
    {
        point_vec[i].angle    = included_angle( point_vec[i], point_vec[i+1] );    	
        point_vec[i].angle360 = point_vec[i].angle + 360;
    }

    point_vec[last_pid].angle    = included_angle( point_vec[last_pid], point_vec[0] );
    point_vec[last_pid].angle360 = point_vec[last_pid].angle + 360;

    /* start from normal vector between point 0 & last point */

    std::pair <unsigned, unsigned> max_pid ( 0, last_pid );
    unsigned next_pid = 0; // next starting check point
    double max_dist = distance_sqr( point_vec[0], point_vec[last_pid] );
    double current_angle = 0.0;

    double s_angle = point_vec[last_pid].angle  + PREC_S180;
    double t_angle = point_vec[0].angle + PREC_L180;

    for( unsigned i=1; i<point_no; ++i )
    {
        /* avoid 0 degree & 360 degree problem */

        if( point_vec[i].angle < point_vec[last_pid].angle )
            current_angle = point_vec[i].angle360;
        else
            current_angle = point_vec[i].angle;

        /* hit the search boundary */

        if( current_angle > s_angle )
        {
            double current_dist = distance_sqr( point_vec[0], point_vec[i] );
            next_pid = i;

            if( current_dist > max_dist )
            {
                max_pid.second = i;
                max_dist = current_dist;
            }

            /* search for other candidates */

            for( unsigned j=i+1; j<point_no; ++j )
            {
                current_dist = distance_sqr( point_vec[0], point_vec[j] );

                if( current_dist > max_dist )
                {
                    max_pid.second = j;
                    max_dist = current_dist;
                }

                if( point_vec[j].angle < point_vec[0].angle )
                    current_angle = point_vec[j].angle360;
                else
                    current_angle = point_vec[j].angle;

                /* hit the search boundary */

                if( current_angle > t_angle )
                {
                    next_pid = j-1;
                    break;
                }
            }

            break;
        }
    }

    /* consider other points based on the previous location */

    unsigned consider_no = point_no-1;

    for( unsigned i=1; i<consider_no; ++i )
    {
        t_angle = point_vec[i].angle + PREC_L180;

        for( unsigned j=next_pid; j<point_no; ++j )
        {
            if( i == j ) 
                continue;

            double current_dist = distance_sqr( point_vec[i], point_vec[j] );

            /* update max distance if necessary */

            if( current_dist > max_dist )
            {
                max_pid.first  = i;
                max_pid.second = j;
                max_dist = current_dist;
            }

            if( point_vec[j].angle < point_vec[i].angle )
                current_angle = point_vec[j].angle360;
            else
                current_angle = point_vec[j].angle;

            if( current_angle > t_angle )
            {
                next_pid = j-1;
                break;
            }

        }
    }

    /* output result */

    printf( "%d %d\n", point_vec[max_pid.first].x, point_vec[max_pid.first].y );
    printf( "%d %d\n", point_vec[max_pid.second].x, point_vec[max_pid.second].y );
    printf( "%lf\n", sqrt( max_dist ) );

    /* debug by brute-force search */

#ifndef NDEBUG
    printf( "\nstart debug..." );
    std::pair<unsigned, unsigned> check_pid;
    double check_max_dist = brute_force( point_vec, check_pid );

    if( fabs( max_dist - check_max_dist ) > PRECISION ) 
    {
        printf( "\nWrong answer!! correct one:\n" );
        printf( "%d %d\n", point_vec[check_pid.first].x, point_vec[check_pid.first].y );
        printf( "%d %d\n", point_vec[check_pid.second].x, point_vec[check_pid.second].y );
        printf( "%lf\n", sqrt( check_max_dist ) );
    }

    printf( "done!!\n" );
#endif

    return 0;
}

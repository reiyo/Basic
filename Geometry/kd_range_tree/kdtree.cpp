/*************************************************************************************
 
   Input:  set of n points in k-dimensional space, set of q queries
   Output: list of numbers; for each query, report number of points being dominated
   Time complexity: O( n(logn)^(k-1) + q(logn)^(k-1) )
                    tree construction - O(n(logn)^(k-1)), k>=1 
                    each query - O((logn)^(k-1))
   Method: kd range tree

 ************************************************************************************/

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>

#include "kdtree.h"

using std::cout;
using std::endl;
using std::ios;
using std::ofstream;
using std::sort;
using std::vector;

unsigned DIMENSION;

// check multiple points using brute force
void check_points_dominate( vPointIter &pBegin, vPointIter &pEnd, Point &q_point, unsigned &rank )
{
    for( vPointIter tIter=pBegin; tIter!=pEnd; tIter++ )
        check_point_dominate( (*tIter), q_point, rank );

    return;
}

// check single point directly
void check_point_dominate( Point &t_point, Point &q_point, unsigned &rank )
{
    rank++; // guess

    for( unsigned i=0; i<q_point.coord.size(); ++i )
    {
        if( t_point.coord[i] > q_point.coord[i] )
        {
            rank--; // recover
            break;
        }
    }

    return;
}

// nid = 0 if it is a root
unsigned find_split( vector<KDNode> &kdtree, unsigned nid, double val )
{
    // all points are in the first quadrant, also x_l = 0
    if( (kdtree[nid].is_leaf) || (val - kdtree[nid].key >= 0) )
        return nid;
    else
        return find_split( kdtree, kdtree[nid].left_child, val );
}

// read input data and set unique id for points
void parse_inputs( unsigned &dim, unsigned &point_no, vector<Point> &point_vec )
{
    scanf( "%u %u", &dim, &point_no );
    assert( dim>=2 && dim<=6 );
    dim_conv = new unsigned[dim+1];

    for( unsigned i=1; i<=dim; ++i )
        dim_conv[i] = dim-i;

    // printf( "(Dim, PointNo): (%u, %u)\n", dim, point_no );
    point_vec.resize( point_no, Point(dim) );

    switch(dim)
    {
        case 2:
            for( unsigned i=0; i<point_no; ++i )
                scanf( "%lf %lf", &(point_vec[i].coord[0]), &(point_vec[i].coord[1]) );
            break;
        case 3:
            for( unsigned i=0; i<point_no; ++i )
                scanf( "%lf %lf %lf", &(point_vec[i].coord[0]), &(point_vec[i].coord[1]), &(point_vec[i].coord[2]) );
            break;
        case 4:
            for( unsigned i=0; i<point_no; ++i )
                scanf( "%lf %lf %lf %lf", &(point_vec[i].coord[0]), &(point_vec[i].coord[1]), &(point_vec[i].coord[2]), &(point_vec[i].coord[3]) );
            break;
        case 5:
            for( unsigned i=0; i<point_no; ++i )
                scanf( "%lf %lf %lf %lf %lf", &(point_vec[i].coord[0]), &(point_vec[i].coord[1]), &(point_vec[i].coord[2]), &(point_vec[i].coord[3]), &(point_vec[i].coord[4]) );
            break;
        case 6:
            for( unsigned i=0; i<point_no; ++i )
                scanf( "%lf %lf %lf %lf %lf %lf", &(point_vec[i].coord[0]), &(point_vec[i].coord[1]), &(point_vec[i].coord[2]), &(point_vec[i].coord[3]), &(point_vec[i].coord[4]), &(point_vec[i].coord[5]) );
            break;
        default:
            printf( "Error dimension specification!" );
    }

    // to conquer two or more same points
    for( unsigned i=0; i<point_no; ++i )
        point_vec[i].index = i;

    return;
}

// query in 2D space
void range2D_query( vector<KDNode> &kdtree, Point &q_point, unsigned &rank )
{
    unsigned nsplit = find_split( kdtree, 0, q_point.getCoord(2) );

    // brute-force search
    if( kdtree[nsplit].is_leaf )
    {
        assert( distance( kdtree[nsplit].pBegin, kdtree[nsplit].pEnd ) <= BRUTE_FORCE_NO );
        check_points_dominate( kdtree[nsplit].pBegin, kdtree[nsplit].pEnd, q_point, rank );
        return;
    }

    unsigned up_bound;

    { // binary search for fractional cascading
        BasicNode bn;
        bn.setPValIndex( q_point.getCoord(1), q_point.index );
        vBasicNodeIter up_Iter = upper_bound( (kdtree[nsplit].aux_vec).begin(), (kdtree[nsplit].aux_vec).end(), bn, cmp_BasicNode_val_woIndex );
        up_bound = distance( (kdtree[nsplit].aux_vec).begin(), up_Iter );
    }

    if( up_bound == 0 )
        return;
    else
        up_bound--;

    unsigned n_tmp = kdtree[nsplit].left_child;
    int up_tmp = kdtree[nsplit].aux_vec[up_bound].left_id;

    // search for left child
    while( !(kdtree[n_tmp].is_leaf) && up_tmp!=-1 )
    {
        rank += (kdtree[n_tmp].aux_vec[up_tmp].right_id+1);
        up_tmp = kdtree[n_tmp].aux_vec[up_tmp].left_id;
        n_tmp  = kdtree[n_tmp].left_child;
    }

    check_points_dominate( kdtree[n_tmp].pBegin, kdtree[n_tmp].pEnd, q_point, rank );

    up_tmp = kdtree[nsplit].aux_vec[up_bound].right_id;
    n_tmp  = kdtree[nsplit].right_child;

    // search for right child
    while( !(kdtree[n_tmp].is_leaf) && up_tmp!=-1 )
    {
        if( q_point.getCoord(2) >= kdtree[n_tmp].key )
        {
            rank += (kdtree[n_tmp].aux_vec[up_tmp].left_id+1);
            up_tmp = kdtree[n_tmp].aux_vec[up_tmp].right_id;
            n_tmp  = kdtree[n_tmp].right_child;
        }
        else
        {
            up_tmp = kdtree[n_tmp].aux_vec[up_tmp].left_id;
            n_tmp  = kdtree[n_tmp].left_child;
        }
    }

    check_points_dominate( kdtree[n_tmp].pBegin, kdtree[n_tmp].pEnd, q_point, rank );

    return;
}

// query in KD space, K>2
void rangeKD_query( vector<KDNode> &kdtree, Point &q_point, unsigned dim, unsigned &rank )
{
    unsigned nsplit = find_split( kdtree, 0, q_point.getCoord(dim) );

    // brute-force search
    if( kdtree[nsplit].is_leaf )
    {
        assert( distance( kdtree[nsplit].pBegin, kdtree[nsplit].pEnd ) <= BRUTE_FORCE_NO );
        check_points_dominate( kdtree[nsplit].pBegin, kdtree[nsplit].pEnd, q_point, rank );
        return;
    }

    unsigned n_tmp = kdtree[nsplit].left_child;

    // search for left child
    while( !(kdtree[n_tmp].is_leaf) )
    {
        unsigned n_right = kdtree[n_tmp].right_child;

        if( kdtree[n_right].is_leaf )
            check_points_dominate( kdtree[n_right].pBegin, kdtree[n_right].pEnd, q_point, rank );
        else
        {
            if( dim>3 )
                rangeKD_query( kdtree[n_right].aux_tree, q_point, dim-1, rank );
            else
                range2D_query( kdtree[n_right].aux_tree, q_point, rank );
        }

        n_tmp = kdtree[n_tmp].left_child;
    }

    check_points_dominate( kdtree[n_tmp].pBegin, kdtree[n_tmp].pEnd, q_point, rank );
    n_tmp = kdtree[nsplit].right_child;

    // search for right child
    while( !(kdtree[n_tmp].is_leaf) )
    {
        if( q_point.getCoord(dim) >= kdtree[n_tmp].key )
        {
            unsigned n_left = kdtree[n_tmp].left_child;

            if( kdtree[n_left].is_leaf )
                check_points_dominate( kdtree[n_left].pBegin, kdtree[n_left].pEnd, q_point, rank );
            else
            {
                if( dim>3 )
                    rangeKD_query( kdtree[n_left].aux_tree, q_point, dim-1, rank );
                else
                    range2D_query( kdtree[n_left].aux_tree, q_point, rank );
            }

            n_tmp = kdtree[n_tmp].right_child;
        }
        else
            n_tmp = kdtree[n_tmp].left_child;
    }

    check_points_dominate( kdtree[n_tmp].pBegin, kdtree[n_tmp].pEnd, q_point, rank );

    return;
}

void tree2D_gen( vector<KDNode> &kdtree, vector<Point> &point_vec, vPointIter pBIter, vPointIter pEIter )
{
    unsigned total_point_no = distance(pBIter, pEIter);

    kdtree.resize( 2*total_point_no-1 );

    kdtree[0].point_vec.resize( total_point_no, DIMENSION );
    unsigned p_count=0;

    // duplicate data to the root of new tree
    for( vPointIter tIter=pBIter; tIter!=pEIter; tIter++ )
    {
        for( unsigned i=0; i<DIMENSION; ++i )
            ((kdtree[0].point_vec)[p_count]).coord[i] = (*tIter).coord[i];

        ((kdtree[0].point_vec)[p_count]).index = (*tIter).index;

        p_count++;
    }

    pBIter = (kdtree[0].point_vec).begin();
    pEIter = (kdtree[0].point_vec).end();

    kdtree[0].setPBeginEnd( pBIter, pEIter ); // set tree root

    // no need to contruct tree
    if( distance(pBIter, pEIter) <= BRUTE_FORCE_NO )
    {
        kdtree[0].is_leaf = true;
        kdtree[0].aux_vec.resize( distance(pBIter, pEIter) );
        unsigned i=0;

        for( vPointIter tIter=pBIter; tIter!=pEIter; ++tIter )
            kdtree[0].aux_vec[i++].setPValIndex( (*tIter).getCoord(1), (*tIter).index );

        sort( kdtree[0].aux_vec.begin(), kdtree[0].aux_vec.end(), cmp_BasicNode_val );
        return;
    }

    KDCompClass functor( 2 ); // 2D compare
    vPointIter medianIter;
    unsigned tail=1; // size of current 2D tree

    // construct 2D and 1D tree
    for( unsigned i=0; i<tail; ++i )
    {
        KDNode &curNode = kdtree[i];
        unsigned cur_point_no = distance( curNode.pBegin, curNode.pEnd );

        // do brute force
        if( cur_point_no <= BRUTE_FORCE_NO )
        {
            curNode.is_leaf = true;
            curNode.key = (*(curNode.pBegin)).getCoord(2);
            curNode.aux_vec.resize( cur_point_no );
            unsigned j=0;

            for( vPointIter tIter=curNode.pBegin; tIter!=curNode.pEnd; ++tIter )
                curNode.aux_vec[j++].setPValIndex( (*tIter).getCoord(1), (*tIter).index );

            sort( curNode.aux_vec.begin(), curNode.aux_vec.end(), cmp_BasicNode_val );

            continue;
        }

        curNode.is_leaf = false;
        medianIter = curNode.pBegin + cur_point_no/2;
        nth_element( curNode.pBegin, medianIter, curNode.pEnd, functor ); // partition & find median

        curNode.left_child = tail;
        kdtree[tail++].setPBeginEnd( curNode.pBegin, medianIter );
        curNode.right_child = tail;
        kdtree[tail++].setPBeginEnd( medianIter, curNode.pEnd );

        curNode.key = medianIter->getCoord(2);
        (curNode.aux_vec).resize( cur_point_no );
        unsigned j=0;

        // get max and min & insert aux_vec
        for( vPointIter tIter=curNode.pBegin; tIter!=curNode.pEnd; tIter++ )
            (curNode.aux_vec[j++]).setPValIndex( (*tIter).getCoord(1), (*tIter).index );

        // sort for 1D
        sort( curNode.aux_vec.begin(), curNode.aux_vec.end(), cmp_BasicNode_val );
    }

    // add fractional cascading
    for( unsigned i=0; i<tail; ++i )
    {
        if( kdtree[i].is_leaf )
            continue;

        KDNode &curNode   = kdtree[i];
        KDNode &leftNode  = kdtree[curNode.left_child];
        KDNode &rightNode = kdtree[curNode.right_child];
        int left_count=0, right_count=0;

        for( unsigned j=0; j<curNode.aux_vec.size(); ++j )
        {
            // similar to the merge part of merge_sort
            if( (left_count<leftNode.aux_vec.size()) && (curNode.aux_vec[j].p_index==leftNode.aux_vec[left_count].p_index) )
            {
                curNode.aux_vec[j].left_id  = left_count++;
                curNode.aux_vec[j].right_id = right_count-1;
            }
            else
            {
                assert( curNode.aux_vec[j].p_index==rightNode.aux_vec[right_count].p_index );
                curNode.aux_vec[j].right_id = right_count++;
                curNode.aux_vec[j].left_id  = left_count-1;
            }
        }
    }

    return;
}

// used for dim>=3 tree construction
void treeKD_gen( vector<KDNode> &kdtree, unsigned dim, vector<Point> &point_vec, vPointIter pBIter, vPointIter pEIter )
{
    unsigned total_point_no = distance(pBIter, pEIter);

    kdtree.resize( 2*total_point_no-1 );
    kdtree[0].point_vec.resize( total_point_no, DIMENSION );
    unsigned p_count=0;

    // duplicate data to the root of new tree
    for( vPointIter tIter=pBIter; tIter!=pEIter; tIter++ )
    {
        for( unsigned i=0; i<DIMENSION; ++i )
            ((kdtree[0].point_vec)[p_count]).coord[i] = (*tIter).coord[i];

        ((kdtree[0].point_vec)[p_count]).index = (*tIter).index;

        p_count++;
    }

    pBIter = (kdtree[0].point_vec).begin();
    pEIter = (kdtree[0].point_vec).end();

    kdtree[0].setPBeginEnd( pBIter, pEIter ); // setup tree root

    if( distance(pBIter, pEIter) <= BRUTE_FORCE_NO )
    {
        kdtree[0].is_leaf = true;
        return;
    }

    KDCompClass functor( dim );
    vPointIter medianIter;
    unsigned tail=1; // size of current KD tree

    for( unsigned i=0; i<tail; ++i )
    {
        KDNode &curNode = kdtree[i];
        unsigned cur_point_no = distance( curNode.pBegin, curNode.pEnd );

        if( cur_point_no <= BRUTE_FORCE_NO )
        {
            curNode.is_leaf = true;
            continue;
        }

        curNode.is_leaf = false;

        if( dim > 3 )
            treeKD_gen( curNode.aux_tree, dim-1, (curNode.point_vec), curNode.pBegin, curNode.pEnd );
        else // 3D to 2D
            tree2D_gen( curNode.aux_tree, (curNode.point_vec), curNode.pBegin, curNode.pEnd );

        medianIter = curNode.pBegin + cur_point_no/2;
        nth_element( curNode.pBegin, medianIter, curNode.pEnd, functor ); // partition & find median
        curNode.key = medianIter->getCoord(dim);

        // initialize left child and right child
        curNode.left_child = tail;
        kdtree[tail++].setPBeginEnd( curNode.pBegin, medianIter );

        curNode.right_child = tail;
        kdtree[tail++].setPBeginEnd( medianIter, curNode.pEnd );
    }

    return;
}

int main( int argc, char **argv )
{
    //case_gen( atoi(argv[1]), atoi(argv[2]), atoi(argv[3]) ); // dimension, # points, # queries
    //freopen( "case0.in", "r", stdin );

    unsigned POINT_NO; // constants
    vector<Point> point_vec;

    parse_inputs( DIMENSION, POINT_NO, point_vec ); // parse input data
    assert( distance( point_vec.begin(), point_vec.end() ) == POINT_NO );

    vector<KDNode> kdtree;
    Point q_point(DIMENSION);
    q_point.index = POINT_NO; // large than the existing points

    unsigned rank=0;

    // range query
    if( DIMENSION>=3 )
    {
        treeKD_gen( kdtree, DIMENSION, point_vec, point_vec.begin(), point_vec.end() );

        while( scanf( "%lf", &(q_point.coord[0]) ) != EOF )
        {
            for( unsigned i=1; i<DIMENSION; ++i )
                scanf( "%lf", &(q_point.coord[i]) );

            rangeKD_query( kdtree, q_point, DIMENSION, rank );
            //	    assert( rank == checker( point_vec, q_point, DIMENSION ) ); // check by brute force
            printf( "%u\n", rank );
            rank=0;
        }
    }
    else // 2D tree
    {
        tree2D_gen( kdtree, point_vec, point_vec.begin(), point_vec.end() );

        while( scanf( "%lf", &(q_point.coord[0]) ) != EOF )
        {
            scanf( "%lf", &(q_point.coord[1]) );
            range2D_query( kdtree, q_point, rank );
            //	    assert( rank == checker( point_vec, q_point, DIMENSION ) ); // check by brute force
            printf( "%u\n", rank );
            rank=0;
        }
    }

    delete [] dim_conv;

    return 0;
}

void case_gen( unsigned dim, unsigned point_no, unsigned query_no )
{
    ofstream outf( "case0.in", ios::out );
    srand ( time(NULL) ); // initialize random seed

    outf << dim << " " << point_no << endl;

    for( unsigned i=0; i<point_no; ++i )
    {
        for( unsigned j=0; j<dim; ++j )
            outf << (rand()%1000) << " ";

        outf << endl;
    }

    for( unsigned i=0; i<query_no; ++i )
    {
        for( unsigned j=0; j<dim; ++j )
            outf << (rand()%1001) << " ";

        outf << endl;
    }

    outf.close();

    return;
}

unsigned checker( vector<Point> &point_vec, Point &q_point, unsigned dim )
{
    unsigned count = 0;

    for( unsigned i=0; i<point_vec.size(); ++i )
    {
        bool is_dominated = true;

        for( unsigned j=0; j<dim; ++j )
        {
            if( point_vec[i].coord[j] > q_point.coord[j] )
            {
                is_dominated = false;
                break;
            }
        }

        if( is_dominated )
            count++;
    }

    return count;
}

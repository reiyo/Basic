/*************************************************************************************
 
  Input: set of n (horizontal) intervals, k commands doing insertion or deletion
  Output: length of the union of the set of intervals in current segment tree
  Time complexity: O(n*logn+k*logn), where n is # of intervals, k is # of commands
                   Sorting of endpoints -> O(n*logn)
                   Segment tree construction -> O(n)
                   Tree update after each insertion or deletion -> O(logn)
  Method used: Segment tree construction

 ************************************************************************************/

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

#define NDEBUG
#define PRECISION 0.000001

// --------------- class definition --------------- //

// endpoint of an interval
class endpoint
{
    public:
        void init( double v, int id, bool b ) { value = v; interval_id = id; is_left = b; }

        double value;       // endpoint coordinate, real value
        int    interval_id; // id of interval_set
        bool   is_left;     // true if left endpoint, false if right endpoint
};

class interval
{
    public:
        interval(void) { left_x=0; right_x=0; is_zeroLength = false; }

        double   rLeft_x, rRight_x; // real left x, real right x 
        double   rLength;           // rRight_x minus rLeft_x
        bool     is_zeroLength;
        unsigned left_x, right_x;   // left x and right x after transformation
};

// tree node of the segment tree
class node
{
    public:
        node(void) 
        { 
            key            = 0;
            leftChild_id   = 0; 
            rightChild_id  = 0; 
            intervalCount  = 0; 
            subtreeRLength = 0.0;
        }

        unsigned key;
        double   rLeft_x, rRight_x; // real left x, real right x
        double   rLength;           // rRight_x-rLeft_x
        unsigned left_x, right_x; 
        unsigned parent_id;    // 0 if it is root
        unsigned leftChild_id, rightChild_id;

        unsigned intervalCount;
        double subtreeRLength; // interval length of the subtree
};

// --------------- function declaration --------------- //

bool cmp_small_epVal( const endpoint &e1, const endpoint &e2 );

void tree_update( unsigned cur_id, std::vector<node> &segment_tree, double interval_length );

void interval_deletion( unsigned target_left_x, unsigned target_right_x, unsigned cur_id, std::vector<node> &segment_tree );

void interval_insertion( unsigned target_left_x, unsigned target_right_x, unsigned cur_id, std::vector<node> &segment_tree );

void report_result( node &root );

void tree_construct( std::vector<node> &segment_tree );

using std::vector;

// --------------- function definition --------------- //

bool cmp_small_epVal( const endpoint &e1, const endpoint &e2 )
{
    return ( e1.value < e2.value );
}

// update towards root after insertion
void tree_update( unsigned cur_id, vector<node> &segment_tree, double interval_length )
{
    // has been inserted or hit the root
    if( segment_tree[cur_id].intervalCount > 0 || cur_id == 1 )
    {
        segment_tree[cur_id].subtreeRLength += interval_length;
        return;
    }
    else
    {
        segment_tree[cur_id].subtreeRLength += interval_length;
        tree_update( segment_tree[cur_id].parent_id, segment_tree, interval_length );
    }

    return;
}

void interval_deletion( unsigned target_left_x, unsigned target_right_x, unsigned cur_id, vector<node> &segment_tree )
{
    if( (target_left_x <= segment_tree[cur_id].left_x) && (target_right_x >= segment_tree[cur_id].right_x) )
    {
        assert( segment_tree[cur_id].intervalCount > 0 );
        (segment_tree[cur_id].intervalCount)--;

        if( segment_tree[cur_id].intervalCount == 0 && cur_id != 1 )
            tree_update( segment_tree[cur_id].parent_id, segment_tree, segment_tree[cur_id].subtreeRLength-segment_tree[cur_id].rLength );

        return;
    }

    if( target_left_x < segment_tree[cur_id].key )
        interval_deletion( target_left_x, target_right_x, segment_tree[cur_id].leftChild_id, segment_tree );

    if( target_right_x > segment_tree[cur_id].key )
        interval_deletion( target_left_x, target_right_x, segment_tree[cur_id].rightChild_id, segment_tree );

    return;
}

// insert an interval and do update towards root
void interval_insertion( unsigned target_left_x, unsigned target_right_x, unsigned cur_id, vector<node> &segment_tree )
{
    if( (target_left_x <= segment_tree[cur_id].left_x) && (target_right_x >= segment_tree[cur_id].right_x) )
    {
        if( segment_tree[cur_id].intervalCount == 0 && cur_id != 1 )
            tree_update( segment_tree[cur_id].parent_id, segment_tree, segment_tree[cur_id].rLength-segment_tree[cur_id].subtreeRLength );

        (segment_tree[cur_id].intervalCount)++;
        return;
    }

    if( target_left_x < segment_tree[cur_id].key )
        interval_insertion( target_left_x, target_right_x, segment_tree[cur_id].leftChild_id, segment_tree );

    if( target_right_x > segment_tree[cur_id].key )
        interval_insertion( target_left_x, target_right_x, segment_tree[cur_id].rightChild_id, segment_tree );

    return;
}

void report_result( node &root )
{
    if( root.intervalCount == 0 )
    {
        if( fabs(root.subtreeRLength) > PRECISION )
            printf( "%lf\n", root.subtreeRLength );
        else
            printf( "0.000000\n" );
    }
    else
    {
        if( fabs(root.rLength) > PRECISION )
            printf( "%lf\n", root.rLength );
        else
            printf( "0.000000\n" );
    }
}

// construct segment tree
void tree_construct( vector<node> &segment_tree, vector<double> &map_to_real )
{
    unsigned key, tree_size, right_id; // temporary variables

    for( unsigned i=1; i<segment_tree.size(); ++i )
    {
        if( (segment_tree[i].right_x - segment_tree[i].left_x) <= 1 )
        {
            // leaf does not need to calculate key value, default is 0
            assert( (segment_tree[i].right_x - segment_tree[i].left_x) == 1 );
            assert( segment_tree[i].leftChild_id == 0 && segment_tree[i].rightChild_id == 0 );
            continue;
        }

        key = (segment_tree[i].left_x + segment_tree[i].right_x)/2;
        segment_tree[i].key = key;

        tree_size = segment_tree.size();
        segment_tree.resize( tree_size+2 );
        right_id = tree_size+1;

        segment_tree[i].leftChild_id  = tree_size;
        segment_tree[i].rightChild_id = right_id;

        // construct left sub-tree
        segment_tree[tree_size].rLeft_x   = map_to_real[segment_tree[i].left_x];
        segment_tree[tree_size].rRight_x  = map_to_real[key];
        segment_tree[tree_size].rLength   = segment_tree[tree_size].rRight_x - segment_tree[tree_size].rLeft_x;
        segment_tree[tree_size].left_x    = segment_tree[i].left_x;
        segment_tree[tree_size].right_x   = key;
        segment_tree[tree_size].parent_id = i;

        // construct right sub-tree
        segment_tree[right_id].rLeft_x   = map_to_real[key];
        segment_tree[right_id].rRight_x  = map_to_real[segment_tree[i].right_x];
        segment_tree[right_id].rLength   = segment_tree[right_id].rRight_x - segment_tree[right_id].rLeft_x;
        segment_tree[right_id].left_x    = key;
        segment_tree[right_id].right_x   = segment_tree[i].right_x;
        segment_tree[right_id].parent_id = i;
    }

    return;
}

// --------------- main function --------------- //

int main( int argc, char **argv )
{
    // freopen( "../interval_cases/case1.in", "r", stdin );

    unsigned interval_no;

    scanf( "%u", &interval_no );

    vector<interval> interval_set; // all intervals (use +1 offset)
    interval_set.resize( interval_no+1 );

    vector<endpoint> endpoint_set; // all endpoints (use no offset)
    endpoint_set.resize( interval_no*2 );

    { // parse input intervals
        unsigned e_id = 0;
        interval_no+=1;

        for( unsigned i=1; i<interval_no; ++i )
        {
            scanf( "%lf", &(interval_set[i].rLeft_x) );
            scanf( "%lf", &(interval_set[i].rRight_x) );
            interval_set[i].rLength = interval_set[i].rRight_x - interval_set[i].rLeft_x;
            assert( interval_set[i].rLength >= 0.0 );

            if( fabs( interval_set[i].rLength ) < PRECISION )
                interval_set[i].is_zeroLength = true;

            endpoint_set[e_id++].init( interval_set[i].rLeft_x, i, true );
            endpoint_set[e_id++].init( interval_set[i].rRight_x, i, false );
        }

        assert( e_id == endpoint_set.size() );
        interval_no-=1; // recover
    }

    unsigned global_max_x; // max x of the mapped space, min x is 1
    vector<double> map_to_real; // mapped space to real value, (use +1 offset)
    map_to_real.resize( interval_no*2+1 );

    { // map the real space to integer space counting from integer 1
        // sort in ascending order
        std::sort( endpoint_set.begin(), endpoint_set.end(), cmp_small_epVal );

        unsigned mapped_val = 1;
        unsigned tmp_id = endpoint_set[0].interval_id;
        map_to_real[1] = endpoint_set[0].value;

        if( endpoint_set[0].is_left )
            interval_set[tmp_id].left_x  = mapped_val;
        else
            interval_set[tmp_id].right_x = mapped_val;

        unsigned endpoint_no = endpoint_set.size();

        for( unsigned i=1; i<endpoint_no; ++i )
        {
            // if two real values are the same, they should map to the same integer
            // otherwise, just map to next continuous integer
            if( fabs( endpoint_set[i].value - endpoint_set[i-1].value ) > PRECISION )
            {
                mapped_val++;
                map_to_real[mapped_val] = endpoint_set[i].value;
            }

            tmp_id = endpoint_set[i].interval_id;

            if( endpoint_set[i].is_left )
                interval_set[tmp_id].left_x  = mapped_val;
            else
                interval_set[tmp_id].right_x = mapped_val;
        }

#ifndef NDEBUG
        printf( "debug mapping..." );

        for( unsigned i=1; i<interval_no+1; ++i )
            assert( interval_set[i].left_x > 0 && interval_set[i].right_x > 0 );

        printf( "done!\n" );
#endif 
        global_max_x = mapped_val;
    }

    vector<node> segment_tree; // segment tree (use +1 offset)

    { // construct segment tree
        segment_tree.resize(2);
        segment_tree[1].rLeft_x   = map_to_real[1];
        segment_tree[1].rRight_x  = map_to_real[global_max_x];
        segment_tree[1].rLength   = segment_tree[1].rRight_x - segment_tree[1].rLeft_x;
        segment_tree[1].left_x    = 1;
        segment_tree[1].right_x   = global_max_x;
        segment_tree[1].parent_id = 0;
        tree_construct( segment_tree, map_to_real ); 
    }

    vector<unsigned> interval_count; // record the interval count being inserted (use +1 offset)
    interval_count.resize( interval_no+1, 0 );

    { // read the remainder input command to do insertion or deletion
        char instruction;   // 'D' or 'I'    
        char command[20];   // ex. D3 - delete interval 3
        unsigned target_id; // target interval id
        unsigned target_left_x, target_right_x;

        while( scanf( "%s", command ) != EOF )
        {
            // printf( "command is %s\n", command );

            instruction = command[0];
            command[0] = '0';
            target_id = static_cast<unsigned>(atoi(command));

            if( instruction == 'I' )
            {
                if( (interval_count[target_id]==0) && !(interval_set[target_id].is_zeroLength) )
                {
                    target_left_x  = interval_set[target_id].left_x;
                    target_right_x = interval_set[target_id].right_x;
                    interval_insertion( target_left_x, target_right_x, 1, segment_tree );
                }

                interval_count[target_id]++;
                report_result( segment_tree[1] );
            }
            else
            {
                assert( instruction == 'D' );

                if( interval_count[target_id]==1 )
                {
                    target_left_x  = interval_set[target_id].left_x;
                    target_right_x = interval_set[target_id].right_x;
                    interval_deletion( target_left_x, target_right_x, 1, segment_tree );
                    interval_count[target_id]=0;
                    report_result( segment_tree[1] );
                }
                else if( interval_count[target_id]>1 )
                {
                    interval_count[target_id]--;
                    report_result( segment_tree[1] );
                }
                else
                    printf( "S%u not inserted yet\n", target_id );
            }
        }
    }

    return 0;
}

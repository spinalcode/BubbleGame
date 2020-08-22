const uint8_t maxBall[]={ // highest ball number for each level
 4,5,5,6,6,6,6,
};

const uint8_t levelData[][88]={
{
1,1,2,2,3,3,4,4,
1,1,2,2,3,3,4,0,
3,3,4,4,1,1,2,2,
3,4,4,1,1,2,2,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
},{
0,2,2,2,1,1,1,0,
2,0,0,0,0,0,1,0,
0,2,0,0,0,0,1,0,
2,3,4,5,3,4,1,0,
0,4,5,3,4,5,3,0,
0,3,4,5,3,4,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
},{
0,0,0,2,4,3,0,0,
 0,0,4,3,5,2,0,0,
0,0,0,5,7,4,0,0,
 0,3,4,2,0,0,0,0,
0,2,5,3,4,0,0,0,
 0,4,7,5,0,0,0,0,
0,0,0,3,4,2,0,0,
 0,0,2,5,3,4,0,0,
0,0,0,4,7,5,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
},{
0,0,1,0,0,1,0,0,
 0,0,1,5,1,0,0,0,
0,0,0,5,3,0,0,0,
 0,0,5,3,5,0,0,0,
0,0,0,3,6,0,0,0,
 0,0,4,6,4,0,0,0,
0,0,0,6,4,0,0,0,
 0,0,0,4,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
},{
1,2,3,4,6,1,2,8,
 0,0,0,0,2,0,0,0,
0,0,0,2,2,0,0,0,
 0,0,2,0,0,0,0,0,
8,4,3,2,1,2,3,4,
 0,0,0,0,4,0,0,0,
0,0,0,4,4,0,0,0,
 0,0,4,0,0,0,0,0,
1,2,3,4,6,1,2,8,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
},{
4,0,0,0,0,0,0,4,
 6,1,3,2,6,1,3,0,
2,0,0,0,0,0,0,2,
 1,2,3,4,1,2,3,0,
0,0,0,0,4,0,0,0,
 0,0,0,0,5,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
},{
1,1,0,0,0,0,6,6,
 5,6,0,0,0,1,2,0,
3,5,0,0,0,0,2,4,
 3,0,0,0,0,0,4,0,
5,6,0,0,0,0,1,2,
 5,0,0,0,0,0,2,0,
1,5,0,0,0,0,2,6,
 1,6,0,0,0,1,6,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
},{
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
},{
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
},{
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
},{
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
},{
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
},{
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
},{
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
}  
};


const uint8_t tileMask[]={
    0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
    2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
    3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,
    0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
    2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
    3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,
    0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
    2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
    3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,
    0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
    2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
    3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,
    0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
    2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
    3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,
    0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
    2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
    3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,
};


const int8_t wiggle[]={
0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,7,
8,8,9,9,10,10,10,11,11,11,12,12,12,12,13,13,13,
13,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
14,14,14,14,14,13,13,13,13,12,12,12,12,11,11,11,10,
10,10,9,9,8,8,7,7,7,6,6,5,5,4,4,3,3,
2,2,1,1,0,0,0,-1,-1,-2,-2,-3,-3,-4,-4,-5,-5,
-6,-6,-7,-7,-7,-8,-8,-9,-9,-10,-10,-10,-11,-11,-11,-12,-12,
-12,-12,-13,-13,-13,-13,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,
-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-13,-13,-13,-13,-12,-12,-12,
-12,-11,-11,-11,-10,-10,-10,-9,-9,-8,-8,-7,-7,-7,-6,-6,-5,
-5,-4,-4,-3,-3,-2,-2,-1,-1,0,0,0,1,1,2,2,3,
3,4,4,5,5,6,6,7,7,7,8,8,9,9,10,10,10,
11,11,11,12,12,12,12,13,13,13,13,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,13,13,
13,13,12,12,12,12,11,11,11,10,10,10,9,9,8,8,7,
7,7,6,6,5,5,4,4,3,3,2,2,1,1,0,0,0,
-1,-1,-2,-2,-3,-3,-4,-4,-5,-5,-6,-6,-7,-7,-7,-8,-8,
-9,-9,-10,-10,-10,-11,-11,-11,-12,-12,-12,-12,-13,-13,-13,-13,-14,
-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,
-14,-14,-14,-13,-13,-13,-13,-12,-12,-12,-12,-11,-11,-11,-10,-10,-10,
-9,-9,-8,-8,-7,-7,-7,-6,-6,-5,-5,-4,-4,-3,-3,-2,-2,
-1,-1,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,
7,7,7,8,8,9,9,10,10,10,11,11,11,12,12,12,12,
13,13,13,13,14,14,14,14,14,14,14,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,13,13,13,13,12,12,12,12,11,
11,11,10,10,10,9,9,8,8,7,7,7,6,6,5,5,4,
4,3,3,2,2,1,1,0,0,0,-1,-1,-2,-2,-3,-3,-4,
-4,-5,-5,-6,-6,-7,-7,-7,-8,-8,-9,-9,-10,-10,-10,-11,-11,
-11,-12,-12,-12,-12,-13,-13,-13,-13,-14,-14,-14,-14,-14,-14,-14,-14,
-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-13,-13,-13,-13,
-12,-12,-12,-12,-11,-11,-11,-10,-10,-10,-9,-9,-8,-8,-7,-7,-7,
-6,-6,-5,-5,-4,-4,-3,-3,-2,
};

// table for (y%8)*8 -> background tile rendering
const uint8_t ymod8[]={
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56,
0,8,16,24,32,40,48,56
};

// y%6*7+2
const uint8_t ymod6_7[]={
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7,14,21,28,35,
0,7
};
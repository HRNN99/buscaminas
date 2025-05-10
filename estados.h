#ifndef ESTADOS_H
#define ESTADOS_H

const int square1[10][10] =
{
    {B,B,B,B,B,B,B,B,B,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GF,GF,GF,GF,GF,GF,GF,GF,GF}
};

const int square2[10][10] =
{
    {GF,GF,GF,GF,GF,GF,GF,GF,GF,GF},
    {GF,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {GF,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {GF,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {GF,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {GF,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {GF,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {GF,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {GF,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {GF,GF,GF,GF,GF,GF,GF,GF,GF,GF}
};

const int mine[10][10] =
{
    {B,B,B,B,B,B,B,B,B,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,N,N,GS,GS,GS,GF},
    {B,GS,GS,N,N,N,N,GS,GS,GF},
    {B,GS,N,N,N,N,B,N,GS,GF},
    {B,GS,N,N,N,N,N,N,GS,GF},
    {B,GS,GS,N,N,N,N,GS,GS,GF},
    {B,GS,GS,GS,N,N,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GF,GF,GF,GF,GF,GF,GF,GF,GF}
};

const int one[10][10] =
{
    {B,B,B,B,B,B,B,B,B,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,A,A,GS,GS,GS,GF},
    {B,GS,GS,A,A,A,GS,GS,GS,GF},
    {B,GS,GS,GS,A,A,GS,GS,GS,GF},
    {B,GS,GS,GS,A,A,GS,GS,GS,GF},
    {B,GS,GS,GS,A,A,GS,GS,GS,GF},
    {B,GS,GS,A,A,A,A,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GF,GF,GF,GF,GF,GF,GF,GF,GF}
};

const int two[10][10] =
{
    {B,B,B,B,B,B,B,B,B,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,V,V,GS,GS,GS,GF},
    {B,GS,GS,V,GS,GS,V,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,V,GS,GS,GF},
    {B,GS,GS,GS,GS,V,GS,GS,GS,GF},
    {B,GS,GS,GS,V,GS,GS,GS,GS,GF},
    {B,GS,GS,V,V,V,V,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GF,GF,GF,GF,GF,GF,GF,GF,GF}
};

const int three[10][10] =
{
    {B,B,B,B,B,B,B,B,B,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,R,R,R,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,R,GS,GS,GF},
    {B,GS,GS,GS,R,R,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,R,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,R,GS,GS,GF},
    {B,GS,GS,R,R,R,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GF,GF,GF,GF,GF,GF,GF,GF,GF}
};

const int flag[10][10] =
{
    {B,B,B,B,B,B,B,B,B,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,R,N,GS,GS,GS,GF},
    {B,GS,GS,R,R,N,GS,GS,GS,GF},
    {B,GS,R,R,R,N,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,N,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,N,GS,GS,GS,GF},
    {B,GS,GS,N,N,N,N,N,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GF,GF,GF,GF,GF,GF,GF,GF,GF}
};

#endif // ESTADOS_H

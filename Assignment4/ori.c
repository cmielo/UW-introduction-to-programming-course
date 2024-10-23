//Author: Gracjan Barski

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h> 
#define EPS 1e-10

typedef enum representation {Rectangle, Circle, Fold} representation;

//point on the plane
typedef struct point {
    double x, y;
} point;

//types of sheets
//----------------------------------------------------------------
typedef struct rectangle { //rectangle sheet of paper
    point p1, p2;
} rectangle;

typedef struct circle { //circular sheet of paper
    point p1;
    double r;
} circle;

typedef struct fold { //sheet that was folded
    int which_sheet; //this field indicates which sheet was folded
    point p1, p2;
} fold;
//----------------------------------------------------------------

//generalization
typedef union sheet {
    rectangle r;
    circle c;
    fold f;
} sheet;

//general struct with indication which representation is used
typedef struct paper {
    representation rep;
    sheet sh;
} paper;

//to conveniently represent a query
typedef struct query {
    int which_sheet;
    point p;
} query;

//to conveniently represent a line (ax + by + c = 0)
typedef struct line {
    double a, b, c;
} line;

double square(double x) {
    return x * x;
}

bool is_zero(double x) { 
    return (fabs(x) <= EPS); 
}

paper* read_papers(int n) {
    paper* res = (paper*) malloc((size_t) n * sizeof(paper));
    if(res == NULL) //malloc failed
        return NULL;
    char rep;
    for(int i = 0; i < n; i++) {
        scanf(" %c", &rep);
        switch(rep) {
            case 'P':
                res[i].rep = Rectangle;
                scanf(" %lf %lf %lf %lf", &res[i].sh.r.p1.x, &res[i].sh.r.p1.y, &res[i].sh.r.p2.x, &res[i].sh.r.p2.y);
                break;

            case 'K':
                res[i].rep = Circle;
                scanf(" %lf %lf %lf", &res[i].sh.c.p1.x, &res[i].sh.c.p1.y, &res[i].sh.c.r);
                break;

            case 'Z':
                res[i].rep = Fold;
                scanf(" %d %lf %lf %lf %lf", &res[i].sh.f.which_sheet, &res[i].sh.f.p1.x, &res[i].sh.f.p1.y, 
                                            &res[i].sh.f.p2.x, &res[i].sh.f.p2.y);
                res[i].sh.f.which_sheet--;
                break;

            default:
                printf("Invalid input");
                free(res);
                return NULL;
        }
    }
    return res;
}

query* read_queries(int q) {
    query* res = (query*) malloc((size_t) q * sizeof(query));
    if(res == NULL) //malloc failed
        return NULL;

    for(int i = 0; i < q; i++) {
        scanf(" %d %lf %lf", &res[i].which_sheet, &res[i].p.x, &res[i].p.y);
        res[i].which_sheet--;
    }
    return res;
}

//returns a line running through two points which have to be distinct
line make_line(point p1, point p2) {
    double a = p2.y - p1.y;
    double b = p1.x - p2.x;
    double c = p1.y * (p2.x - p1.x) - p1.x * (p2.y - p1.y);
    line res = {.a = a, .b = b, .c = c};
    return res;
}

//to_reflect is the point that is supposed to be mirrored; p1, p2 describe the fold line
point find_point_reflection(point to_reflect, point p1, point p2) {
    line l = make_line(p1, p2);
    double p = to_reflect.x;
    double q = to_reflect.y;
    //formula for (p,q) point reflection over a line ax + by + c = 0
    point res = {
        .x = 2 * (square(l.b)*p - l.a * (l.b * q + l.c)) / (square(l.a) + square(l.b)) - p,
        .y = 2 * (square(l.a)*q - l.b * (l.a * p + l.c)) / (square(l.a) + square(l.b)) - q
    };
    return res;
}

bool is_on_right(point p, point p1, point p2) {
    double a = p.x, b = p.y, x1 = p1.x, y1 = p1.y, x2 = p2.x, y2 = p2.y; //extra variables for simplicity
    //compute the value of \Vec(p - p1) \times \Vec(p2 - p1)
    double cross_product = (a - x1) * (y2 - y1) - (b - y1) * (x2 - x1);
    //if it is more than 0, then it is on the right side
    return (cross_product > EPS);
}

bool is_on_fold_line(point p, point p1, point p2) {
    line l = make_line(p1, p2);
    //check if point satisfies the equation of line
    return is_zero(l.a * p.x + l.b * p.y + l.c);
}

//settle how many layers in a rectangle
long long in_rectangle(paper* rect, point p) {
    if(rect->rep != Rectangle) //Wrong call
        return INT_MIN;
    return (
        (rect->sh.r.p1.x < p.x || is_zero(rect->sh.r.p1.x - p.x)) //is_zero() necessary to check for edge cases
        && (p.x < rect->sh.r.p2.x || is_zero(p.x - rect->sh.r.p2.x)) 
        && (rect->sh.r.p1.y < p.y || is_zero(rect->sh.r.p1.y - p.y)) 
        && (p.y < rect->sh.r.p2.y || is_zero(p.y - rect->sh.r.p2.y))
    );
}

//settle how many layers in a circle
long long in_circle(paper* circ, point p) {
    if(circ->rep != Circle) //Wrong call
        return INT_MIN;
    double dist = square(circ->sh.c.p1.x - p.x) + square(circ->sh.c.p1.y - p.y); 
    return (dist < square(circ->sh.c.r) || is_zero(dist - square(circ->sh.c.r)));
}

//settle how many layers in a folded paper
long long in_fold(paper* fld, paper* P, point p) {
    if(fld->rep == Rectangle) 
    { //the paper was unfolded recursively to the rectangle
        return in_rectangle(fld, p);
    } 
    else if(fld->rep == Circle) 
    { //the paper was unfolded recursively to the circle
        return in_circle(fld, p);
    } 
    else 
    { //the paper is folded
        int sheet_number = fld->sh.f.which_sheet;
        point p1 = fld->sh.f.p1;
        point p2 = fld->sh.f.p2;
        if(is_on_right(p, p1, p2)) //pierce point is on the right of the fold line, so we are guaranteed to have 0 layers
            return 0;
        else if(is_on_fold_line(p, p1, p2)) //pierce point is exactly on the fold line, so unfold and repeat the query
            return in_fold(P + sheet_number, P, p);
        else
            //pierce point is on the left of the fold line, so unfold and run two queries
            return in_fold(P + sheet_number, P, p) + in_fold(P + sheet_number, P, find_point_reflection(p, p1, p2));
    }
} 

void run_query(paper* P, query* q) {
    paper* current = P + (q->which_sheet);
    switch(current->rep) {
        case Rectangle:
            printf("%lli\n", in_rectangle(current, q->p));
            break;
        case Circle:
            printf("%lli\n", in_circle(current, q->p));
            break;
        case Fold:
            printf("%lli\n", in_fold(current, P, q->p));
    }
}


void solve() {
    int n, q;
    scanf(" %d %d", &n, &q);

    paper* P = read_papers(n);
    if(P == NULL)
        return;

    query* Q = read_queries(q);
    if(Q == NULL)
        return;

    for(int i = 0; i < q; i++) {
        run_query(P, Q + i);
    }

    free(P);
    free(Q);
}


int main() {
    solve();
    return 0;
}
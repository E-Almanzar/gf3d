//Body is what contacts one thing to another
/*In body.h
list.h, primitives.h
/
#include "body.h"
#include "gfc_primitives.h"
typedef struct Body_S{

    GFC_TextLine    name; 
    GFC_Vector3D    pos;
    GFC_Vector3D    vel;
    GFC_List        *volumes;
    GFC_Vector3D    stepPosition;
}Body;

void (*onCollide)(struct Body_S *self, struct Body_S *other, void *data); 
void *data;

typedef void body_collide_func(Body *self, Body *other, void *data);

Uint8 body_test_prims(GFC_Primitive *a, GFC_Primitive *b){
    int retval, i, j, c, d;

    //First 
    GFC_Box a1, b1;
    a1 = (GFC_Box)*a;
    retval = gfc_box_overlap(, );
    

}

Uint8 body_test_body(Body *a, Body *b){
    int i,c,j,d;
    GFC_Primitive *ap, *bp;
    GFC_Primitive apTest, bpTest;
    //WTH is step position

    //You get the number of volumes it has (i think 1 for now)
    c = gfc_list_count(a->volumes);
    for (i = 0; i < c; i++){
        //Then for each of those, you capture them as the one you wanna test
        //Not true actually, you test them moved based on how far they go in their step
        //So in players case its their velocity?
        ap = gfc_list_nth(a->volumes,i);
        apTest = gfc_primitive_offset(*ap, a->stepPosition);
        d = gfc_list_count(b->volumes);
        for(int j = 0;j < d;j++){
            //For every one of B, test that
            bp = gfc_list_nth(b->volumes,i);
            bpTest = gfc_primitive_offset(*bp, b->stepPosition);
            //TODO test if the two primitives collided
            /*
                How the hell do you test if two primitives intersected?
                Do you need to check every edge in a point in triangle?
            /
            }
    }
            
		
}*/
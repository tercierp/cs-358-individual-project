$fn=128;
eps=0.01;
driftstepper=4.5;

//color("red") translate([41.5-driftstepper,-12.5/2-5,34]) rotate([-90,180,0]) import("stepper.stl", convexity=3);


module trou(d1,ep) {
    cylinder(h=ep+2*eps,d=d1,center=true);
}

module plot(ep) {
    translate([10.04,-ep,9.95]) difference() {
        union() {
             
            translate([0,-1,0])cube([14,5.5,5]);
            translate([7-3.5/2,-5,0]) cube([3.5,5+ep,5]);
            translate([7,-5,5/2]) rotate([90,0,90]) cylinder(h=3.5,d=5,center=true);
        
        
        }
        translate([7,-5,5/2]) rotate([90,0,90]) cylinder(h=3.5+2*eps,d=3.3,center=true);
    }
}

module plaque() {
    la = 57.5;
    lo = 39;
    ep = 2.5;
    av = 18;
    ls = 5;
    es = 5;
    motordeep=2;
    difference() {
        union() {
            cube([la-driftstepper,ep,lo]);
            translate([25.5-driftstepper,-av+0.5,lo-es]) cube([ls,av+motordeep,es]);
            translate([25.5+22+ls-driftstepper,-av+0.5,lo-es]) cube([ls,av+motordeep,es]);
            translate([30.5-driftstepper,-5,lo-17.5]) cube([22,7,17.5]);
            //color("green") translate([25.5+2.5,-12,33]) cube([27,1,1])
            plot(ep);
            
        }
        //trous
        translate([5.5,0,3.5]) translate([0,ep/2,0]) rotate([90,0,0])  trou(3,ep);
        translate([5.5+24.004,0,3.5]) translate([0,ep/2,0]) rotate([90,0,0])  trou(3,ep);
        translate([5.5+24.004,0,3.5+14.5]) translate([0,ep/2,0]) rotate([90,0,0])  trou(3,ep);
        translate([5.5,0,3.5+14.5]) translate([0,ep/2,0]) rotate([90,0,0])  trou(3,ep);
        translate([25.5+ls/2-driftstepper,-12.5/2-5,lo-es/2]) trou(3,es);
        translate([25.5+22+ls+ls/2-driftstepper,-12.5/2-5,lo-es/2]) trou(3,es);    
    }
}
//==============

module front(){
    //color("yellow")translate ([10,0,39])cube([1,1,motorh]);
    ox = 17;
    oy = -27;
    oz = 9.95;
    h = 48.5;
    m=11.45;
    platic=12.7;
    motorh=15.4;
    translate([ox,oy,oz]) union(){
        difference(){
            union() {
                
                translate ([0,0,7.5]) cylinder(h=15,d=14,center=true);
                translate ([-3,4.5,0])cube([6,2.5,h]);
                translate ([-3,-7.5,0])cube([6,2.5,15]);
                translate ([-7,0,0])cube([14,15.5,5]);
                //rotate([0,0,90])color("green")translate ([13.8,-12,43.5])cube([4,12.7,1.6]); 
                translate ([-3,4.5,h-2.5])cube([6,m+4,2.5]);
                translate ([-3,m+5+1,h-6])cube([6,2.5,4.5]);
                translate ([-3,m+5-6,h-6])cube([6,2.5,4.5]);
                
                
                
            difference() {
                union() {
             
                    translate([3.5+0.1-1.75,17-1-3,0]) cube([3.5,2.5+1+3,5]);
                    translate([3.5+0.1,2.5+17,5/2]) rotate([90,0,90]) cylinder(h=3.5,d=5,center=true);
        
        
                }
                translate([3.5+0.1,2.5+17,5/2]) rotate([90,0,90]) cylinder(h=3.5+2*eps,d=3.3,center=true);
            }
                
                
            difference() {
                union() {
                    translate([-3.5-0.1-1.75,17-1-3,0]) cube([3.5,2.5+1+3,5]);
                    translate([-3.5-0.1,2.5+17,5/2]) rotate([90,0,90]) cylinder(h=3.5,d=5,center=true);
        
                }
                translate([-3.5-0.1,2.5+17,5/2]) rotate([90,0,90]) cylinder(h=3.5+2*eps,d=3.3,center=true);
            }    
                
                }
            union() {  
                translate ([0,0,7.5]) cylinder(h=15+2*eps,d=9,center=true);
                translate ([0,-5,8])rotate([90,0,0])trou(3,5);
                
            }
        }
    }
}



front();
plaque(); 


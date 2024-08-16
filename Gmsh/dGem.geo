SetFactory("OpenCASCADE");
// Define some constants
t = 0.57;//thickness
ct = 0.002;//cladding thickness
r = 0.17/2;//radius
p = 0.28;//pitch
dl = 2.0;//drift length
il = 2.0;//induction length
tl = 2.0;//transfer length
h = Sqrt(3) * p;
total_thickness = dl + ct*2 + t + il;

Box(1) = {0,0,0,p,h,t};
Extrude{0,0,-ct}{Surface{5};}
Extrude{0,0,ct}{Surface{6};}

Cylinder(4) = {0,0,-ct,0,0,t+2*ct, r, Pi/2};
Cylinder(5) = {p,0,-ct,0,0,t+2*ct, r, Pi/2};
Rotate{{0,0,1},{p,0,0},Pi/2}{Volume{5};}
Cylinder(6) = {p/2,h/2,-ct,0,0,t+2*ct, r, 2*Pi};
Cylinder(7) = {0,h,-ct,0,0,t+2*ct, r, Pi/2};
Rotate{{0,0,1},{0,h,0},3*Pi/2}{Volume{7};}
Cylinder(8) = {p,h,-ct,0,0,t+2*ct, r, Pi/2};
Rotate{{0,0,1},{p,h,0},Pi}{Volume{8};}

BooleanDifference{Volume{1:3};Delete;}{Volume{4:8};Delete;}

//second GEM
Translate{0,0,tl+t+2*ct}{Duplicata{Volume{1:3};}}

//gas
Box(7) = {0,0,-ct-dl,p,h,dl+il+tl+2*(t+2*ct)};

BooleanDifference{Volume{7};Delete;}{Volume{1:6};}

Physical Surface("Cathode") = {96};
Physical Surface("Anode") = {97};

Physical Volume("2GTv") = {3};
Physical Volume("2GBv") = {2};
Physical Volume("2Glass") = {1};

Physical Volume("1GTv") = {6};
Physical Volume("1GBv") = {5};
Physical Volume("1Glass") = {4};

Physical Volume("Gas") = {7};

Physical Surface("2GTs") = {2,22:31};
Physical Surface("2GBs") = {4,12:21};
Physical Surface("1GTs") = {64,84:93};
Physical Surface("1GBs") = {66,74:83};

Field[1] = Box;
Field[1].VIn = 0.02;
Field[1].VOut = 0.065;
Field[1].XMin = -0.01;
Field[1].XMax = p+0.01;
Field[1].YMin = -0.01;
Field[1].YMax = h+0.01;
Field[1].ZMin = -ct-0.1;
Field[1].ZMax = t+ct+tl+t+2*ct+0.1;
Background Field = 1;

Physical Surface("A") = {98, 6, 16, 26, 78, 68, 88};
Physical Surface("B") = {94, 22, 1, 12, 63, 84, 74};
Physical Surface("C") = {95, 20, 10, 30, 72, 82, 92};
Physical Surface("D") = {99, 90, 70, 80, 8, 18, 28};


/** This function creates the CNRG 2009 map */
void mapSpecInit(){
    navMap[nStart].dirs[NORTH] = n1;
    navMap[nStart].paths[NORTH] = 110;      // in CM
    navMap[nStart].dirs[WEST] = n6;
    navMap[nStart].paths[WEST] = 116;
    navMap[nStart].dist = 0;                // is goal
   
    navMap[n1].dirs[SOUTH] = nStart;
    navMap[n1].paths[SOUTH] = 110;             
    navMap[n1].dirs[WEST] = n2;
    navMap[n1].paths[WEST] = 70;
    navMap[n1].dist = 1;
    navMap[n1].flags = VISITED;
    
    navMap[RM1].dirs[NORTH] = n2;
    navMap[RM1].flags = IS_ROOM;
    navMap[RM1].dist = 100;

    navMap[n2].dirs[WEST] = n3;
    navMap[n2].paths[WEST] = 46;
    navMap[n2].dirs[EAST] = n1;
    navMap[n2].paths[EAST] = 70;
    navMap[n2].dirs[SOUTH] = RM1;           // we should follow right wall in
    navMap[n2].dist = 2;
    
    navMap[n3].dirs[SOUTH] = n4;
    navMap[n3].paths[SOUTH] = 57; //80;
    navMap[n3].dirs[EAST] = n1;
    navMap[n3].paths[EAST] = 46;
    navMap[n3].dist = 3;
    
    navMap[n4].dirs[SOUTH] = n8;
    navMap[n4].paths[SOUTH] = 134;
    navMap[n4].dirs[NORTH] = n3;
    navMap[n4].paths[NORTH] = 80;
    navMap[n4].dirs[WEST] = n5;
    navMap[n4].paths[WEST] = 72;
    navMap[n4].dist = 2;
	
    navMap[n5].dirs[EAST] = n4;
    navMap[n5].paths[EAST] = 72;
    navMap[n5].dirs[NORTH] = RM2;
    navMap[n5].dist = 3;
    
    navMap[RM2].dirs[SOUTH] = n5;
    navMap[RM2].flags = IS_ROOM;
    navMap[RM2].dist = 100;

    navMap[n6].dirs[EAST] = nStart;
    navMap[n6].paths[EAST] = 118;
    navMap[n6].dirs[NORTH] = n4;
    navMap[n6].paths[NORTH] = 46;
    navMap[n6].dirs[SOUTH] = n8;          // skip n7
    navMap[n6].paths[SOUTH] = 88;
    navMap[n6].dist = 1;
  
    navMap[n7].dirs[NORTH] = n6;
    navMap[n7].paths[NORTH] = 46;
    navMap[n7].dirs[EAST] = RM4;
    navMap[n7].dist = 2;
    
    navMap[RM4].dirs[WEST] = n7;
    navMap[RM4].flags = IS_ROOM;
    navMap[RM4].dist = 100;

    navMap[n8].dirs[NORTH] = n7;
    navMap[n8].paths[NORTH] = 46;
    navMap[n8].dirs[WEST] = RM3;
    navMap[n8].dist = 3;    
    
    navMap[RM3].dirs[EAST] = n8;
    navMap[RM3].flags = IS_ROOM;
    navMap[RM3].dist = 100;

    // Now initialize for first node
    navMap[nStart].flags = VISITED;
    lnode = nStart;
    lheading = WEST;
    //odometer = navMap[nStart].
    odometer = navMap[lnode].paths[lheading] - NBHD_SIZE;	
    BREG = CHECK_FORWARD;
}


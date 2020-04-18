/** This function creates the Trinity 2010 map */
void mapSpecInit(){   
    // Trinity specific setup
    navMap[nStart].dirs[NORTH] = n1;
    navMap[nStart].paths[NORTH] = 120;        // in CM
    navMap[nStart].dirs[EAST] = n4;
    navMap[nStart].paths[EAST] = 105; // 112;   	
    navMap[nStart].dist = 0;                  // is goal
	
    navMap[n1].dirs[SOUTH] = nStart;
    navMap[n1].paths[SOUTH] = 120; 			
    navMap[n1].dirs[EAST] = n2;
    navMap[n1].paths[EAST] = 105; //112; 
    navMap[n1].dist = 1;
	
    navMap[n2].dirs[WEST] = n1;
    navMap[n2].paths[WEST] = 112;
    navMap[n2].dirs[SOUTH] = n3;
    navMap[n2].paths[SOUTH] = 69; //74; 
    navMap[n2].dist = 2;
	
    navMap[n3].dirs[NORTH] = n2;
    navMap[n3].paths[NORTH] = 74;
    navMap[n3].dirs[WEST] = RM1;
    navMap[n3].dirs[SOUTH] = n4;
    navMap[n3].paths[SOUTH] = 46;
    navMap[n3].dist = 2;
		
    navMap[RM1].dirs[EAST] = n3;
    navMap[RM1].flags = IS_ROOM;
    navMap[RM1].dist = 101;
		
    navMap[n4].dirs[NORTH] = n3;
    navMap[n4].paths[NORTH] = 46;
    navMap[n4].dirs[WEST] = nStart;
    navMap[n4].paths[WEST] = 112;
    navMap[n4].dirs[SOUTH] = n5;
    navMap[n4].paths[SOUTH] = 72;
    //navMap[n4].dirs[EAST] = n6;
    //navMap[n4].paths[EAST] = 56;
    navMap[n4].dirs[EAST] = n7;
    navMap[n4].paths[EAST] = 92;
    navMap[n4].dist = 1;
	
    navMap[n5].dirs[NORTH] = n4;
    navMap[n5].paths[NORTH] = 76;  // TODO: Should be 72? (discrepancy with line 39)
    navMap[n5].dirs[WEST] = RM2;
    navMap[n5].dist = 2;
	
    navMap[RM2].dirs[EAST] = n5;
    navMap[RM2].flags = IS_ROOM;
    navMap[RM2].dist = 102;

    navMap[n6].dirs[NORTH] = RM3;
    navMap[n6].dirs[WEST] = n4;
    navMap[n6].paths[WEST] = 46;  // TODO: Should be 56? (discrepancy with line 41)			
    navMap[n6].dirs[EAST] = n7;
    navMap[n6].paths[EAST] = 46;	
    navMap[n6].dist = 2;
		
    navMap[RM3].dirs[SOUTH] = n6;
    navMap[RM3].flags = IS_ROOM;
    navMap[RM3].dist = 102;
	
    navMap[n7].dirs[WEST] = n6;
    navMap[n7].paths[WEST] = 46;
    navMap[n7].dirs[SOUTH] = RM4;
    navMap[n7].dist = 3;
	
    navMap[RM4].dirs[NORTH] = n7;
    navMap[RM4].flags = IS_ROOM;
    navMap[RM4].dist = 102;
	
    // Now initialize for first node
    navMap[nStart].flags = VISITED;
    lnode = nStart;
    lheading = EAST;      
    odometer = navMap[lnode].paths[lheading] - NBHD_SIZE;	
    BREG = CHECK_RIGHT;
}


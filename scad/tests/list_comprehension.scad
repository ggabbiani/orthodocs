steps = 50;

points = [
	// first expression generating the points in the positive Y quadrant
	for (a = [0 : steps]) [ a, 10 * sin(a * 360 / steps) + 10 ],
	// second expression generating the points in the negative Y quadrant
	for (a = [steps : -1 : 0]) [ a, 10 * cos(a * 360 / steps) - 20 ]
	// additional list of fixed points
	[ 10, -3 ], [ 3, 0 ], [ 10, 3 ]
];

polygon(points);

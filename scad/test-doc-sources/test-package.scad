// general 2d polygon bounding box
function fl_bb_polygon(points) = let(
  x = [for(p=points) p.x],
  y = [for(p=points) p.y]
) [[min(x),min(y)],[max(x),max(y)]];

function pi() = PI;

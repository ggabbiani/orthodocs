function fl_grid_quad(
  //! bounding box relative grid origin
  origin=[0,0],
  //! 2d deltas
  step,
  //! used for clipping the out of region points
  bbox,
  //! generator (default generator just returns its center resulting in a quad grid ... hence the name)
  generator=function(point,bbox) [point]
) = let(
  low   = bbox[0],
  high  = bbox[1],
  step  = is_num(step) ? [step,step] : step
) [
    for(y=[low.y+origin.y:step.y:high.y])
      for(x=[low.x+origin.x:step.x:high.x],p=generator([x,y],bbox))
        p
  ];

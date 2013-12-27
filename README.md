pyplatec
========

Plate simulation for python (wrapper of an existing C/C++ library).

Take a look at the simulation at work: http://www.youtube.com/watch?v=bi4b45tMEPE#t=0

The original project is here: http://sourceforge.net/projects/platec/ . That project is part of a Bachelor of Engineering thesis in Metropolia University of Applied Sciences, Helsinki, Finland. The thesis is freely downloadable from http://urn.fi/URN:NBN:fi:amk-201204023993 .

Kudos to the original author!

usage
=====

The library is quite simple:

    p = platec.create(seed=3)
    while platec.is_finished(p)==0:
        platec.step(p)
    hm = platec.get_heightmap(p)
    platec.destroy(p)


Or if you want more control:

    p = platec.create(seed=3,map_side=512,sea_level=0.65,erosion_period=60,
                      folding_ratio=0.02,aggr_overlap_abs=1000000,
                      aggr_overlap_rel=0.33,cycle_count=2,num_plates=10)

Enjoy!

# Campus data

Two CSV files hold the campus graph. Buildings are the vertices, pathways are
the edges. 39 buildings and 151 pathways.

## buildings.csv

    id,code,name,lat,lon,x,y

- `id` - starts at 0 and goes up by 1. The id is the building's position in
  the buildings array.
- `code` - the code Northeastern uses on its campus map, such as SL for Snell
  Library.
- `name` - full building name.
- `lat`, `lon` - latitude and longitude. Kept so a row can be checked on a
  map. The program does not read these.
- `x`, `y` - position in meters, rounded to the nearest meter. x is east, y
  is north. The origin is the centre of Snell Library, at 42.3383406,
  -71.0879537. Snell Library's own row is at its main entrance, so it sits at
  (-17, 17) rather than (0, 0). The program uses these because A* needs
  distances in meters.

To convert:

    x = (longitude - origin longitude) * 82286
    y = (latitude  - origin latitude)  * 111320

82286 and 111320 are meters per degree at Boston's latitude.

Buildings 0 to 22 sit at their main entrance where OpenStreetMap records one.
Six of them have no entrance recorded, so they sit at the centre of the
building instead: Behrakis, Hurtig, Hastings, West Village C, F and H.
Buildings 23 to 38 all sit at the centre of the building. Distances involving
any of these read slightly long.

Dana Research Center is attached to the Forsyth Building, so the distance
between them is the walk around the outside, not the way through.

## pathways.csv

    from,to,distance,has_stairs,is_closed,is_indoor,source

- `from`, `to` - building ids. Pathways go both ways, so one row covers both
  directions.
- `distance` - walking distance in meters.
- `has_stairs` - 1 if the route has steps.
- `is_closed` - 1 if the pathway is closed.
- `is_indoor` - 1 if the route goes through the campus tunnels.
- `source` - where the distance came from.
  - `osm_routed` - measured by running Dijkstra over the OpenStreetMap walkway
    network and adding up the real path segments. 139 rows.
  - `google_maps` - read from Google Maps walking directions. 1 row, Snell
    Library to Churchill Hall, where the routed figure did not match.
  - `estimated` - no measured source. 11 rows: the nine tunnels and the two
    routes over the ISEC bridge, none of which OpenStreetMap maps. Each tunnel
    distance is the surface distance for the same pair plus 5 to 19 m (15 m
    for most) for going down and back up, since a tunnel runs under the
    buildings and cannot be shorter than the walk above. The two bridge routes
    are estimates of the walk from Snell Library over the bridge to ISEC.

Ten pairs of buildings appear on two rows, because there are two different
ways to make the trip. Snell Library to ISEC has a 530 m route with stairs and
a 560 m step-free route using the lift. Nine pairs have a surface route and a
tunnel route.

## Closed pathways

One pathway is closed: the surface path between Snell Library and Curry
Student Center. Routing between them returns the 86 m tunnel instead of the
72 m walk across the plaza.

## Test files

These are not program data. `tests/tests.cpp` loads both of them.

- `test_no_route_pathways.csv` - a copy of pathways.csv with both of EXP's
  pathways closed, so EXP cannot be reached and the program has to say there
  is no route.
- `test_stairs_only_pathways.csv` - a copy of pathways.csv with both of EXP's
  pathways marked as having stairs, so EXP can be reached normally but not
  under NO_STAIRS, and the program has to say there is no step-free route.

## Accessibility preferences

- STANDARD - shortest route, skipping closed pathways.
- NO_STAIRS - also skips any pathway with steps.
- INDOOR_PREFERRED - outdoor pathways cost 1.5 times their distance, so
  tunnels win when they are not much longer. Nothing else is excluded.

Closed pathways are skipped under all three preferences. The distance shown
to the user is always the real walking distance, not the 1.5 weighted cost.

## Rule that must hold

No pathway may be shorter than the straight-line distance between the two
buildings it joins. A* estimates the distance left to go as a straight line,
and if a pathway were shorter than that the estimate would be too high and A*
could return a route that is not the shortest. All 151 rows were checked,
and `loadCampusData()` checks every row again when it loads the file and stops
with an error if one breaks the rule.

## Sources

- Building positions, entrances and footpaths: OpenStreetMap, queried through
  the Overpass API. (c) OpenStreetMap contributors, available under the Open
  Database License. The data in this folder is derived from OpenStreetMap and
  is available under the same license.
- Building codes and the tunnel network: Northeastern University campus map.
- Accessible entrances: Northeastern University Campus Accessibility map,
  July 2018.

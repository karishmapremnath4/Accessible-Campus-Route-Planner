# Campus data

Two CSV files describe the campus graph. Buildings are the vertices, pathways
are the edges. The graph holds 39 buildings and 151 pathways.

## buildings.csv

    id,code,name,lat,lon,x,y

- `id` - integer, starts at 0, increases by 1 with no gaps. The id is the
  building's index in the buildings vector.
- `code` - the building code Northeastern uses on its own campus map, e.g. SL
  for Snell Library, SN for Snell Engineering Center.
- `name` - full building name.
- `lat`, `lon` - latitude and longitude, kept so the positions can be checked
  against a map. The program does not read these columns.
- `x`, `y` - position in meters on a flat campus grid, converted from `lat`
  and `lon`. x increases to the east, y to the north. The origin is the
  centroid of Snell Library, 42.3383406, -71.0879537. These are the columns
  the program uses, because the A* heuristic needs distances in meters.

Buildings 0 to 22 are placed at their mapped main entrance where
OpenStreetMap records one. Six of those have no entrance recorded and sit at
the centre of the building instead: Behrakis, Hurtig, Hastings, West Village
C, F and H. Buildings 23 to 38 were added later and all sit at the centre of
the building, so distances involving them read slightly long.

Every building is matched to a distinct point on the walkway network. Dana
Research Center is physically attached to the Forsyth Building, so its walking
distance to Forsyth is the route around the outside rather than the internal
connection.

## pathways.csv

    from,to,distance,has_stairs,is_closed,is_indoor,source

- `from`, `to` - building ids. Pathways are two-way, so one row covers both
  directions.
- `distance` - walking distance in meters.
- `has_stairs` - 1 if the route includes steps, 0 if not.
- `is_closed` - 1 if temporarily closed, 0 if open.
- `is_indoor` - 1 if the route runs through the campus tunnels, 0 if outdoors.
- `source` - how the distance was obtained. `osm_routed` means it was measured
  by running Dijkstra over the OpenStreetMap walkway network, summing the real
  path segments. `estimated` means no measured source was available. This
  applies to the nine tunnel routes, which OpenStreetMap does not map, and to
  the two Snell Library to ISEC routes over the pedestrian bridge. Tunnel
  distances are the measured surface distance for the same pair plus roughly
  15 m for the descent and ascent at each end, because a tunnel runs beneath
  the buildings and cannot be shorter than the walk above it. Where no surface
  route was measured, the estimate is 1.6 times the straight-line distance.
  `google_maps` means the distance was read from Google Maps walking directions
  between the two buildings; this applies to one row, Snell Library to Churchill
  Hall, where the routed figure disagreed with the measured walking route.

A pair of buildings may appear on more than one row. That is deliberate: it
represents two genuinely different ways of making the same trip. Snell Library
to ISEC has a 530 m route with stairs and a 560 m step-free route using the
lift. Nine pairs have both a surface route and a tunnel route.

## Accessibility preferences

The program filters and weights pathways according to the preference the user
selects:

- `STANDARD` - every open pathway is allowed, shortest distance wins.
- `NO_STAIRS` - pathways with `has_stairs = 1` are excluded entirely.
- `INDOOR_PREFERRED` - every pathway is allowed, but outdoor pathways are
  given a weight penalty so that a tunnel is chosen when one exists. Useful
  in winter. The distance reported to the user is the real distance, not the
  penalised weight.

Closed pathways are excluded under all preferences.

## Getting the coordinates

Latitude and longitude come from OpenStreetMap. Convert to meters with:

    x = (longitude - longitude_of_origin) * 82286
    y = (latitude  - latitude_of_origin)  * 111320

82286 and 111320 are meters per degree of longitude and latitude at Boston's
latitude.

## Rule that must hold

Every pathway's `distance` must be at least the straight-line distance between
its two buildings:

    distance >= sqrt((x1-x2)^2 + (y1-y2)^2)

Walking routes bend around buildings, so they are always longer than the
straight line. A* uses straight-line distance to estimate the remaining walk,
and that estimate is only safe if it never exceeds the real distance. A row
that breaks this rule can make A* return a route that is not the shortest.

## Sources

- Building positions, entrances and footpaths: OpenStreetMap, queried through
  the Overpass API. (c) OpenStreetMap contributors, available under the Open
  Database License.
- Building codes and the tunnel network: Northeastern University campus map.
- Accessible entrances: Northeastern University Campus Accessibility map,
  July 2018.

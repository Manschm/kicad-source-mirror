// Do not edit this file, it is autogenerated by CMake from the .md file
_HKI( "### Top-level Clauses\n"
"\n"
"    (version <number>)\n"
"\n"
"    (rule <rule_name> <rule_clause> ...)\n"
"\n"
"\n"
"<br>\n"
"\n"
"### Rule Clauses\n"
"\n"
"    (constraint <constraint_type> ...)\n"
"\n"
"    (condition \"<expression>\")\n"
"\n"
"    (layer \"<layer_name>\")\n"
"\n"
"    (severity <severity_name>)\n"
"\n"
"\n"
"<br>\n"
"\n"
"### Constraints\n"
"\n"
"| Constraint type           | Argument type                                                                                                         | Description                                                                                                                                                                                                                                                                                                                                                                                                                                            |\n"
"|---------------------------|-----------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|\n"
"| `annular_width`           | min/opt/max                                                                                                           | Checks the width of annular rings on vias.<br>                                                                                                                                                                                                                                                                                                                                                                                                         |\n"
"| `assertion`               | \"&lt;expression>\"                                                                                                     | Checks the given expression.<br>                                                                                                                                                                                                                                                                                                                                                                                                  |\n"
"| `clearance`               | min                                                                                                                   | Specifies the **electrical** clearance between copper objects of different nets.  (See `physical_clearance` if you wish to specify clearance between objects regardless of net.)<br><br>To allow copper objects to overlap (collide), create a `clearance` constraint with the `min` value less than zero (for example, `-1`).<br>                                                                                                                     |\n"
"| `courtyard_clearance`     | min                                                                                                                   | Checks the clearance between footprint courtyards and generates an error if any two courtyards are closer than the `min` distance.  If a footprint does not have a courtyard shape, no errors will be generated from this constraint.<br>                                                                                                                                                                                                              |\n"
"| `diff_pair_gap`           | min/opt/max                                                                                                           | Checks the gap between coupled tracks in a differential pair.  Coupled tracks are segments that are parallel to each other.  Differential pair gap is not tested on uncoupled portions of a differential pair (for example, the fanout from a component).<br>                                                                                                                                                                                          |\n"
"| `diff_pair_uncoupled`     | max                                                                                                                   | Checks the distance that a differential pair track is routed uncoupled from the other polarity track in the pair (for example, where the pair fans out from a component, or becomes uncoupled to pass around another object such as a via).<br>                                                                                                                                                                                                        |\n"
"| `disallow`                | `track`<br>`via`<br>`micro_via`<br>`buried_via`<br>`pad`<br>`zone`<br>`text`<br>`graphic`<br>`hole`<br>`footprint`<br> | Specify one or more object types to disallow, separated by spaces.  For example, `(constraint disallow track)` or `(constraint disallow track via pad)`.  If an object of this type matches the rule condition, a DRC error will be created.<br><br>This constraint is essentially the same as a keepout rule area, but can be used to create more specific keepout restrictions.<br>                                                                  |\n"
"| `edge_clearance`          | min/opt/max                                                                                                           | Checks the clearance between objects and the board edge.<br><br>This can also be thought of as the \"milling tolerance\" as the board edge will include all graphical items on the `Edge.Cuts` layer as well as any *oval* pad holes.  (See `physical_hole_clearance` for the drilling tolerance.)<br>                                                                                                                                                   |\n"
"| `length`                  | min/max                                                                                                               | Checks the total routed length for the nets that match the rule condition and generates an error for each net that is below the `min` value (if specified) or above the `max` value (if specified) of the constraint.<br>                                                                                                                                                                                                                              |\n"
"| `hole`                    | min/max                                                                                                               | Checks the size (diameter) of a drilled hole in a pad or via.  For oval holes, the smaller (minor) diameter will be tested against the `min` value (if specified) and the larger (major) diameter will be tested against the `max` value (if specified).<br>                                                                                                                                                                                           |\n"
"| `hole_clearance`          | min                                                                                                                   | Checks the clearance between a drilled hole in a pad or via and copper objects on a different net.  The clearance is measured from the diameter of the hole, not its center.<br>                                                                                                                                                                                                                                                                       |\n"
"| `hole_to_hole`            | min                                                                                                                   | Checks the clearance between mechanically-drilled holes in pads and vias.  The clearance is measured between the diameters of the holes, not between their centers.<br><br>This constraint is soley for the protection of drill bits.  The clearance between **laser-drilled** (microvias) and other non-mechanically-drilled holes is not checked, nor is the clearance between **milled** (oval-shaped) and other non-mechanically-drilled holes.<br> |\n"
"| `physical_clearance`      | min                                                                                                                   | Checks the clearance between two objects on a given layer (including non-copper layers).<br><br>While this can perform more general-purpose checks than `clearance`, it is much slower.  Use `clearance` where possible.<br>                                                                                                                                                                                                                           |\n"
"| `physical_hole_clearance` | min                                                                                                                   | Checks the clearance between a drilled hole in a pad or via and another object, regardless of net. The clearance is measured from the diameter of the hole, not its center.<br><br>This can also be thought of as the \"drilling tolerance\" as it only includes **round** holes (see `edge_clearance` for the milling tolerance).<br>                                                                                                                   |\n"
"| `silk_clearance`          | min/opt/max                                                                                                           | Checks the clearance between objects on silkscreen layers and other objects.<br>                                                                                                                                                                                                                                                                                                                                                                       |\n"
"| `skew`                    | max                                                                                                                   | Checks the total skew for the nets that match the rule condition, that is, the difference between the length of each net and the average of all the lengths of each net that is matched by the rule.  If the absolute value of the difference between that average and the length of any one net is above the constraint `max` value, an error will be generated.<br>                                                                                  |\n"
"| `thermal_relief_gap`      | min                                                                                                                   | Specifies the width of the gap between a pad and a zone with a thermal-relief connection.<br>                                                                                                                                                                                                                                                                                                                                                          |\n"
"| `thermal_spoke_width`     | opt                                                                                                                   | Specifies the width of the spokes connecting a pad to a zone with a thermal-relief connection.<br>                                                                                                                                                                                                                                                                                                                                                     |\n"
"| `track_width`             | min/opt/max                                                                                                           | Checks the width of track and arc segments.  An error will be generated for each segment that has a width below the `min` value (if specified) or above the `max` value (if specified).<br>                                                                                                                                                                                                                                                            |\n"
"| `via_count`               | max                                                                                                                   | Counts the number of vias on every net matched by the rule condition.  If that number exceeds the constraint `max` value on any matched net, an error will be generated for that net.<br>                                                                                                                                                                                                                                                              |\n"
"| `zone_connection`         | `solid`<br>`thermal_reliefs`<br>`none`                                                                                | Specifies the connection to be made between a zone and a pad.<br>                                                                                                                                                                                                                                                                                                                                                                                      |\n"
"\n"
"\n"
"### Items\n"
"\n"
" * `A` &nbsp;&nbsp; _the first (or only) item under test_\n"
" * `B` &nbsp;&nbsp; _the second item under test (for binary tests)_\n"
" * `L` &nbsp;&nbsp; _the layer currently under test_\n"
"\n"
"<br>\n"
"\n"
"### Severity Names\n"
"\n"
" * warning\n"
" * error\n"
" * exclusion\n"
" * ignore\n"
"\n"
"<br>\n"
"\n"
"### Examples\n"
"\n"
"    (version 1)\n"
"\n"
"    (rule HV\n"
"       (constraint clearance (min 1.5mm))\n"
"       (condition \"A.NetClass == 'HV'\"))\n"
"\n"
"\n"
"    (rule HV\n"
"       (layer outer)\n"
"       (constraint clearance (min 1.5mm))\n"
"       (condition \"A.NetClass == 'HV'\"))\n"
"\n"
"\n"
"    (rule HV_HV\n"
"       # wider clearance between HV tracks\n"
"       (constraint clearance (min \"1.5mm + 2.0mm\"))\n"
"       (condition \"A.NetClass == 'HV' && B.NetClass == 'HV'\"))\n"
"\n"
"\n"
"    (rule HV_unshielded\n"
"       (constraint clearance (min 2mm))\n"
"       (condition \"A.NetClass == 'HV' && !A.enclosedByArea('Shield*')\"))\n"
"\n"
"\n"
"    (rule heavy_thermals\n"
"       (constraint thermal_spoke_width (min 0.5mm))\n"
"       (condition \"A.NetClass == 'HV'\"))\n"
"<br><br>\n"
"\n"
"### Notes\n"
"\n"
"Version clause must be the first clause.  It indicates the syntax version of the file so that \n"
"future rules parsers can perform automatic updates.  It should be\n"
"set to \"1\".\n"
"\n"
"Rules should be ordered by specificity.  Later rules take\n"
"precedence over earlier rules; once a matching rule is found\n"
"no further rules will be checked.\n"
"\n"
"Use Ctrl+/ to comment or uncomment line(s).\n"
"<br><br><br>\n"
"\n"
"### Expression functions\n"
"\n"
"All function parameters support simple wildcards (`*` and `?`).\n"
"<br><br>\n"
"\n"
"    A.intersectsCourtyard('<footprint_refdes>')\n"
"True if any part of `A` lies within the given footprint's principal courtyard.\n"
"<br><br>\n"
"\n"
"    A.intersectsFrontCourtyard('<footprint_refdes>')\n"
"True if any part of `A` lies within the given footprint's front courtyard.\n"
"<br><br>\n"
"\n"
"    A.intersectsBackCourtyard('<footprint_refdes>')\n"
"True if any part of `A` lies within the given footprint's back courtyard.\n"
"<br><br>\n"
"\n"
"    A.intersectsArea('<zone_name>')\n"
"True if any part of `A` lies within the given zone's outline.\n"
"<br><br>\n"
"\n"
"    A.enclosedByArea('<zone_name>')\n"
"True if all of `A` lies within the given zone's outline.\n"
"\n"
"NB: this is potentially a more expensive call than `intersectsArea()`.  Use `intersectsArea()`\n"
"where possible.\n"
"<br><br>\n"
"\n"
"    A.getField('<field_name>')\n"
"The value of the given field. Only footprints have fields, so a field is only returned if\n"
"`A` is a footprint.\n"
"<br><br>\n"
"\n"
"    A.isPlated()\n"
"True if `A` has a hole which is plated.\n"
"<br><br>\n"
"\n"
"    A.inDiffPair('<net_name>')\n"
"True if `A` has a net that is part of the specified differential pair.\n"
"`<net_name>` is the base name of the differential pair.  For example, `inDiffPair('/CLK')`\n"
"matches items in the `/CLK_P` and `/CLK_N` nets.\n"
"<br><br>\n"
"\n"
"    AB.isCoupledDiffPair()\n"
"True if `A` and `B` are members of the same diff pair.\n"
"<br><br>\n"
"\n"
"    A.memberOfGroup('<group_name>')\n"
"True if `A` is a member of the given group. The name can contain wildcards.\n"
"Includes nested membership.\n"
"<br><br>\n"
"\n"
"    A.memberOfFootprint('<footprint_reference>|<footprint_id>')\n"
"True if `A` is a member of a footprint matching the given reference designator or footprint\n"
"ID.  The parameter can contain wildcards.\n"
"\n"
"NB: If matching against a footprint ID is desired, the parameter must contain a ':'.\n"
"<br><br>\n"
"\n"
"    A.memberOfSheet('<sheet_path>')\n"
"True if `A` is a member of the given schematic sheet. The sheet path can contain wildcards.\n"
"<br><br>\n"
"\n"
"    A.existsOnLayer('<layer_name>')\n"
"True if `A` exists on the given layer.  The layer name can be\n"
"either the name assigned in Board Setup > Board Editor Layers or\n"
"the canonical name (ie: `F.Cu`).\n"
"\n"
"NB: this returns true if `A` is on the given layer, independently\n"
"of whether or not the rule is being evaluated for that layer.\n"
"For the latter use a `(layer \"layer_name\")` clause in the rule.\n"
"<br><br>\n"
"\n"
"### More Examples\n"
"\n"
"    (rule \"copper keepout\"\n"
"       (constraint disallow track via zone)\n"
"       (condition \"A.intersectsArea('zone3')\"))\n"
"\n"
"\n"
"    (rule \"BGA neckdown\"\n"
"       (constraint track_width (min 0.2mm) (opt 0.25mm))\n"
"       (constraint clearance (min 0.05mm) (opt 0.08mm))\n"
"       (condition \"A.intersectsCourtyard('U3')\"))\n"
"\n"
"\n"
"    # prevent silk over tented vias\n"
"    (rule silk_over_via\n"
"       (constraint silk_clearance (min 0.2mm))\n"
"       (condition \"A.Type == '*Text' && B.Type == 'Via'\"))\n"
"\n"
"\n"
"    (rule \"Distance between Vias of Different Nets\"\n"
"        (constraint hole_to_hole (min 0.254mm))\n"
"        (condition \"A.Type == 'Via' && B.Type == 'Via' && A.Net != B.Net\"))\n"
"\n"
"    (rule \"Clearance between Pads of Different Nets\"\n"
"        (constraint clearance (min 3.0mm))\n"
"        (condition \"A.Type == 'Pad' && B.Type == 'Pad' && A.Net != B.Net\"))\n"
"\n"
"\n"
"    (rule \"Via Hole to Track Clearance\"\n"
"        (constraint hole_clearance (min 0.254mm))\n"
"        (condition \"A.Type == 'Via' && B.Type == 'Track'\"))\n"
"\n"
"    (rule \"Pad to Track Clearance\"\n"
"        (constraint clearance (min 0.2mm))\n"
"        (condition \"A.Type == 'Pad' && B.Type == 'Track'\"))\n"
"\n"
"\n"
"    (rule \"clearance-to-1mm-cutout\"\n"
"        (constraint edge_clearance (min 0.8mm))\n"
"        (condition \"A.Layer == 'Edge.Cuts' && A.Line_Width == 1.0mm\"))\n"
"\n"
"\n"
"    (rule \"Max Drill Hole Size Mechanical\"\n"
"        (constraint hole_size (max 6.3mm))\n"
"        (condition \"A.Pad_Type == 'NPTH, mechanical'\"))\n"
"\n"
"    (rule \"Max Drill Hole Size PTH\"\n"
"        (constraint hole_size (max 6.35mm))\n"
"        (condition \"A.Pad_Type == 'Through-hole'\"))\n"
"\n"
"\n"
"    # Specify an optimal gap for a particular diff-pair\n"
"    (rule \"dp clock gap\"\n"
"        (constraint diff_pair_gap (opt \"0.8mm\"))\n"
"        (condition \"A.inDiffPair('/CLK')\"))\n"
"\n"
"    # Specify a larger clearance around any diff-pair\n"
"    (rule \"dp clearance\"\n"
"        (constraint clearance (min \"1.5mm\"))\n"
"        (condition \"A.inDiffPair('*') && !AB.isCoupledDiffPair()\"))\n"
"\n"
"\n"
"    # Don't use thermal reliefs on heatsink pads\n"
"    (rule heat_sink_pad\n"
"        (constraint zone_connection solid)\n"
"        (condition \"A.Fabrication_Property == 'Heatsink pad'\"))\n"
"\n"
"    # Require all four thermal relief spokes to connect to parent zone\n"
"    (rule fully_spoked_pads\n"
"        (constraint min_resolved_spokes 4))\n"
"\n"
"    # Set thermal relief gap & spoke width for all zones\n"
"    (rule defined_relief\n"
"        (constraint thermal_relief_gap (min 10mil))\n"
"        (constraint thermal_spoke_width (min 12mil)))\n"
"\n"
"    # Override thermal relief gap & spoke width for GND and PWR zones\n"
"    (rule defined_relief_pwr\n"
"        (constraint thermal_relief_gap (min 10mil))\n"
"        (constraint thermal_spoke_width (min 12mil))\n"
"        (condition \"A.Name == 'zone_GND' || A.Name == 'zone_PWR'\"))\n"
"\n"
"    # Prevent copper fills under the courtyards of capacitors\n"
"    (rule no_copper_under_caps\n"
"        (constraint physical_clearance (min 0mm))\n"
"        (condition \"A.Type == 'Zone' && B.Reference == 'C*'\"))\n"
"\n"
"\n"
"    # Prevent solder wicking from SMD pads\n"
"    (rule holes_in_pads\n"
"        (constraint physical_hole_clearance (min 0.2mm))\n"
"        (condition \"B.Pad_Type == 'SMD'\"))\n"
"\n"
"    # Disallow solder mask margin overrides\n"
"    (rule \"disallow solder mask margin overrides\"\n"
"        (constraint assertion \"A.Soldermask_Margin_Override == 0mm\")\n"
"        (condition \"A.Type == 'Pad'\"))\n"
"\n"
"\n"
"    # Enforce a mechanical clearance between components and board edge\n"
"    (rule front_mechanical_board_edge_clearance\n"
"        (layer \"F.Courtyard\")\n"
"        (constraint physical_clearance (min 3mm))\n"
"        (condition \"B.Layer == 'Edge.Cuts'\"))\n"
"\n"
"\n"
"    # Check current-carrying capacity\n"
"    (rule high-current\n"
"        (constraint track_width (min 1.0mm))\n"
"        (constraint connection_width (min 0.8mm))\n"
"        (condition \"A.NetClass == 'Power'\"))\n"
"\n"
"\n"
"    # Separate drill bit and milling cutter size constraints\n"
"    (rule \"Plated through-hole size\"\n"
"        (constraint hole_size (min 0.2mm) (max 6.35mm))\n"
"        (condition \"A.isPlated() && A.Hole_Size_X == A.Hole_Size_Y\"))\n"
"    (rule \"Plated slot size\"\n"
"        (constraint hole_size (min 0.5mm))\n"
"        (condition \"A.isPlated() && A.Hole_Size_X != A.Hole_Size_Y\"))\n"
"\n"
"\n"
"    # Allow blind/buried to micro-via hole-to-hole violations when it is known that\n"
"    # the fab will mechanically drill blind/buried via holes -before- laser drilling \n"
"    # micro-vias.\n"
"    (rule hole_to_hole_uvia_exclusion\n"
"        (condition \"A.Via_Type == 'Blind/buried' && B.Via_Type == 'Micro'\")\n"
"        (constraint hole_to_hole)\n"
"        (severity ignore))\n"
"\n"
"### Documentation\n"
"\n"
"For the full documentation see [https://docs.kicad.org](https://docs.kicad.org/GetMajorMinorVersion/en/pcbnew/pcbnew.html#custom_design_rules)." );

clockblock v0.2 hub
Prepared by Michael Cousins
michael@wileycousins.com

This panel consists of the two boards in our product with surface mount components. We will handle de-panellization. 

Requested specs:
  * ENIG finish and lead-free solder
  * 0.062", 1 oz copper
  * White soldermask
  * Silkscreen on both sides
  * Red silkscreen if possible, otherwise black silkscreen

File summary:
Gerbers:
  * clockblock-hub-B_Cu.gbl      - bottom copper
  * clockblock-hub-B_Mask.gbs    - botton soldermask
  * clockblock-hub-B_SilkS.gbo   - bottom silkscreen
  * clockblock-hub-Edge_Cuts.gbr - board outline
  * clockblock-hub-F_Cu.gtl      - top copper
  * clockblock-hub-F_Mask.gts    - top soldermask
  * clockblock-hub-F_Paste.gtp   - top solderpaste
  * clockblock-hub-F_SilkS.gto   - top silkscreen
Excellon drill files (inches, 2:4 precision, no zero suppression):
  * clockblock-hub-NPTH.drl - drill file for non-plated though holes
  * clockblock-hub.drl      - drill file for plated through holes
Assembly:
  * clockblock-hub-pos.csv - pick and place position file
  * clockblock-hub-bom.csv - bill of materials

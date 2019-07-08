#!/bin/sh

SHAPER_OPTS="--font-funcs=ot  --shapers=ot"
CLUSTER_OPTS="--utf8-clusters"
FORMAT_OPTS="--show-flags --no-glyph-names --output-format=json"
STANDARD_OPTS="$SHAPER_OPTS $CLUSTER_OPTS $FORMAT_OPTS"

hb-shape $STANDARD_OPTS fonts/notonastaliq.ttf "یہ" > fixtures/notonastaliq_U06CC_U06C1.json
hb-shape $STANDARD_OPTS fonts/amiri-regular.ttf "123" > fixtures/amiri-regular_123.json
hb-shape $STANDARD_OPTS --features="+numr" fonts/amiri-regular.ttf "123" > fixtures/amiri-regular_123_numr.json
hb-shape $SHAPER_OPTS $FORMAT_OPTS /Library/Fonts/AppleGothic.ttf "가나다" > fixtures/AppleGothic_korean_issue_22.json

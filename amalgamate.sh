#!/bin/sh
python3 extern/amalgamate/amalgamate.py -c tools/amalgamate_config_all.json -s . -v=yes
python3 extern/amalgamate/amalgamate.py -c tools/amalgamate_config_fwd.json -s . -v=yes

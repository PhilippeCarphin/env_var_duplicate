#!/usr/bin/env python3

import subprocess

cp = subprocess.run('/usr/bin/env', stdout=subprocess.PIPE, universal_newlines=True)
for l in cp.stdout.splitlines():
    if l.startswith('TMPDIR'):
        print(l)
